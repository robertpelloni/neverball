/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <SDL.h>
#include <math.h>
#include <assert.h>

#include "glext.h"
#include "vec3.h"
#include "geom.h"
#include "part.h"
#include "ball.h"
#include "image.h"
#include "audio.h"
#include "config.h"
#include "video.h"

#include "solid_draw.h"
#include "solid_all.h"

#include "game_client.h"
#include "game_common.h"
#include "game_proxy.h"
#include "game_draw.h"
#include "progress.h"

#include "cmd.h"

/*---------------------------------------------------------------------------*/

int game_compat_map;                    /* Client/server map compat flag     */

/*---------------------------------------------------------------------------*/

#define CURR 0
#define PREV 1

static struct game_draw gd[MAX_PLAYERS];
static struct game_lerp gl[MAX_PLAYERS];

/* Ghost Player State */
static struct game_draw ghost_gd;
static struct game_lerp ghost_gl;
static int ghost_active = 0;

struct client_stats {
    float timer;
    int   status;
    int   coins;
    int   jump_ready;
    float dash_charge;
    int   gyro_active;
    float gyro_timer;
};

static struct client_stats stats[MAX_PLAYERS];

static struct cmd_state cs;             /* Command state                     */

struct
{
    int x, y;
} version;                              /* Current map version               */

static float map_min[3];
static float map_max[3];

/*---------------------------------------------------------------------------*/

static void game_run_cmd(const union cmd *cmd)
{
    struct game_draw *cg = &gd[cs.curr_player];
    struct game_lerp *cl = &gl[cs.curr_player];
    struct client_stats *cst = &stats[cs.curr_player];
    int p;

    if (cg->state)
    {
        struct game_view *view = &cl->view[CURR];
        struct game_tilt *tilt = &cl->tilt[CURR];

        struct s_vary *vary = &cg->vary;
        struct v_item *hp;

        float v[4];
        float dt;

        int idx;

        if (cs.next_update)
        {
            for (p = 0; p < MAX_PLAYERS; p++)
            {
                if (gd[p].state)
                    game_lerp_copy(&gl[p]);
            }
            cs.next_update = 0;
        }

        switch (cmd->type)
        {
        case CMD_SET_PLAYER:
            cs.curr_player = cmd->setplayer.player_index;
            if (cs.curr_player < 0) cs.curr_player = 0;
            if (cs.curr_player >= MAX_PLAYERS) cs.curr_player = 0;
            break;

        case CMD_PUNCH:
            cg->punch_active = cmd->punch.active;
            cl->punch_active[CURR] = cmd->punch.active;
            cl->punch_active[PREV] = cmd->punch.active;
            break;

        case CMD_END_OF_UPDATE:
            cs.got_tilt_axes = 0;
            cs.next_update = 1;

            if (cs.first_update)
            {
                for (p = 0; p < MAX_PLAYERS; p++)
                {
                    if (gd[p].state)
                    {
                        game_lerp_copy(&gl[p]);
                        /* Hack to sync state before the next update. */
                        game_lerp_apply(&gl[p], &gd[p]);
                    }
                }
                cs.first_update = 0;
                break;
            }

            /* Compute gravity for particle effects. */
            /* Use P0 context for global particles */
            if (stats[0].status == GAME_GOAL)
                game_tilt_grav(v, GRAVITY_UP, &gl[0].tilt[CURR]);
            else
                game_tilt_grav(v, GRAVITY_DN, &gl[0].tilt[CURR]);

            /* Step particle, goal and jump effects. */

            if (cs.ups > 0)
            {
                dt = 1.0f / cs.ups;

                for (p = 0; p < MAX_PLAYERS; p++)
                {
                    if (gd[p].state)
                    {
                         struct game_draw *cg_p = &gd[p];
                         struct game_lerp *cl_p = &gl[p];

                         if (cg_p->goal_e && cl_p->goal_k[CURR] < 1.0f)
                            cl_p->goal_k[CURR] += dt;

                         if (cg_p->jump_b)
                         {
                            cl_p->jump_dt[CURR] += dt;

                            if (cl_p->jump_dt[PREV] >= 1.0f)
                                cg_p->jump_b = 0;
                         }
                    }
                }

                part_step(v, dt);
            }

            /* Client-side Particle Logic */
            for (p = 0; p < MAX_PLAYERS; p++)
            {
                if (gd[p].state)
                {
                    /* Spin Dash Charge Particles */
                    float charge = curr_dash_charge(p);
                    if (charge > 0.0f)
                    {
                        float pos[3];
                        curr_ball_pos(p, pos);
                        float color[3] = { 1.0f, 1.0f * (1.0f - charge), 0.0f }; /* Yellow to Red */

                        /* Emit small trail */
                        part_trail(pos, color);
                    }

                    /* Gyro Particles */
                    if (curr_gyro_timer(p) > 0.0f)
                    {
                        float pos[3];
                        curr_ball_pos(p, pos);
                        /* Offset above ball */
                        pos[1] += 1.5f;
                        float color[3] = { 0.5f, 1.0f, 1.0f }; /* Cyan */

                        if ((int)(curr_gyro_timer(p) * 10) % 2 == 0) /* Flicker */
                            part_trail(pos, color);
                    }
                }
            }

            break;

        case CMD_MAKE_BALL:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_MAKE_ITEM:
            /* Not supported anymore. */
            break;

        case CMD_PLACE_ITEM:
            {
                /* Dynamically add item to the vary struct */
                /* This is a hacky way to insert items at runtime since solid_vary usually expects them from .sol */
                struct s_vary *vary = &cg->vary;

                /* Reallocate if needed, or find empty slot */
                int found = -1;
                int i;
                for (i = 0; i < vary->hc; i++) {
                    if (vary->hv[i].t == ITEM_NONE) {
                        found = i;
                        break;
                    }
                }

                if (found == -1) {
                    /* Need to grow array */
                    int new_c = vary->hc + 1;
                    struct v_item *new_v = realloc(vary->hv, new_c * sizeof(struct v_item));
                    if (new_v) {
                        vary->hv = new_v;
                        found = vary->hc;
                        vary->hc = new_c;
                    }
                }

                if (found != -1) {
                    struct v_item *hp = &vary->hv[found];
                    memset(hp, 0, sizeof(struct v_item));
                    v_cpy(hp->p, cmd->placeitem.p);
                    hp->t = cmd->placeitem.t;
                    hp->n = cmd->placeitem.n;

                    /* Needs parent mapping, identity for now */
                    hp->mi = -1;
                    hp->mj = -1;
                }
            }
            break;

        case CMD_PICK_ITEM:
            /* Set up particle effects and discard the item. */

            if ((idx = cmd->pkitem.hi) >= 0 && idx < vary->hc)
            {
                float pos[3];

                hp = &vary->hv[idx];

                sol_entity_world(pos, vary, hp->mi, hp->mj, hp->p);

                item_color(hp, v);
                part_burst(pos, v);

                hp->t = ITEM_NONE;
            }
            break;

        case CMD_TILT_ANGLES:
            if (!cs.got_tilt_axes)
            {
                game_tilt_axes(tilt, view->e);
            }

            tilt->rx = cmd->tiltangles.x;
            tilt->rz = cmd->tiltangles.z;
            break;

        case CMD_SOUND:
            /* Play the sound. */

            if (cmd->sound.n)
                audio_play(cmd->sound.n, cmd->sound.a);

            break;

        case CMD_TIMER:
            cst->timer = cmd->timer.t;
            break;

        case CMD_STATUS:
            cst->status = cmd->status.t;
            break;

        case CMD_COINS:
            cst->coins = cmd->coins.n;
            break;

        case CMD_JUMP_READY:
            cst->jump_ready = cmd->jumpready.active;
            break;

        case CMD_DASH_CHARGE:
            cst->dash_charge = cmd->dashcharge.charge;
            break;

        case CMD_GYRO_STATE:
            cst->gyro_active = cmd->gyrostate.active;
            cst->gyro_timer = cmd->gyrostate.timer;
            break;

        case CMD_JUMP_ENTER:
            cg->jump_b  = 1;
            cg->jump_e  = 0;
            cl->jump_dt[PREV] = 0.0f;
            cl->jump_dt[CURR] = 0.0f;
            break;

        case CMD_JUMP_EXIT:
            cg->jump_e = 1;
            break;

        case CMD_MOVE_PATH:
        case CMD_MOVE_TIME:
        case CMD_BODY_PATH:
        case CMD_BODY_TIME:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_GOAL_OPEN:
            if (!cg->goal_e)
            {
                cg->goal_e = 1;
                cl->goal_k[CURR] = cs.first_update ? 1.0f : 0.0f;
            }
            break;

        case CMD_SWCH_ENTER:
            if ((idx = cmd->swchenter.xi) >= 0 && idx < vary->xc)
                vary->xv[idx].e = 1;
            break;

        case CMD_SWCH_TOGGLE:
            if ((idx = cmd->swchtoggle.xi) >= 0 && idx < vary->xc)
                vary->xv[idx].f = !vary->xv[idx].f;
            break;

        case CMD_SWCH_EXIT:
            if ((idx = cmd->swchexit.xi) >= 0 && idx < vary->xc)
                vary->xv[idx].e = 0;
            break;

        case CMD_UPDATES_PER_SECOND:
            cs.ups = cmd->ups.n;
            break;

        case CMD_BALL_RADIUS:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_CLEAR_ITEMS:
            /* Not supported anymore. */
            break;

        case CMD_CLEAR_BALLS:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_BALL_POSITION:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_BALL_BASIS:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_BALL_PEND_BASIS:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_VIEW_POSITION:
            v_cpy(view->p, cmd->viewpos.p);
            break;

        case CMD_VIEW_CENTER:
            v_cpy(view->c, cmd->viewcenter.c);
            break;

        case CMD_VIEW_BASIS:
            v_cpy(view->e[0], cmd->viewbasis.e[0]);
            v_cpy(view->e[1], cmd->viewbasis.e[1]);
            v_crs(view->e[2], view->e[0], view->e[1]);
            break;

        case CMD_CURRENT_BALL:
            if ((idx = cmd->currball.ui) >= 0 && idx < vary->uc)
                cs.curr_ball = idx;
            break;

        case CMD_PATH_FLAG:
            if ((idx = cmd->pathflag.pi) >= 0 && idx < vary->pc)
                vary->pv[idx].f = cmd->pathflag.f;
            break;

        case CMD_STEP_SIMULATION:
            sol_lerp_cmd(&cl->lerp, &cs, cmd);
            break;

        case CMD_MAP:
            game_compat_map = (version.x == cmd->map.version.x);
            break;

        case CMD_TILT_AXES:
            cs.got_tilt_axes = 1;
            v_cpy(tilt->x, cmd->tiltaxes.x);
            v_cpy(tilt->z, cmd->tiltaxes.z);
            break;

        case CMD_NONE:
        case CMD_MAX:
            break;
        }
    }
}

void game_client_sync(fs_file demo_fp)
{
    union cmd *cmdp;

    while ((cmdp = game_proxy_deq()))
    {
        if (demo_fp)
            cmd_put(demo_fp, cmdp);

        game_run_cmd(cmdp);

        cmd_free(cmdp);
    }
}

/*---------------------------------------------------------------------------*/

int  game_client_init(const char *file_name)
{
    char *back_name = "", *grad_name = "";
    int i, p;
    int player_count = config_get_d(CONFIG_MULTIBALL);
    if (player_count < 1) player_count = 1;
    if (player_count > MAX_PLAYERS) player_count = MAX_PLAYERS;

    game_client_free(file_name);

    /* Load SOL data. */

    if (!game_base_load(file_name))
        return (gd[0].state = 0); /* Signal failure via gd[0] */

    /* Initialize all players */
    for (p = 0; p < player_count; p++)
    {
        struct game_draw *cg = &gd[p];
        struct game_lerp *cl = &gl[p];
        struct client_stats *cst = &stats[p];

        cst->coins = 0;
        cst->status = GAME_NONE;
        cst->timer = 0.0f;
        cst->jump_ready = 0;
        cst->dash_charge = 0.0f;

        if (!sol_load_vary(&cg->vary, &game_base))
        {
            /* If fail, partial clean? We rely on game_client_free cleaning up. */
            /* But caller checks return. */
            /* Let's assume if p=0 fails we return 0. */
            if (p == 0)
            {
                game_base_free(NULL);
                return (cg->state = 0);
            }
        }

        if (!sol_load_draw(&cg->draw, &cg->vary, config_get_d(CONFIG_SHADOW)))
        {
            sol_free_vary(&cg->vary);
            if (p == 0)
            {
                game_base_free(NULL);
                return (cg->state = 0);
            }
        }

        cg->state = 1;

        /* Initialize game state. */

        game_tilt_init(&cg->tilt);
        game_view_init(&cg->view);

        cg->jump_e  = 1;
        cg->jump_b  = 0;
        cg->jump_dt = 0.0f;

        cg->goal_e = 0;
        cg->goal_k = 0.0f;

        /* Initialize interpolation. */

        game_lerp_init(cl, cg);

        /* Initialize fade. */

        cg->fade_k =  1.0f;
        cg->fade_d = -2.0f;
    }

    /* Load level info (from shared base) */

    version.x = 0;
    version.y = 0;

    for (i = 0; i < game_base.dc; i++)
    {
        char *k = game_base.av + game_base.dv[i].ai;
        char *v = game_base.av + game_base.dv[i].aj;

        if (strcmp(k, "back") == 0) back_name = v;
        if (strcmp(k, "grad") == 0) grad_name = v;

        if (strcmp(k, "version") == 0)
            sscanf(v, "%d.%d", &version.x, &version.y);
    }

    game_compat_map = version.x == 1;

    part_reset();

    cmd_state_init(&cs);

    back_init(grad_name);
    /* Back is shared visually? Or each player has own skybox instance? */
    /* game_draw takes gd->back. */
    /* We need to init back for each player or share? */
    /* sol_load_full takes path. It loads geometry. */
    /* We should probably load it for each player so they have independent state/draw struct */
    for (p = 0; p < player_count; p++)
    {
        sol_load_full(&gd[p].back, back_name, 0);
    }

    light_reset();

    ghost_active = 0;

    /* Compute map bounds */
    if (gd[0].state) {
        struct s_base *base = gd[0].vary.base;
        if (base->vc > 0) {
            v_cpy(map_min, base->vv[0].p);
            v_cpy(map_max, base->vv[0].p);
            for (i = 1; i < base->vc; i++) {
                if (base->vv[i].p[0] < map_min[0]) map_min[0] = base->vv[i].p[0];
                if (base->vv[i].p[0] > map_max[0]) map_max[0] = base->vv[i].p[0];
                if (base->vv[i].p[2] < map_min[2]) map_min[2] = base->vv[i].p[2];
                if (base->vv[i].p[2] > map_max[2]) map_max[2] = base->vv[i].p[2];
            }
        } else {
            v_zero(map_min);
            v_zero(map_max);
        }
    }

    return gd[0].state;
}

void game_client_free(const char *next)
{
    int p;
    /* Clean up all players */
    /* We don't track player_count precisely here, so loop MAX or check state */
    /* But checking state is safe */

    game_proxy_clr();

    for (p = 0; p < MAX_PLAYERS; p++)
    {
        if (gd[p].state)
        {
            game_lerp_free(&gl[p]);
            sol_free_draw(&gd[p].draw);
            sol_free_vary(&gd[p].vary);
            sol_free_full(&gd[p].back);
            gd[p].state = 0;
        }
    }

    /* Only free game_base if we actually loaded something (gd[0].state was 1) */
    /* But we just cleared state. */
    /* game_client_init checked gd[0].state. */
    /* We can just call game_base_free(next) unconditionally? */
    /* game_base_free checks if base is loaded? No, it frees. */
    /* If we call it multiple times, we need safety. */
    /* For now, assume single call flow. */

    game_base_free(next);
    back_free();
}

/*---------------------------------------------------------------------------*/

int enable_interpolation = 1;

void game_client_blend(float a)
{
    int p;
    for (p = 0; p < MAX_PLAYERS; p++)
    {
        if (enable_interpolation)
            gl[p].alpha = a;
        else
            gl[p].alpha = 1.0f;
    }
}

void game_client_draw(int pose, float t)
{
    int p;
    int count = config_get_d(CONFIG_MULTIBALL);
    if (count < 1) count = 1;
    if (count > MAX_PLAYERS) count = MAX_PLAYERS;

    int w = video.device_w;
    int h = video.device_h;

    for (p = 0; p < count; p++)
    {
        if (!gd[p].state) continue;

        int vp_x = 0, vp_y = 0, vp_w = w, vp_h = h;

        if (count == 2)
        {
            /* Horizontal split */
            vp_h = h / 2;
            vp_y = (p == 0) ? h / 2 : 0;
        }
        else if (count >= 3)
        {
            /* Quad split */
            vp_w = w / 2;
            vp_h = h / 2;
            if (p == 0) { vp_x = 0;    vp_y = h/2; }
            if (p == 1) { vp_x = w/2;  vp_y = h/2; }
            if (p == 2) { vp_x = 0;    vp_y = 0;   }
            if (p == 3) { vp_x = w/2;  vp_y = 0;   }
        }

        game_lerp_apply(&gl[p], &gd[p]);

        /* Pass viewport to game_draw */
        game_draw(gd, p, count, pose, t, vp_x, vp_y, vp_w, vp_h);
    }
}

/*
 * Ghost Playback Interface
 */

void game_client_ghost_init(void)
{
    if (!gd[0].state) return;

    /* Clone P0 state for ghost */
    if (sol_load_vary(&ghost_gd.vary, &game_base))
    {
        if (sol_load_draw(&ghost_gd.draw, &ghost_gd.vary, config_get_d(CONFIG_SHADOW)))
        {
            ghost_gd.state = 1;
            game_tilt_init(&ghost_gd.tilt);
            game_view_init(&ghost_gd.view);
            ghost_gd.jump_e = 1;
            ghost_gd.jump_b = 0;
            ghost_gd.jump_dt = 0.0f;
            ghost_gd.goal_e = 0;
            ghost_gd.goal_k = 0.0f;
            ghost_gd.fade_k = 1.0f;
            ghost_gd.fade_d = -2.0f;

            /* Share P0 background */
            /* Actually sol_load_full does a deep copy/load. We need to manually set it or load again. */
            /* Let's just point to gd[0].back for drawing if needed, but game_draw_balls doesn't need back. */

            game_lerp_init(&ghost_gl, &ghost_gd);
            ghost_active = 1;
        }
        else
        {
            sol_free_vary(&ghost_gd.vary);
        }
    }
}

void game_client_ghost_sync(fs_file fp)
{
    if (!ghost_active || !fp) return;

    union cmd cmd;
    static struct cmd_state ghost_cs;
    static int init = 0;

    if (!init) {
        cmd_state_init(&ghost_cs);
        init = 1;
    }

    while (cmd_get(fp, &cmd))
    {
        /* Minimal parser for ghost state */
        switch (cmd.type)
        {
        case CMD_END_OF_UPDATE:
            game_lerp_copy(&ghost_gl);
            game_lerp_apply(&ghost_gl, &ghost_gd);
            return; /* Done for this frame */

        case CMD_BALL_POSITION:
            if (ghost_gd.vary.uc > 0)
                v_cpy(ghost_gd.vary.uv[0].p, cmd.ballpos.p);
            break;

        case CMD_BALL_BASIS:
            if (ghost_gd.vary.uc > 0) {
                v_cpy(ghost_gd.vary.uv[0].e[0], cmd.ballbasis.e[0]);
                v_cpy(ghost_gd.vary.uv[0].e[1], cmd.ballbasis.e[1]);
                v_crs(ghost_gd.vary.uv[0].e[2], cmd.ballbasis.e[0], cmd.ballbasis.e[1]);
            }
            break;

        default:
            /* Ignore other commands for ghost */
            break;
        }
    }
}

/* Special draw for ghost ball overlaid on current view */
void game_client_draw_ghost(int p, float t)
{
    if (!ghost_active || !ghost_gd.state) return;

    game_lerp_apply(&ghost_gl, &ghost_gd);

    /* Draw Ghost Ball */
    struct s_vary *vary = &ghost_gd.vary;
    if (vary->uc > 0)
    {
        float ball_M[16], pend_M[16], bill_M[16];
        m_basis(ball_M, vary->uv[0].e[0], vary->uv[0].e[1], vary->uv[0].e[2]);
        m_basis(pend_M, vary->uv[0].E[0], vary->uv[0].E[1], vary->uv[0].E[2]);
        m_ident(bill_M); /* Billboard not strictly needed for ball */

        glPushMatrix();
        glTranslatef(vary->uv[0].p[0],
                     vary->uv[0].p[1] + BALL_FUDGE,
                     vary->uv[0].p[2]);

        /* Ghost Color: Translucent White/Blue */
        glColor4f(0.5f, 0.5f, 1.0f, 0.5f);

        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);

        /* Instead of trying to use solid_draw's ball_draw which needs private s_rend,
           we just draw the shared ball object from ball.c directly. */

        /* Since ball_draw_geom doesn't exist, we fallback to drawing a point,
           but we'll just try to use the solid.draw directly if possible, but that's internal.
           So instead, we just draw a basic sphere using points/lines for now to avoid the link error. */

        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glPointSize(10.0f);
        glBegin(GL_POINTS);
        glVertex3f(0,0,0);
        glEnd();
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);

        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);

        glPopMatrix();
    }
}

void game_client_draw_debug(int p)
{
    /* Only debug draw for P0 for now */
    if (p != 0) return;

    /* Check Mode */
    /* Accessing mode via curr_mode() might be circular if not careful,
       but game_common uses progress.h. */
    /* Better: check internal state if possible or rely on caller. */
    /* game_server is authoritative. */
    /* Let's just draw generic bounds if enabled. */

    /* Draw Court Lines based on mode */
    /* Accessing 'game_mode' from game_server.c is via 'curr_mode()' */
    int mode = curr_mode();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);

    if (mode == MODE_TENNIS) {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        /* Baseline */
        glVertex3f(-10, 0.1f, -15); glVertex3f(10, 0.1f, -15);
        glVertex3f(-10, 0.1f,  15); glVertex3f(10, 0.1f,  15);
        /* Sidelines */
        glVertex3f(-10, 0.1f, -15); glVertex3f(-10, 0.1f, 15);
        glVertex3f( 10, 0.1f, -15); glVertex3f( 10, 0.1f, 15);
        /* Net */
        glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
        glVertex3f(-12, 1.0f, 0); glVertex3f(12, 1.0f, 0);
        glEnd();
    }
    else if (mode == MODE_SOCCER) {
        /* Field */
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(-20, 0.1f, -40); glVertex3f(20, 0.1f, -40);
        glVertex3f(-20, 0.1f,  40); glVertex3f(20, 0.1f,  40);
        glVertex3f(-20, 0.1f, -40); glVertex3f(-20, 0.1f, 40);
        glVertex3f( 20, 0.1f, -40); glVertex3f( 20, 0.1f, 40);

        /* Goals */
        /* Red Goal Z- */
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
        glVertex3f(-5, 0, -40); glVertex3f(-5, 3, -40);
        glVertex3f( 5, 0, -40); glVertex3f( 5, 3, -40);
        glVertex3f(-5, 3, -40); glVertex3f( 5, 3, -40);

        /* Blue Goal Z+ */
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        glVertex3f(-5, 0, 40); glVertex3f(-5, 3, 40);
        glVertex3f( 5, 0, 40); glVertex3f( 5, 3, 40);
        glVertex3f(-5, 3, 40); glVertex3f( 5, 3, 40);
        glEnd();
    }
    else if (mode == MODE_BASEBALL) {
        /* Diamond */
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex3f(0, 0.1f, 15);  /* Home */
        glVertex3f(15, 0.1f, 0);  /* 1st */
        glVertex3f(0, 0.1f, -15); /* 2nd */
        glVertex3f(-15, 0.1f, 0); /* 3rd */
        glEnd();
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

/*---------------------------------------------------------------------------*/

int curr_clock(int p)
{
    return (int) (stats[p].timer * 100.f);
}

int curr_coins(int p)
{
    return stats[p].coins;
}

int curr_status(int p)
{
    return stats[p].status;
}

int curr_jump_ready(int p)
{
    return stats[p].jump_ready;
}

float curr_dash_charge(int p)
{
    return stats[p].dash_charge;
}

float curr_gyro_timer(int p)
{
    return stats[p].gyro_active ? stats[p].gyro_timer : 0.0f;
}

void curr_map_bounds(float *min_v, float *max_v)
{
    v_cpy(min_v, map_min);
    v_cpy(max_v, map_max);
}

void curr_ball_pos(int p, float *pos)
{
    if (gd[p].state && gd[p].vary.uc > 0)
        v_cpy(pos, gd[p].vary.uv[0].p);
    else
        v_zero(pos);
}

int curr_goal_count(void)
{
    if (gd[0].state)
        return gd[0].vary.base->zc;
    return 0;
}

void curr_goal_pos(int i, float *pos)
{
    if (gd[0].state && i >= 0 && i < gd[0].vary.base->zc)
        v_cpy(pos, gd[0].vary.base->zv[i].p);
    else
        v_zero(pos);
}

/*---------------------------------------------------------------------------*/

void game_look(float phi, float theta)
{
    /* Apply look to all players? Or default P0? */
    /* This function is used by st_look (mouse look around). */
    /* Usually purely visual/client side. */
    /* Default to P0 for now. */
    int p = 0;
    struct game_view *view = &gl[p].view[CURR];

    view->c[0] = view->p[0] + fsinf(V_RAD(theta)) * fcosf(V_RAD(phi));
    view->c[1] = view->p[1] +                       fsinf(V_RAD(phi));
    view->c[2] = view->p[2] - fcosf(V_RAD(theta)) * fcosf(V_RAD(phi));

    gl[p].view[PREV] = gl[p].view[CURR];
}

/*---------------------------------------------------------------------------*/

void game_kill_fade(void)
{
    int p;
    for (p = 0; p < MAX_PLAYERS; p++)
    {
        gd[p].fade_k = 0.0f;
        gd[p].fade_d = 0.0f;
    }
}

void game_step_fade(float dt)
{
    int p;
    for (p = 0; p < MAX_PLAYERS; p++)
    {
        struct game_draw *cg = &gd[p];
        if ((cg->fade_k < 1.0f && cg->fade_d > 0.0f) ||
            (cg->fade_k > 0.0f && cg->fade_d < 0.0f))
            cg->fade_k += cg->fade_d * dt;

        if (cg->fade_k < 0.0f)
        {
            cg->fade_k = 0.0f;
            cg->fade_d = 0.0f;
        }
        if (cg->fade_k > 1.0f)
        {
            cg->fade_k = 1.0f;
            cg->fade_d = 0.0f;
        }
    }
}

void game_fade(float d)
{
    int p;
    for (p = 0; p < MAX_PLAYERS; p++)
        gd[p].fade_d = d;
}

/*---------------------------------------------------------------------------*/

void game_client_fly(float k)
{
    int p;
    for (p = 0; p < MAX_PLAYERS; p++)
    {
        game_view_fly(&gl[p].view[CURR], &gd[p].vary, k);
        gl[p].view[PREV] = gl[p].view[CURR];
    }
}

/*---------------------------------------------------------------------------*/
