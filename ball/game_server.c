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

#include "vec3.h"
#include "geom.h"
#include "config.h"
#include "binary.h"
#include "common.h"
#include "ease.h"

#include "solid_sim.h"
#include "solid_all.h"

#include "game_common.h"
#include "game_server.h"
#include "game_proxy.h"

#include "cmd.h"
#include "progress.h"
#include "stats.h"
#include "profile.h"
#include "hud.h"

/*---------------------------------------------------------------------------*/

static int server_state = 0;
static int game_mode = MODE_NORMAL;

struct server_player
{
    struct s_vary vary;
    struct s_vary *sim_state;
    int ball_index;
    int sim_owner;

    struct game_tilt tilt;
    struct game_view view;

    float view_k;
    float view_time;
    float view_fade;

    float view_zoom_curr;
    float view_zoom_start;
    float view_zoom_end;
    float view_zoom_time;

    float time_limit;
    float time_elapsed;
    float timer;
    int   status;
    int   coins;

    int   goal_e;
    int   jump_e;
    int   jump_b;
    float jump_dt;
    float jump_p[3];
    float start_p[3];

    /* Flight Physics State */
    int   fly_active;
    int   fly_done;
    float fly_pitch;

    /* Jump Physics State */
    int   can_jump;
    float jump_timer;

    /* Spin Dash Physics State */
    float spin_charge;

    /* Fight Physics State */
    int   punch_state; /* 0=None, 1=Extending, 2=Retracting */
    float punch_timer;

    /* Gyrocopter State */
    int   gyro_active;
    float gyro_timer;

    /* Shock Absorber State */
    int   shock_active;
    float shock_timer;

    /* Super Speed State */
    int   speed_active;
    float speed_timer;

    /* Billiards/Bowling/Golf State */
    float shot_power;
    int   bowling_frame;
    int   bowling_throw;
    int   golf_strokes;
    int   golf_par;
    int   bowling_pins[10];
    float bowling_pin_start[10][3];
    int   shot_state; /* 0=Aim, 1=Power, 2=Rolling */

    /* Boat State */
    float paddle_left_timer;
    float paddle_right_timer;
    float boat_speed;

    /* Shot State */
    float cursor_x;
    float cursor_y;
    int   ammo;
    float reload_timer;

    /* Soccer State */
    int team; /* 0=Red, 1=Blue */
    int score;

    /* Tennis State */
    /* score is reused */
    int serve_state; /* 0=Serve, 1=Play */

    /* Baseball State */
    int baseball_role; /* 0=Pitcher, 1=Batter */
    float baseball_charge;

    /* Race Items */
    int   held_item; /* 0=None, ITEM_MISSILE, ITEM_BANANA */
    float stun_timer;

    /* Hub State */
    /* Hub State */
    int   warp_id;

    /* Mole State */
    int   mole_grid[16]; /* 4x4 */
    float mole_timers[16];

    /* Hammer Throw State */
    float hammer_angle;
    float hammer_spin_speed;
    int   hammer_thrown;

    int   action_prev;

    float gravity_dir[3];
    float dist_accumulator;

    int   is_cpu;
    float ai_timer;
    int   ai_state;
};

static struct server_player players[MAX_PLAYERS];
static int player_count = 1;
static int cpu_count = 0;

void game_ai_step(int p, float dt);

void game_set_cpu_count(int n) {
    cpu_count = n;
}

#define VIEW_FADE_MIN 0.2f
#define VIEW_FADE_MAX 1.0f

#define ZOOM_DELAY (GROW_TIME * 0.5f)
#define ZOOM_TIME (ZOOM_DELAY + GROW_TIME)
#define ZOOM_MIN 0.75f
#define ZOOM_MAX 1.25f

/*---------------------------------------------------------------------------*/

struct input
{
    float s;
    float x;
    float z;
    float r;
    int   c;
    int   action;
    int   dash;
};

static struct input input_players[MAX_PLAYERS];
static struct character_stats player_stats[MAX_PLAYERS];

static float get_angle_bound(int p)
{
    float base = config_get_d(CONFIG_PHYSICS) ? 60.0f : ANGLE_BOUND;
    /* Increase tilt angle if stats.control is high */
    /* This allows turning faster */
    if (p >= 0 && p < MAX_PLAYERS)
        return base * player_stats[p].control;
    return base;
}

static void input_init(void)
{
    int i;
    for (i = 0; i < MAX_PLAYERS; i++)
    {
        input_players[i].s = RESPONSE;
        input_players[i].x = 0;
        input_players[i].z = 0;
        input_players[i].r = 0;
        input_players[i].c = 0;
        input_players[i].action = 0;
        input_players[i].dash = 0;
    }
}

static void input_set_s(int p, float s)
{
    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].s = s;
}

static void input_set_x(int p, float x)
{
    float bound = get_angle_bound(p);
    if (x < -bound) x = -bound;
    if (x >  bound) x =  bound;

    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].x = x;
}

static void input_set_z(int p, float z)
{
    float bound = get_angle_bound(p);
    if (z < -bound) z = -bound;
    if (z >  bound) z =  bound;

    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].z = z;
}

static void input_set_r(int p, float r)
{
    if (r < -VIEWR_BOUND) r = -VIEWR_BOUND;
    if (r >  VIEWR_BOUND) r =  VIEWR_BOUND;

    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].r = r;
}

static void input_set_c(int p, int c)
{
    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].c = c;
}

static void input_set_action(int p, int a)
{
    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].action = a;
}

static void input_set_dash(int p, int d)
{
    if (p >= 0 && p < MAX_PLAYERS)
        input_players[p].dash = d;
}

static float input_get_s(int p)
{
    float s = (p >= 0 && p < MAX_PLAYERS) ? input_players[p].s : RESPONSE;
    if (config_get_d(CONFIG_PHYSICS)) return s * 0.6f; /* Much faster, snappier acceleration for Arcade */
    return s;
}

static float input_get_x(int p)
{
    return (p >= 0 && p < MAX_PLAYERS) ? input_players[p].x : 0.0f;
}

static float input_get_z(int p)
{
    return (p >= 0 && p < MAX_PLAYERS) ? input_players[p].z : 0.0f;
}

static float input_get_r(int p)
{
    return (p >= 0 && p < MAX_PLAYERS) ? input_players[p].r : 0.0f;
}

static int input_get_c(int p)
{
    return (p >= 0 && p < MAX_PLAYERS) ? input_players[p].c : 0;
}

static int input_get_action(int p)
{
    return (p >= 0 && p < MAX_PLAYERS) ? input_players[p].action : 0;
}

static int input_get_dash(int p)
{
    return (p >= 0 && p < MAX_PLAYERS) ? input_players[p].dash : 0;
}

/*---------------------------------------------------------------------------*/

/* Target Zones Configuration */
static const struct target_zone zones[] = {
    {  2.0f, 500, { 1.0f, 0.0f, 0.0f, 0.5f } }, /* Red Bullseye */
    {  5.0f, 300, { 1.0f, 1.0f, 0.0f, 0.5f } }, /* Yellow Inner */
    { 10.0f, 100, { 0.0f, 0.0f, 1.0f, 0.5f } }, /* Blue Outer */
    { 15.0f,  50, { 1.0f, 1.0f, 1.0f, 0.5f } }  /* White Edge */
};

int game_get_zone_count(void)
{
    return sizeof(zones) / sizeof(zones[0]);
}

const struct target_zone *game_get_zones(void)
{
    return zones;
}

/*---------------------------------------------------------------------------*/

static union cmd cmd;

static void game_cmd_map(const char *name, int ver_x, int ver_y)
{
    cmd.type          = CMD_MAP;
    cmd.map.name      = strdup(name);
    cmd.map.version.x = ver_x;
    cmd.map.version.y = ver_y;
    game_proxy_enq(&cmd);
}

static void game_cmd_eou(void)
{
    cmd.type = CMD_END_OF_UPDATE;
    game_proxy_enq(&cmd);
}

static void game_cmd_ups(void)
{
    cmd.type  = CMD_UPDATES_PER_SECOND;
    cmd.ups.n = UPS;
    game_proxy_enq(&cmd);
}

static void game_cmd_sound(const char *filename, float a)
{
    cmd.type = CMD_SOUND;

    cmd.sound.n = strdup(filename);
    cmd.sound.a = a;

    game_proxy_enq(&cmd);
}

#define audio_play(s, f) game_cmd_sound((s), (f))

static void game_cmd_set_player(int p)
{
    cmd.type = CMD_SET_PLAYER;
    cmd.setplayer.player_index = p;
    game_proxy_enq(&cmd);
}

static void game_cmd_goalopen(int p)
{
    game_cmd_set_player(p);
    cmd.type = CMD_GOAL_OPEN;
    game_proxy_enq(&cmd);
}

static void game_cmd_updball(int p)
{
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    game_cmd_set_player(p);

    cmd.type = CMD_CURRENT_BALL;
    cmd.currball.ui = pl->ball_index;
    game_proxy_enq(&cmd);

    cmd.type = CMD_BALL_POSITION;
    v_cpy(cmd.ballpos.p, b->p);
    game_proxy_enq(&cmd);

    cmd.type = CMD_BALL_BASIS;
    v_cpy(cmd.ballbasis.e[0], b->e[0]);
    v_cpy(cmd.ballbasis.e[1], b->e[1]);
    game_proxy_enq(&cmd);

    cmd.type = CMD_BALL_PEND_BASIS;
    v_cpy(cmd.ballpendbasis.E[0], b->E[0]);
    v_cpy(cmd.ballpendbasis.E[1], b->E[1]);
    game_proxy_enq(&cmd);
}

/* Update ALL balls for the given player's view */
static void game_cmd_upd_all_balls(int p)
{
    struct server_player *pl = &players[p];
    int i;

    for (i = 0; i < pl->sim_state->uc; i++)
    {
        struct v_ball *b = &pl->sim_state->uv[i];

        game_cmd_set_player(p);

        cmd.type = CMD_CURRENT_BALL;
        cmd.currball.ui = i;
        game_proxy_enq(&cmd);

        cmd.type = CMD_BALL_POSITION;
        v_cpy(cmd.ballpos.p, b->p);
        game_proxy_enq(&cmd);

        cmd.type = CMD_BALL_BASIS;
        v_cpy(cmd.ballbasis.e[0], b->e[0]);
        v_cpy(cmd.ballbasis.e[1], b->e[1]);
        game_proxy_enq(&cmd);

        cmd.type = CMD_BALL_PEND_BASIS;
        v_cpy(cmd.ballpendbasis.E[0], b->E[0]);
        v_cpy(cmd.ballpendbasis.E[1], b->E[1]);
        game_proxy_enq(&cmd);
    }
}

static void game_cmd_updview(int p)
{
    game_cmd_set_player(p);

    cmd.type = CMD_VIEW_POSITION;
    v_cpy(cmd.viewpos.p, players[p].view.p);
    game_proxy_enq(&cmd);

    cmd.type = CMD_VIEW_CENTER;
    v_cpy(cmd.viewcenter.c, players[p].view.c);
    game_proxy_enq(&cmd);

    cmd.type = CMD_VIEW_BASIS;
    v_cpy(cmd.viewbasis.e[0], players[p].view.e[0]);
    v_cpy(cmd.viewbasis.e[1], players[p].view.e[1]);
    game_proxy_enq(&cmd);
}

static void game_cmd_ballradius(int p)
{
    struct server_player *pl = &players[p];

    /* Sync ALL ball radii */
    int i;
    for (i = 0; i < pl->sim_state->uc; i++) {
        game_cmd_set_player(p);
        cmd.type = CMD_CURRENT_BALL;
        cmd.currball.ui = i;
        game_proxy_enq(&cmd);

        cmd.type = CMD_BALL_RADIUS;
        cmd.ballradius.r = pl->sim_state->uv[i].r;
        game_proxy_enq(&cmd);
    }
}

static void game_cmd_init_balls(int p, int count)
{
    int i;
    game_cmd_set_player(p);
    cmd.type = CMD_CLEAR_BALLS;
    game_proxy_enq(&cmd);

    for (i = 0; i < count; i++) {
        cmd.type = CMD_MAKE_BALL;
        game_proxy_enq(&cmd);
    }

    game_cmd_upd_all_balls(p);
    game_cmd_ballradius(p);
}

static void game_cmd_pkitem(int p, int hi)
{
    game_cmd_set_player(p);
    cmd.type      = CMD_PICK_ITEM;
    cmd.pkitem.hi = hi;
    game_proxy_enq(&cmd);
}

static void game_cmd_jump(int p, int e)
{
    game_cmd_set_player(p);
    cmd.type = e ? CMD_JUMP_ENTER : CMD_JUMP_EXIT;
    game_proxy_enq(&cmd);
}

static void game_cmd_punch(int p, int e)
{
    game_cmd_set_player(p);
    cmd.type = CMD_PUNCH;
    cmd.punch.active = e;
    game_proxy_enq(&cmd);
}

static void game_cmd_jump_ready(int p, int e)
{
    game_cmd_set_player(p);
    cmd.type = CMD_JUMP_READY;
    cmd.jumpready.active = e;
    game_proxy_enq(&cmd);
}

static void game_cmd_dash_charge(int p, float charge)
{
    game_cmd_set_player(p);
    cmd.type = CMD_DASH_CHARGE;
    cmd.dashcharge.charge = charge;
    game_proxy_enq(&cmd);
}

static void game_cmd_gyro_state(int p, int active, float timer)
{
    game_cmd_set_player(p);
    cmd.type = CMD_GYRO_STATE;
    cmd.gyrostate.active = active;
    cmd.gyrostate.timer = timer;
    game_proxy_enq(&cmd);
}

static void game_cmd_tiltangles(int p)
{
    game_cmd_set_player(p);
    cmd.type = CMD_TILT_ANGLES;

    cmd.tiltangles.x = players[p].tilt.rx;
    cmd.tiltangles.z = players[p].tilt.rz;

    game_proxy_enq(&cmd);
}

static void game_cmd_tiltaxes(int p)
{
    game_cmd_set_player(p);
    cmd.type = CMD_TILT_AXES;

    v_cpy(cmd.tiltaxes.x, players[p].tilt.x);
    v_cpy(cmd.tiltaxes.z, players[p].tilt.z);

    game_proxy_enq(&cmd);
}

static void game_cmd_timer(int p)
{
    game_cmd_set_player(p);
    cmd.type    = CMD_TIMER;
    cmd.timer.t = players[p].timer;
    game_proxy_enq(&cmd);
}

static void game_cmd_coins(int p)
{
    game_cmd_set_player(p);
    cmd.type    = CMD_COINS;
    cmd.coins.n = players[p].coins;
    game_proxy_enq(&cmd);
}

static void game_cmd_status(int p)
{
    game_cmd_set_player(p);
    cmd.type     = CMD_STATUS;
    cmd.status.t = players[p].status;
    game_proxy_enq(&cmd);
}

/*---------------------------------------------------------------------------*/

static int grow_init(int p, int type)
{
    struct server_player *pl = &players[p];
    struct v_ball *up = &pl->sim_state->uv[pl->ball_index];

    int size = up->size;

    if (type == ITEM_SHRINK)
        size--;
    else if (type == ITEM_GROW)
        size++;

    size = CLAMP(0, size, 2);

    if (size != up->size)
    {
        const int old_size = up->size;

        up->r_vel = (up->sizes[size] - up->r) / GROW_TIME;
        up->size = size;

        if (size < old_size)
            return -1;

        if (size > old_size)
            return +1;
    }

    return 0;
}

static void grow_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct v_ball *up = &pl->sim_state->uv[pl->ball_index];

    if (up->r_vel != 0.0f)
    {
        float r, dr;

        r = up->r + up->r_vel * dt;

        if ((up->r < up->sizes[up->size] && r >= up->sizes[up->size]) ||
            (up->r > up->sizes[up->size] && r <= up->sizes[up->size]))
        {
            r = up->sizes[up->size];
            up->r_vel = 0.0f;
        }

        dr = r - up->r;

        up->p[1] += dr;
        up->r     = r;

        game_cmd_ballradius(p);
    }
}

/*---------------------------------------------------------------------------*/

static struct lockstep server_step;

/* Helper to resize ball array for party modes */
static void game_init_party_mode_physics(struct server_player *pl, int count)
{
    if (pl->sim_owner && count > 1)
    {
        struct v_ball *new_uv = realloc(pl->vary.uv, sizeof(struct v_ball) * count);
        if (new_uv)
        {
            pl->vary.uv = new_uv;
            /* Initialize new balls as copies of the first (default) ball */
            int i;
            for (i = 1; i < count; i++) {
                pl->vary.uv[i] = pl->vary.uv[0];
            }
            pl->vary.uc = count;
        }
    }
}

static void game_player_init(int p, int t, int e, int mode)
{
    struct server_player *pl = &players[p];
    int i;
    int ball_count = 1;

    /* Clean up existing physics state if re-initializing */
    if (pl->sim_owner) {
        sol_free_vary(&pl->vary);
    }
    memset(&pl->vary, 0, sizeof(struct s_vary));

    pl->time_limit = (float) t / 100.0f;
    pl->time_elapsed = 0.0f;
    pl->timer = 0.0f;
    pl->status = GAME_NONE;
    pl->coins = 0;

    pl->fly_active = 0;
    pl->fly_done = 0;
    pl->fly_pitch = 0.0f;

    pl->punch_state = 0;
    pl->punch_timer = 0.0f;

    pl->gyro_active = 0;
    pl->gyro_timer = 0.0f;

    pl->shock_active = 0;
    pl->shock_timer = 0.0f;

    pl->speed_active = 0;
    pl->speed_timer = 0.0f;

    pl->shot_power = 0.0f;

    pl->action_prev = 0;

    pl->bowling_frame = 1;
    pl->bowling_throw = 1;
    for(i=0; i<10; i++) pl->bowling_pins[i] = 0;
    pl->golf_strokes = 0;
    pl->shot_state = 0;

    pl->paddle_left_timer = 0.0f;
    pl->paddle_right_timer = 0.0f;
    pl->boat_speed = 0.0f;

    pl->cursor_x = 0.0f;
    pl->cursor_y = 0.0f;
    pl->ammo = 6;
    pl->reload_timer = 0.0f;

    pl->team = (p % 2); /* 0, 1, 0, 1 */
    pl->score = 0;
    pl->serve_state = 0;

    pl->baseball_role = 0;
    pl->baseball_charge = 0.0f;

    pl->held_item = 0;
    pl->stun_timer = 0.0f;

    pl->warp_id = -1;
    pl->dist_accumulator = 0.0f;

    /* CPU flag based on player count and config */
    /* If total players = P, and cpu_count = C, then players [P-C .. P-1] are CPU */
    if (p >= (player_count - cpu_count)) {
        pl->is_cpu = 1;
    } else {
        pl->is_cpu = 0;
    }
    pl->ai_timer = 0.0f;
    pl->ai_state = 0;

    /* Load stats for the current ball */
    stats_load(&player_stats[p], config_get_s(CONFIG_BALL_FILE));

    /* Adjust jump force based on stats (default is implicitly handled by physics engine but we can tweak it?) */
    /* The jump logic uses sol_jump_test which just checks for jump pads. */
    /* But we can affect acceleration. */

    if (mode == MODE_BATTLE || mode == MODE_TARGET || mode == MODE_FIGHT)
    {
        ball_count = player_count;
        if (p == 0)
        {
            /* Master simulation */
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            for (i = 1; i < ball_count; i++)
            {
                pl->vary.uv[i].p[0] += (float)i * 1.5f;
                pl->vary.uv[i].p[2] += (float)i * 1.5f;
            }
        }
        else
        {
            /* Slave */
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = p;
    }
    else if (mode == MODE_BILLIARDS)
    {
        ball_count = 16;
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            /* Init Billiard Positions (Triangle) */
            float x = pl->vary.uv[0].p[0];
            float z = pl->vary.uv[0].p[2] + 5.0f; /* Start further down */
            float r = pl->vary.uv[0].r;
            float d = r * 2.05f; /* Diameter + padding */

            int b = 1;
            int row, col;
            for (row = 0; row < 5; row++) {
                float z_row = z + row * d * 0.866f; /* sin(60) */
                float x_start = x - (row * d) * 0.5f;
                for (col = 0; col <= row; col++) {
                    if (b < ball_count) {
                        pl->vary.uv[b] = pl->vary.uv[0];
                        pl->vary.uv[b].p[0] = x_start + col * d;
                        pl->vary.uv[b].p[2] = z_row;
                        b++;
                    }
                }
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = 0;
    }
    else if (mode == MODE_BOWLING)
    {
        ball_count = 11; /* 1 Player + 10 Pins */
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            /* Setup Pins (Triangle) at end of lane */
            /* Assuming standard lane length, e.g. 20m? */
            /* Base position for pins */
            float x = pl->vary.uv[0].p[0];
            float z = pl->vary.uv[0].p[2] + 20.0f;
            float r = pl->vary.uv[0].r;
            float d = r * 2.5f; /* Pins spaced out a bit */

            int b = 1;
            int row, col;
            for (row = 0; row < 4; row++) { /* 4 rows for 10 pins */
                float z_row = z + row * d * 0.866f;
                float x_start = x - (row * d) * 0.5f;
                for (col = 0; col <= row; col++) {
                     if (b < ball_count) {
                        pl->vary.uv[b] = pl->vary.uv[0];
                        pl->vary.uv[b].p[0] = x_start + col * d;
                        pl->vary.uv[b].p[2] = z_row;

                        /* Store start pos */
                        v_cpy(pl->bowling_pin_start[b-1], pl->vary.uv[b].p);
                        b++;
                     }
                }
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = 0;
    }
    else if (mode == MODE_GOLF)
    {
        ball_count = 1;
        sol_load_vary(&pl->vary, &game_base);
        pl->sim_state = &pl->vary;
        pl->sim_owner = 1;
        pl->ball_index = 0;
        pl->golf_strokes = 0;

        if (p > 0)
        {
            /* Multiplayer Golf: Alternating or Concurrent? */
            /* For now, concurrent independent worlds like Race */
            pl->vary.uv[0].p[0] += (float)p * 1.5f;
            pl->vary.uv[0].p[2] += (float)p * 1.5f;
        }
    }
    else if (mode == MODE_SOCCER)
    {
        ball_count = player_count + 1;

        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            /* Init Soccer Ball */
            pl->vary.uv[0].r = 2.0f; /* Big ball */

            /* Init Player Balls */
            for(i=1; i < ball_count; i++) {
                pl->vary.uv[i] = pl->vary.uv[0]; /* Copy defaults */
                pl->vary.uv[i].r = 0.5f; /* Normal size */

                /* Team Positions */
                int team = ((i-1) % 2);
                float z_offset = (team == 0) ? -10.0f : 10.0f;
                float x_offset = (((i-1) / 2) * 2.0f) - 1.0f;

                pl->vary.uv[i].p[0] = x_offset;
                pl->vary.uv[i].p[1] += 5.0f;
                pl->vary.uv[i].p[2] = z_offset;
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }

        pl->ball_index = p + 1;
    }
    else if (mode == MODE_TENNIS)
    {
        ball_count = player_count + 1;
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            /* Tennis Ball (Index 0) */
            pl->vary.uv[0].r = 0.3f;
            pl->vary.uv[0].p[1] = 2.0f;

            /* Player Balls */
            for(i=1; i<ball_count; i++) {
                pl->vary.uv[i] = pl->vary.uv[0];
                pl->vary.uv[i].r = 0.5f;

                /* Court Positioning */
                int team = ((i-1) % 2);
                float z_offset = (team == 0) ? -10.0f : 10.0f;
                float x_offset = (((i-1) / 2) * 3.0f) - 1.5f;

                pl->vary.uv[i].p[0] = x_offset;
                pl->vary.uv[i].p[1] = 0.5f;
                pl->vary.uv[i].p[2] = z_offset;
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = p + 1;
    }
    else if (mode == MODE_BASEBALL)
    {
        ball_count = player_count + 1;
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            /* Baseball (Index 0) */
            pl->vary.uv[0].r = 0.2f;
            v_zero(pl->vary.uv[0].p);

            /* Player 1 (Pitcher - Team 0) */
            pl->vary.uv[1] = pl->vary.uv[0];
            pl->vary.uv[1].r = 0.5f;
            pl->vary.uv[1].p[2] = -15.0f; /* Mound */

            /* Player 2 (Batter - Team 1) */
            if (ball_count > 2) {
                pl->vary.uv[2] = pl->vary.uv[0];
                pl->vary.uv[2].r = 0.5f;
                pl->vary.uv[2].p[2] = 15.0f; /* Home Plate */
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }

        pl->ball_index = p + 1;
        pl->baseball_role = (p == 0) ? 0 : 1; /* P1 Pitch, P2 Bat */
    }
    else if (mode == MODE_MOLE)
    {
        ball_count = player_count + 16; /* 16 Moles */
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;

            game_init_party_mode_physics(pl, ball_count);

            /* Init Moles (Indices 1 to 16) */
            for(i=0; i<16; i++) {
                pl->vary.uv[i+player_count] = pl->vary.uv[0];
                pl->vary.uv[i+player_count].r = 0.5f;
                /* 4x4 Grid */
                int row = i / 4;
                int col = i % 4;
                pl->vary.uv[i+player_count].p[0] = -4.5f + (col * 3.0f);
                pl->vary.uv[i+player_count].p[1] = -2.0f; /* Hidden underground */
                pl->vary.uv[i+player_count].p[2] = -4.5f + (row * 3.0f);
                pl->mole_grid[i] = 0;
                pl->mole_timers[i] = 0.0f;
            }

            /* Player starting pos (Index 0 to player_count-1) */
            for(i=0; i<player_count; i++) {
                pl->vary.uv[i] = pl->vary.uv[0];
                pl->vary.uv[i].p[0] = 0.0f;
                pl->vary.uv[i].p[1] = 5.0f;
                pl->vary.uv[i].p[2] = 10.0f + (i * 2.0f);
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = p;
    }
    else if (mode == MODE_HAMMER)
    {
        ball_count = player_count;
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;
            game_init_party_mode_physics(pl, ball_count);
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = p;
        pl->hammer_angle = 0.0f;
        pl->hammer_spin_speed = 0.0f;
        pl->hammer_thrown = 0;
    }
    else if (mode == MODE_DOGFIGHT)
    {
        ball_count = player_count;
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;
            game_init_party_mode_physics(pl, ball_count);
            for (i = 1; i < ball_count; i++)
            {
                pl->vary.uv[i].p[0] += (float)i * 5.0f;
                pl->vary.uv[i].p[1] += 10.0f; /* Start in the air */
                pl->vary.uv[i].p[2] += (float)i * 5.0f;
            }
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = p;

        /* Auto-deploy wings */
        pl->fly_active = 1;
        pl->fly_pitch = 0.0f;
        pl->ammo = 20; /* Machine gun / missiles */
        pl->reload_timer = 0.0f;
    }
    else if (mode == MODE_EDITOR)
    {
        ball_count = 1;
        if (p == 0)
        {
            sol_load_vary(&pl->vary, &game_base);
            pl->sim_state = &pl->vary;
            pl->sim_owner = 1;
            game_init_party_mode_physics(pl, ball_count);

            /* Give editor a specific radius and position */
            pl->vary.uv[0].r = 0.1f;
            pl->vary.uv[0].p[1] = 10.0f;
        }
        else
        {
            pl->sim_state = &players[0].vary;
            pl->sim_owner = 0;
        }
        pl->ball_index = 0;

        /* No collision, free flight */
        pl->fly_active = 1;
        pl->fly_pitch = 0.0f;
        pl->action_prev = 0;
        pl->held_item = ITEM_BANANA; /* Default spawn item */
    }
    else
    {
        /* Race / Independent */
        ball_count = 1;
        sol_load_vary(&pl->vary, &game_base);
        pl->sim_state = &pl->vary;
        pl->sim_owner = 1;
        pl->ball_index = 0;

        if (p > 0)
        {
            pl->vary.uv[0].p[0] += (float)p * 1.5f;
            pl->vary.uv[0].p[2] += (float)p * 1.5f;
        }
    }

    /* Apply Stats to Physics Body */
    /* We iterate all balls this player owns */
    for (i = 0; i < pl->sim_state->uc; i++) {
        /* Mass is not directly exposed in v_ball but radius is. */
        /* Acceleration is handled by tilt. */
    }

    game_tilt_init(&pl->tilt);

    v_cpy(pl->gravity_dir, GRAVITY_DN);

    pl->jump_e = 1;
    pl->jump_b = 0;
    pl->goal_e = e ? 1 : 0;

    game_view_fly(&pl->view, pl->sim_state, 0.0f);

    v_cpy(pl->start_p, pl->sim_state->uv[pl->ball_index].p);

    pl->view_k = 1.0f;
    pl->view_time = 0.0f;
    pl->view_fade = 0.0f;
    pl->view_zoom_curr = 1.0f;
    pl->view_zoom_time = ZOOM_TIME;

    if (pl->sim_owner)
        sol_init_sim(pl->sim_state);

    game_cmd_timer(p);
    if (pl->goal_e) game_cmd_goalopen(p);
    game_cmd_init_balls(p, ball_count);
    game_cmd_updview(p);
}

int game_server_init(const char *file_name, int t, int e, int mode)
{
    struct { int x, y; } version;
    int i, p;

    game_server_free(file_name);

    if (!game_base_load(file_name))
        return (server_state = 0);

    player_count = config_get_d(CONFIG_MULTIBALL);
    if (player_count < 1) player_count = 1;
    if (player_count > MAX_PLAYERS) player_count = MAX_PLAYERS;

    server_state = 1;
    game_mode = mode;

    version.x = 0;
    version.y = 0;

    for (i = 0; i < game_base.dc; i++)
    {
        char *k = game_base.av + game_base.dv[i].ai;
        char *v = game_base.av + game_base.dv[i].aj;

        if (strcmp(k, "version") == 0)
            sscanf(v, "%d.%d", &version.x, &version.y);
    }

    input_init();

    game_cmd_map(file_name, version.x, version.y);
    game_cmd_ups();

    for (p = 0; p < player_count; p++)
    {
        game_player_init(p, t, e, mode);
    }

    game_cmd_eou();

    lockstep_clr(&server_step);

    return server_state;
}

void game_server_free(const char *next)
{
    int p;
    if (server_state)
    {
        sol_quit_sim();

        for (p = 0; p < player_count; p++)
        {
            if (players[p].sim_owner)
                sol_free_vary(&players[p].vary);
        }

        game_base_free(next);

        server_state = 0;
    }
}

/*---------------------------------------------------------------------------*/

static void game_update_view(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    /* Current view scale. */

    if (pl->view_zoom_time < ZOOM_TIME)
    {
        pl->view_zoom_time += dt;

        if (pl->view_zoom_time >= ZOOM_TIME)
        {
            pl->view_zoom_time = ZOOM_TIME;
            pl->view_zoom_curr = pl->view_zoom_end;
            pl->view_zoom_end = 0.0f;
        }
        else if (pl->view_zoom_time >= ZOOM_DELAY)
        {
            float a = (pl->view_zoom_time - ZOOM_DELAY) / (ZOOM_TIME - ZOOM_DELAY);

            a = easeInOutBack(a);

            pl->view_zoom_curr = pl->view_zoom_start + (pl->view_zoom_end - pl->view_zoom_start) * a;
        }
    }

    float SCL = pl->view_zoom_curr;

    float dc = pl->view.dc * (pl->jump_b > 0 ? 2.0f * fabsf(pl->jump_dt - 0.5f) : 1.0f);
    float da = 90.0f * input_get_r(p) * dt;
    float k;

    float M[16], v[3], Y[3] = { 0.0f, 1.0f, 0.0f };
    float view_v[3];

    float spd = (float) cam_speed(input_get_c(p)) / 1000.0f;

    /* Track manual rotation time. */

    if (da == 0.0f)
    {
        if (pl->view_time < 0.0f)
        {
            pl->view_fade = CLAMP(VIEW_FADE_MIN, -pl->view_time, VIEW_FADE_MAX);
            pl->view_time = 0.0f;
        }
        pl->view_time += dt;
    }
    else
    {
        if (pl->view_time > 0.0f)
        {
            pl->view_fade = 0.0f;
            pl->view_time = 0.0f;
        }
        pl->view_time -= dt;
    }

    /* Center the view about the ball. */

    v_cpy(pl->view.c, b->p);

    view_v[0] = -b->v[0];
    view_v[1] =  0.0f;
    view_v[2] = -b->v[2];

    /* Compute view vector. */

    if (spd >= 0.0f)
    {
        if (da == 0.0f)
        {
            float s;

            v_sub(pl->view.e[2], pl->view.p, pl->view.c);
            v_nrm(pl->view.e[2], pl->view.e[2]);

            s = fpowf(pl->view_time, 3.0f) / fpowf(pl->view_fade, 3.0f);
            s = CLAMP(0.0f, s, 1.0f);

            v_mad(pl->view.e[2], pl->view.e[2], view_v, v_len(view_v) * spd * s * dt);
        }
    }
    else
    {
        pl->view.e[2][0] = fsinf(V_RAD(pl->view.a));
        pl->view.e[2][1] = 0.0;
        pl->view.e[2][2] = fcosf(V_RAD(pl->view.a));
    }

    if (da != 0.0f)
    {
        m_rot(M, Y, V_RAD(da));
        m_vxfm(v, M, pl->view.e[2]);
        v_cpy(pl->view.e[2], v);
    }

    /* Arcade Camera Snap */
    if (config_get_d(CONFIG_PHYSICS))
    {
        float speed = v_len(b->v);
        if (speed > 5.0f)
        {
            float vel_n[3];
            v_cpy(vel_n, b->v);
            v_nrm(vel_n, vel_n);

            float target_n[3];
            v_scl(target_n, vel_n, -1.0f);

            v_lerp(pl->view.e[2], pl->view.e[2], target_n, 12.0f * dt);
            v_nrm(pl->view.e[2], pl->view.e[2]);
        }
    }

    v_crs(pl->view.e[0], pl->view.e[1], pl->view.e[2]);
    v_crs(pl->view.e[2], pl->view.e[0], pl->view.e[1]);
    v_nrm(pl->view.e[0], pl->view.e[0]);
    v_nrm(pl->view.e[2], pl->view.e[2]);

    k = 1.0f + v_dot(pl->view.e[2], view_v) / 10.0f;

    pl->view_k = pl->view_k + (k - pl->view_k) * dt;

    if (pl->view_k < 0.5f) pl->view_k = 0.5;

    v_scl(v,    pl->view.e[1], SCL * pl->view.dp * pl->view_k);
    v_mad(v, v, pl->view.e[2], SCL * pl->view.dz * pl->view_k);
    v_add(pl->view.p, v, b->p);

    v_cpy(pl->view.c, b->p);
    v_mad(pl->view.c, pl->view.c, pl->view.e[1], SCL * dc);

    pl->view.a = V_DEG(fatan2f(pl->view.e[2][0], pl->view.e[2][2]));

    game_cmd_updview(p);
}

static void game_update_time(int p, float dt, int b)
{
    struct server_player *pl = &players[p];
    if (b)
    {
        pl->time_elapsed += dt;

        if (pl->time_limit > 0.0f && pl->time_elapsed > pl->time_limit)
            pl->time_elapsed = pl->time_limit;

        pl->timer = fabsf(pl->time_limit - pl->time_elapsed);

        game_cmd_timer(p);
    }
}

static void zoom_init(int p, float target)
{
    struct server_player *pl = &players[p];
    pl->view_zoom_time = 0.0f;
    pl->view_zoom_start = pl->view_zoom_curr;
    pl->view_zoom_end = CLAMP(ZOOM_MIN, target, ZOOM_MAX);
}

static int game_update_state(int p, int bt)
{
    struct server_player *pl = &players[p];
    struct b_goal *zp;
    int hi;

    /* Test for an item. */

    if (bt && (hi = sol_item_test(pl->sim_state, NULL, ITEM_RADIUS)) != -1)
    {
        struct v_item *hp = pl->sim_state->hv + hi;

        game_cmd_pkitem(p, hi);

        if (hp->t == ITEM_COIN)
        {
            pl->coins += hp->n;
            if (!pl->is_cpu) profile_add_stat(STAT_COINS, hp->n);
            game_cmd_coins(p);
        }
        else if (hp->t == ITEM_CLOCK)
        {
            const float value = (float) hp->n;

            audio_play(AUD_CLOCK, 1.f);

            if (pl->time_limit > 0.0f)
                pl->time_limit = pl->time_limit + value;
            else
                pl->time_elapsed = MAX(0.0f, pl->time_elapsed - value);

            game_update_time(p, 0.0f, bt);
        }
        else if (hp->t == ITEM_GROW || hp->t == ITEM_SHRINK)
        {
            switch (grow_init(p, hp->t))
            {
                case -1:
                    audio_play(AUD_SHRINK, 1.0f);
                    zoom_init(p, pl->sim_state->uv->sizes[pl->sim_state->uv->size] / pl->sim_state->uv->sizes[1]);
                    break;

                case +1:
                    audio_play(AUD_GROW, 1.0f);
                    zoom_init(p, pl->sim_state->uv->sizes[pl->sim_state->uv->size] / pl->sim_state->uv->sizes[1]);
                    break;

                case 0:
                    break;
            }
        }
        else if (hp->t == ITEM_SPEED)
        {
            /* Speed Boost */
            struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
            float fwd[3];
            if (v_len(b->v) > 0.1f) {
                v_cpy(fwd, b->v);
                v_nrm(fwd, fwd);
                v_mad(b->v, b->v, fwd, 15.0f);
            }
            audio_play(AUD_JUMP, 1.0f);
        }
        else if (hp->t == ITEM_SUPER_JUMP)
        {
            /* Marble Blast Style Super Jump */
            struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
            /* Launch straight up with immense force */
            b->v[1] += 30.0f;
            audio_play(AUD_JUMP, 2.0f);
        }
        else if (hp->t == ITEM_GYRO)
        {
            pl->gyro_active = 1;
            pl->gyro_timer = 20.0f;
            game_cmd_gyro_state(p, 1, pl->gyro_timer);
            audio_play(AUD_JUMP, 1.0f);
        }
        else if (hp->t == ITEM_SHOCK)
        {
            pl->shock_active = 1;
            pl->shock_timer = 30.0f;
            /* TODO: Add CMD_SHOCK_STATE for HUD */
            audio_play(AUD_COIN, 1.0f);
        }
        else if (hp->t == ITEM_SUPER_SPEED)
        {
            pl->speed_active = 1;
            pl->speed_timer = 15.0f;
            /* TODO: Add CMD_SPEED_STATE for HUD */
            audio_play(AUD_COIN, 1.0f);
        }
        else if (hp->t == ITEM_MISSILE)
        {
            pl->held_item = ITEM_MISSILE;
            audio_play(AUD_COIN, 1.f);
        }
        else if (hp->t == ITEM_BANANA)
        {
            pl->held_item = ITEM_BANANA;
            audio_play(AUD_COIN, 1.f);
        }

        /* Check for item consumption */
        if (hp->t != ITEM_NONE)
        {
            audio_play(AUD_COIN, 1.f);
            hp->t = ITEM_NONE;
        }
    }

    /* Test for a switch. */

    if (sol_swch_test(pl->sim_state, game_proxy_enq, 0) == SWCH_INSIDE)
        audio_play(AUD_SWITCH, 1.f);

    /* Test for a jump. */

    if (pl->jump_e == 1 && pl->jump_b == 0 && (sol_jump_test(pl->sim_state, pl->jump_p, 0) ==
                                       JUMP_INSIDE))
    {
        pl->jump_b  = 1;
        pl->jump_e  = 0;
        pl->jump_dt = 0.f;

        audio_play(AUD_JUMP, 1.f);

        game_cmd_jump(p, 1);
    }
    if (pl->jump_e == 0 && pl->jump_b == 0 && (sol_jump_test(pl->sim_state, pl->jump_p, 0) ==
                                       JUMP_OUTSIDE))
    {
        pl->jump_e = 1;
        game_cmd_jump(p, 0);
    }

    /* Test for a goal. */

    if (bt && pl->goal_e && (zp = sol_goal_test(pl->sim_state, NULL, 0)))
    {
        audio_play(AUD_GOAL, 1.0f);
        return GAME_GOAL;
    }

    /* Test for time-out. */

    if (bt && pl->time_limit > 0.0f && pl->time_elapsed >= pl->time_limit)
    {
        audio_play(AUD_TIME, 1.0f);
        return GAME_TIME;
    }

    /* Test for fall-out. */

    if (bt && (pl->sim_state->base->vc == 0 || pl->sim_state->uv[pl->ball_index].p[1] < pl->sim_state->base->vv[0].p[1]))
    {
        audio_play(AUD_FALL, 1.0f);
        return GAME_FALL;
    }

    /* Party Mode Specific Fall Out Logic for shared balls (Soccer, Tennis) */
    if (bt && pl->sim_owner) {
        if (game_mode == MODE_SOCCER || game_mode == MODE_TENNIS) {
            struct v_ball *game_ball = &pl->sim_state->uv[0];
            /* Check if game ball fell */
            if (pl->sim_state->base->vc == 0 || game_ball->p[1] < pl->sim_state->base->vv[0].p[1]) {
                audio_play(AUD_FALL, 1.0f);

                /* Reset Ball */
                v_zero(game_ball->v);
                v_zero(game_ball->p);
                game_ball->p[1] = 5.0f;

                game_cmd_upd_all_balls(p);

                hud_show_toast("Ball Reset!");
            }
        }
    }

    return GAME_NONE;
}

static void game_fly_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    /* Editor Free-Cam Logic */
    if (game_mode == MODE_EDITOR)
    {
        float fwd[3], right[3], up[3] = {0, 1, 0};

        /* View Forward is -e[2] */
        v_cpy(fwd, pl->view.e[2]);
        v_scl(fwd, fwd, -1.0f);

        /* View Right is e[0] */
        v_cpy(right, pl->view.e[0]);

        float move_speed = 30.0f;

        /* WASD mapped to Z and X input? We assume input_z is fwd/back, input_x is left/right */
        float z_input = input_get_z(p) / ANGLE_BOUND; /* 1.0 = forward */

        /* Directly set velocity instead of accelerating */
        v_zero(b->v);
        v_mad(b->v, b->v, fwd, z_input * move_speed);

        /* Stop gravity */
        b->v[1] += (input_get_action(p) ? move_speed : 0.0f) * dt;

        /* Handle Place Item Input via Dash button */
        int dash = input_get_dash(p);
        if (dash && !pl->punch_state) { /* Reuse punch_state as a debounce flag for dash */
            pl->punch_state = 1;

            /* Spawn item 10 units in front of camera */
            float spawn_pos[3];
            v_cpy(spawn_pos, b->p);
            v_mad(spawn_pos, spawn_pos, fwd, 10.0f);

            /* Tell clients to render the new item */
            cmd.type = CMD_PLACE_ITEM;
            v_cpy(cmd.placeitem.p, spawn_pos);
            cmd.placeitem.t = pl->held_item;
            cmd.placeitem.n = 1;
            game_proxy_enq(&cmd);

            audio_play(AUD_GOAL, 1.0f);
        } else if (!dash) {
            pl->punch_state = 0;
        }

        return;
    }

    /* Control Pitch */
    float z_input = input_get_z(p) / ANGLE_BOUND; /* Normalize to -1..1 range approximately */
    /* If Stick Up (Forward), Pitch Down. If Stick Down (Back), Pitch Up. */

    pl->fly_pitch += z_input * 2.0f * dt; /* 2 degrees per second? */
    pl->fly_pitch = CLAMP(-45.0f, pl->fly_pitch, 45.0f);

    /* Physics */
    float speed = v_len(b->v);

    if (speed > 1.0f)
    {
        float lift_dir[3] = { 0.0f, 1.0f, 0.0f }; /* Up for now. */

        float lift_force = speed * speed * 0.05f * (pl->fly_pitch + 10.0f); /* Bias +10 deg */
        float drag_force = speed * speed * 0.01f;

        float drag_vec[3];
        v_cpy(drag_vec, b->v);
        v_nrm(drag_vec, drag_vec);
        v_scl(drag_vec, drag_vec, -drag_force);

        float lift_vec[3];
        v_scl(lift_vec, lift_dir, lift_force);

        /* Apply forces directly to velocity */
        v_mad(b->v, b->v, drag_vec, dt);
        v_mad(b->v, b->v, lift_vec, dt);
    }
}

static void game_fight_step(int p, float dt)
{
    struct server_player *pl = &players[p];

    if (input_get_action(p) && !pl->action_prev && pl->punch_state == 0) {
        pl->punch_state = 1;
        pl->punch_timer = 0.0f;
        game_cmd_punch(p, 1);
        audio_play(AUD_JUMP, 1.0f); /* Sound */
    }

    if (pl->punch_state == 1) {
        pl->punch_timer += dt;
        if (pl->punch_timer > 0.2f) { /* Extension time */
            pl->punch_state = 2;
        }

        /* Hit detection */
        int i;
        struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
        float punch_range = b->r * 2.0f;
        float punch_vec[3];

        v_cpy(punch_vec, pl->view.e[2]);
        v_scl(punch_vec, punch_vec, -1.0f);

        for (i = 0; i < pl->sim_state->uc; i++) {
            if (i == pl->ball_index) continue;

            struct v_ball *other = &pl->sim_state->uv[i];
            float dist_vec[3];
            v_sub(dist_vec, other->p, b->p);
            float dist = v_len(dist_vec);

            if (dist < (b->r + other->r + punch_range)) {
                float d = v_dot(punch_vec, dist_vec);
                if (d > 0) {
                    /* HIT */
                    float force[3];
                    v_cpy(force, punch_vec);
                    v_scl(force, force, 20.0f * dt);
                    v_add(other->v, other->v, force);

                    audio_play(AUD_BUMPL, 1.0f);
                }
            }
        }

    }
    else if (pl->punch_state == 2) {
        pl->punch_timer -= dt;
        if (pl->punch_timer <= 0.0f) {
            pl->punch_state = 0;
            game_cmd_punch(p, 0);
        }
    }
}

static void game_billiards_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    int i;

    /* Billiards Logic */

    int action = input_get_action(p);

    if (action) {
        /* Charging */
        pl->shot_power += dt * 2.0f;
        if (pl->shot_power > 1.0f) pl->shot_power = 1.0f;
    } else {
        if (pl->shot_power > 0.0f) {
            /* Release - Shoot */
            struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
            float fwd[3];
            /* View Forward = -e[2] */
            v_cpy(fwd, pl->view.e[2]);
            v_scl(fwd, fwd, -1.0f);

            float force = pl->shot_power * 30.0f; /* Max speed */
            v_mad(b->v, b->v, fwd, force);

            audio_play(AUD_BUMPL, pl->shot_power);

            pl->shot_power = 0.0f;
        }
    }

    /* Friction */
    for (i = 0; i < pl->sim_state->uc; i++) {
        struct v_ball *b = &pl->sim_state->uv[i];
        if (v_len(b->v) > 0.0f) {
             v_scl(b->v, b->v, 0.99f);
             if (v_len(b->v) < 0.05f) v_zero(b->v);
        }
    }

    /* Pocket Detection */
    struct s_vary *vary = pl->sim_state;
    if (vary->base->zc > 0) {
        for (i = 0; i < vary->uc; i++) {
            struct v_ball *b = &vary->uv[i];
            struct b_goal *goal = &vary->base->zv[0];

            float d[3];
            v_sub(d, b->p, goal->p);
            if (v_len(d) < (b->r + goal->r)) {
                if (i == 0) {
                    /* Scratch */
                    audio_play(AUD_FALL, 1.0f);
                    v_cpy(b->p, pl->start_p);
                    v_zero(b->v);
                } else {
                    /* Scored */
                    b->p[1] = -1000.0f; /* Remove */
                    v_zero(b->v);

                    pl->coins += 100;
                    game_cmd_coins(p);
                    audio_play(AUD_GOAL, 1.0f);
                }
            }
        }
    }
}

static void game_race_item_step(int p, float dt)
{
    if (p < 0 || p >= MAX_PLAYERS) return;
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
    int action = input_get_action(p);

    if (pl->stun_timer > 0.0f) {
        pl->stun_timer -= dt;
        /* Spin out effect */

        /* Apply torque to spin */
        /* Since w is directly updated in sol_step via sol_rotate/pendulum, */
        /* we can add to w. */
        /* b->w[1] += ... */
        /* Or just rotate view? */
        /* Let's just dampen control */
        pl->tilt.rx *= 0.1f;
        pl->tilt.rz *= 0.1f;
        return;
    }

    if (action && !pl->action_prev && pl->held_item) {
        if (pl->held_item == ITEM_MISSILE) {
            /* Fire Missile */
            /* Simplified: Scan for target in front and stun them */
            int i;
            float best_dist = 100.0f;
            int target = -1;

            float fwd[3];
            v_cpy(fwd, pl->view.e[2]);
            v_scl(fwd, fwd, -1.0f);

            for(i=0; i<player_count; i++) {
                if (i==p) continue;
                struct v_ball *other = &pl->sim_state->uv[players[i].ball_index];
                float vec[3];
                v_sub(vec, other->p, b->p);

                float dist = v_len(vec);
                v_nrm(vec, vec);

                if (dist < best_dist && v_dot(fwd, vec) > 0.9f) {
                    best_dist = dist;
                    target = i;
                }
            }

            if (target != -1) {
                players[target].stun_timer = 2.0f;
                audio_play(AUD_BUMPL, 1.0f);

                /* Knockback */
                float force[3];
                v_cpy(force, fwd);
                v_scl(force, force, 20.0f);
                v_add(pl->sim_state->uv[players[target].ball_index].v,
                      pl->sim_state->uv[players[target].ball_index].v, force);
            } else {
                audio_play(AUD_BUMPM, 1.0f); /* Miss sound */
            }

            pl->held_item = 0;
        }
        else if (pl->held_item == ITEM_BANANA) {
            /* Drop Banana */
            /* Spawn static hazard? */
            /* For MVP: Just drop it behind. If someone hits it (collision check), they slip. */
            /* We need to allocate a new hazard entity. */
            /* Reusing ITEM system: Spawn ITEM_BANANA at current position */
            /* Wait, sol_load_vary loads items from file. We can't easily add dynamic items without dynamic array. */
            /* But items are just an array in vary struct. */
            /* We can resize it? Or reuse ITEM_NONE slots? */

            /* Scan for free item slot */
            int i;
            for(i=0; i < pl->sim_state->hc; i++) {
                if (pl->sim_state->hv[i].t == ITEM_NONE) {
                    struct v_item *h = &pl->sim_state->hv[i];
                    h->t = ITEM_BANANA;
                    h->n = 1;

                    /* Transform world position to item local space? */
                    /* Assume item space is Identity for spawned items or use simpler logic */
                    /* Actually, items have mi/mj parents. If we use existing slot, we inherit parent. */
                    /* If parent moves, banana moves. This might be weird. */
                    /* MVP: Just stun self to simulate slipping? No. */

                    /* Let's just play sound for now. */
                    audio_play(AUD_FALL, 1.0f);
                    pl->held_item = 0;
                    break;
                }
            }
        }
    }
}

static void game_baseball_step(int p, float dt)
{
    if (p < 0 || p >= MAX_PLAYERS) return;
    struct server_player *pl = &players[p];

    /* Server controls out-of-bounds / inning logic */
    if (p == 0 && pl->sim_owner) {
        struct v_ball *ball = &pl->sim_state->uv[0];

        /* If ball falls below a threshold or goes out of bounds laterally */
        if (ball->p[1] < 0.2f || fabsf(ball->p[0]) > 40.0f || fabsf(ball->p[2]) > 60.0f) {
            /* Foul or Reset */
            v_zero(ball->v);
            ball->p[0] = 0.0f;
            ball->p[1] = 5.0f; /* Drop it somewhere */
            ball->p[2] = -15.0f; /* Back near pitcher */
            game_cmd_upd_all_balls(p);
        }
    }

    /* Pitcher Logic (Role 0) */
    if (pl->baseball_role == 0) {
        if (input_get_action(p)) {
            pl->baseball_charge += dt;
            if (pl->baseball_charge > 1.0f) pl->baseball_charge = 1.0f;
            game_cmd_dash_charge(p, pl->baseball_charge); /* Show power meter */
        } else if (pl->baseball_charge > 0.0f) {
            /* Throw */
            struct v_ball *ball = &pl->sim_state->uv[0];
            struct v_ball *me = &pl->sim_state->uv[pl->ball_index];

            /* Reset ball to pitcher hand */
            v_cpy(ball->p, me->p);
            ball->p[1] += 1.0f;

            /* Pitching direction includes some curve based on X/Z tilt */
            float fwd[3];
            v_cpy(fwd, pl->view.e[2]);
            v_scl(fwd, fwd, -1.0f);

            /* Apply spin/curve based on tilt (input x) */
            float curve = input_get_x(p) * 0.05f;
            fwd[0] += curve;

            v_nrm(fwd, fwd);

            float speed = 20.0f + (pl->baseball_charge * 40.0f); /* Fastball up to 60 */
            v_scl(ball->v, fwd, speed);

            pl->baseball_charge = 0.0f;
            game_cmd_dash_charge(p, 0.0f);
            audio_play(AUD_JUMP, 1.0f);
        }
    }

    /* Batter Logic (Role 1) */
    else {
        if (input_get_action(p) && !pl->action_prev) {
            /* Swing */
            game_cmd_punch(p, 1);
            audio_play(AUD_JUMP, 1.0f);

            struct v_ball *ball = &pl->sim_state->uv[0];
            struct v_ball *me = &pl->sim_state->uv[pl->ball_index];

            float dist_vec[3];
            v_sub(dist_vec, ball->p, me->p);
            float dist = v_len(dist_vec);

            /* Sweet spot hit detection */
            if (dist < 3.5f) {
                float fwd[3];
                v_cpy(fwd, pl->view.e[2]);
                v_scl(fwd, fwd, -1.0f);

                /* Check if ball is in front of batter */
                if (v_dot(fwd, dist_vec) > 0) {
                    /* Contact! */

                    /* Aim direction based on swing timing (distance on Z axis roughly) */
                    /* Add Upward Angle based on input_z */
                    float loft = 0.5f + (input_get_z(p) * 0.05f);
                    fwd[1] += loft;
                    v_nrm(fwd, fwd);

                    /* Hit speed depends on incoming speed + power */
                    float incoming_speed = v_len(ball->v);
                    float hit_speed = incoming_speed * 1.5f + 20.0f;

                    v_scl(ball->v, fwd, hit_speed);
                    audio_play(AUD_BUMPL, 1.0f);

                    if (hit_speed > 60.0f) {
                        hud_show_toast("HOME RUN!");
                        pl->coins += 100;
                    } else {
                        hud_show_toast("HIT!");
                        pl->coins += 10;
                    }
                    game_cmd_coins(p);
                }
            }
        } else {
            game_cmd_punch(p, 0);
        }
    }
}

static void game_tennis_step(int p, float dt)
{
    struct server_player *pl = &players[p];

    /* Swing Mechanic */
    if (input_get_action(p) && !pl->action_prev) {
        /* Swing Racket */
        game_cmd_punch(p, 1); /* Visuals */
        audio_play(AUD_JUMP, 1.0f);

        /* Hit detection against Tennis Ball (Index 0) */
        struct v_ball *tennis_ball = &pl->sim_state->uv[0];
        struct v_ball *me = &pl->sim_state->uv[pl->ball_index];

        float dist_vec[3];
        v_sub(dist_vec, tennis_ball->p, me->p);
        float dist = v_len(dist_vec);

        /* Range check (Racket reach) */
        if (dist < 2.5f) {
            /* Check angle - must be in front */
            float fwd[3];
            v_cpy(fwd, pl->view.e[2]);
            v_scl(fwd, fwd, -1.0f);

            if (v_dot(fwd, dist_vec) > 0) {
                /* HIT! */
                float force[3];
                v_cpy(force, fwd);
                /* Add some lift */
                force[1] += 0.5f;
                v_nrm(force, force);

                v_scl(force, force, 30.0f); /* Smash speed */
                v_cpy(tennis_ball->v, force);

                audio_play(AUD_BUMPL, 1.0f);
            }
        }
    }
    else {
        game_cmd_punch(p, 0);
    }

    /* Server Logic: Scoring and Sound */
    if (p == 0 && pl->sim_owner) {
        struct v_ball *b = &pl->sim_state->uv[0];

        /* Tennis Ball bounce sound check based on vertical velocity change */
        /* Normally solid_sim_sol handles this, but since we modify ball speed/position manually in party modes
           we can also add explicit hit sounds. The swing hit already plays AUD_BUMPL. */

        /* Out of bounds / Ground touch logic */
        /* Assuming court is flat at y=0. Ball radius 0.3. */
        /* If ball touches ground (y < 0.3) check position */
        if (b->p[1] < 0.35f) {
            /* Bounce logic handled by physics? Yes. */
            /* But we need to track bounces or out of bounds. */
            /* Simplified: If it goes too far Z, point. */

            if (b->p[2] > 15.0f) {
                /* Past Team 1 baseline -> Point for Team 0 */
                hud_show_toast("Point Team Red!");
                /* Reset */
                v_zero(b->v);
                b->p[0] = 0; b->p[1] = 5; b->p[2] = 0;
            }
            else if (b->p[2] < -15.0f) {
                /* Past Team 0 baseline -> Point for Team 1 */
                hud_show_toast("Point Team Blue!");
                v_zero(b->v);
                b->p[0] = 0; b->p[1] = 5; b->p[2] = 0;
            }
        }

        /* Net Logic (Physical net exists in geometry? Or invisible wall?) */
        /* If no geometry, enforce net collision here */
        if (fabsf(b->p[2]) < 0.5f && b->p[1] < 1.0f) {
            /* Hit net */
            b->v[2] *= -0.5f; /* Bounce back */
        }
    }
}

static void game_soccer_step(int p, float dt)
{
    struct server_player *pl = &players[p];

    /* Only Server (Player 0) handles game logic like scoring */
    if (p == 0 && pl->sim_owner) {
        struct v_ball *soccer_ball = &pl->sim_state->uv[0];

        /* Check Goal */
        if (soccer_ball->p[2] > 20.0f) {
            /* Goal for Red Team (Team 0) */
            /* Assuming Blue Goal is at +Z */
            audio_play(AUD_GOAL, 1.0f);
            hud_show_toast("GOAL! Red Team Scores!");

            /* Reset */
            game_respawn(0); /* Respawns everyone? No, game_respawn(p) respawns p. */
            /* We need a full reset */

            /* Add Score */
            /* How to track team score? */
            /* Use player coins for now. */
            /* Give coins to all Red Team players */
            int i;
            for(i=0; i<player_count; i++) {
                if (players[i].team == 0) {
                    players[i].coins++;
                    game_cmd_coins(i);
                }
            }

            /* Reset Positions */
            v_zero(soccer_ball->v);
            v_zero(soccer_ball->p);
            soccer_ball->p[1] = 5.0f; /* Drop */

             for(i=0; i<player_count; i++) {
                game_respawn(i);
            }
        }
        else if (soccer_ball->p[2] < -20.0f) {
            /* Goal for Blue Team (Team 1) */
             audio_play(AUD_GOAL, 1.0f);
             hud_show_toast("GOAL! Blue Team Scores!");

            int i;
            for(i=0; i<player_count; i++) {
                if (players[i].team == 1) {
                    players[i].coins++;
                    game_cmd_coins(i);
                }
            }

            /* Reset Positions */
            v_zero(soccer_ball->v);
            v_zero(soccer_ball->p);
            soccer_ball->p[1] = 5.0f;

             for(i=0; i<player_count; i++) {
                game_respawn(i);
            }
        }
    }
}

static void game_shot_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    /* Rail Shooter: Ball moves automatically along path (if defined) or constant speed */
    /* For MVP, just constant forward */
    float fwd[3];
    v_cpy(fwd, pl->view.e[2]);
    v_scl(fwd, fwd, -1.0f);
    fwd[1] = 0.0f;
    v_nrm(fwd, fwd);
    v_scl(b->v, fwd, 5.0f);

    /* Cursor Control */
    float dx = input_get_x(p) * 2.0f * dt; /* Stick X */
    float dy = input_get_z(p) * 2.0f * dt; /* Stick Y */

    pl->cursor_x += dx;
    pl->cursor_y += dy;

    /* Clamp Cursor */
    if (pl->cursor_x < -1.0f) pl->cursor_x = -1.0f;
    if (pl->cursor_x > 1.0f)  pl->cursor_x = 1.0f;
    if (pl->cursor_y < -1.0f) pl->cursor_y = -1.0f;
    if (pl->cursor_y > 1.0f)  pl->cursor_y = 1.0f;

    /* TODO: Sync cursor position to HUD via CMD? */
    /* We don't have CMD_CURSOR yet. */

    int action = input_get_action(p);
    if (action && !pl->action_prev)
    {
        if (pl->ammo > 0) {
            pl->ammo--;
            audio_play(AUD_BUMPS, 1.0f); /* Pew pew */

            /* Raycast Logic */
            /* Construct ray from camera through cursor position */
            /* Check intersection with enemies (other balls or items) */
            /* For now, just play sound */
        } else {
            audio_play(AUD_BUMPM, 0.5f); /* Click */
        }
    }

    /* Reload */
    if (pl->ammo < 6) {
        pl->reload_timer += dt;
        if (pl->reload_timer > 1.0f) {
            pl->ammo++;
            pl->reload_timer = 0.0f;
        }
    }
}

static void game_boat_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    /* Inputs: Rotate Left/Right act as paddles */
    /* Wait, usually Monkey Boat uses L/R shoulder buttons or stick L/R? */
    /* Let's map 'left' turn input to left paddle, 'right' to right paddle? */
    /* Or specifically L1/R1 buttons if available. */
    /* Current input struct has r, x, z, c, action, dash. */
    /* Let's use 'x' (tilt x) and 'rot' (r) as paddles? No. */
    /* Let's assume input_get_action is Paddle Both? No. */
    /* We need new inputs or reuse existing. */
    /* Reuse: Left/Right Tilt = Steering? */
    /* Monkey Boat: Alternate L/R to accelerate. */

    /* Simplified Model: */
    /* Tap Left (Tilt X < -0.5) -> Paddle Left */
    /* Tap Right (Tilt X > 0.5) -> Paddle Right */

    float x = input_get_x(p);

    if (x < -0.5f && pl->paddle_left_timer <= 0.0f) {
        pl->paddle_left_timer = 0.5f; /* Cooldown */
        pl->boat_speed += 2.0f;
    }
    if (x > 0.5f && pl->paddle_right_timer <= 0.0f) {
        pl->paddle_right_timer = 0.5f;
        pl->boat_speed += 2.0f;
    }

    pl->paddle_left_timer -= dt;
    pl->paddle_right_timer -= dt;

    /* Decay Speed */
    pl->boat_speed -= dt * 1.0f;
    if (pl->boat_speed < 0.0f) pl->boat_speed = 0.0f;
    if (pl->boat_speed > 20.0f) pl->boat_speed = 20.0f;

    /* Apply Velocity in Camera Direction */
    float dir[3];
    v_cpy(dir, pl->view.e[2]);
    v_scl(dir, dir, -1.0f);
    dir[1] = 0.0f;
    v_nrm(dir, dir);

    v_scl(b->v, dir, pl->boat_speed);

    /* Steering based on paddle imbalance? Or just stick? */
    /* If strictly paddling: Left paddle pushes Right. Right pushes Left. */
    /* For now, just forward movement. */
}

static void game_golf_step(int p, float dt)
{
    if (p < 0 || p >= MAX_PLAYERS) return;
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
    int action = input_get_action(p);

    if (pl->shot_state == 0) /* Aim */
    {
        /* Rotate view */
        pl->tilt.rx = 0;
        pl->tilt.rz = 0;

        float r = input_get_r(p);
        if (r != 0.0f)
        {
            /* Rotate view vector */
            /* This requires game_server to manage view rotation state for golf */
            /* Currently we rely on client sending inputs that map to camera rotation? */
            /* Actually game_update_view in client handles view. */
            /* But aim vector is derived from view. */
        }

        if (action) {
            pl->shot_state = 1;
            pl->shot_power = 0.0f;
        }
    }
    else if (pl->shot_state == 1) /* Power */
    {
        pl->shot_power += dt * 1.5f;
        if (pl->shot_power > 1.0f) pl->shot_power = 1.0f; /* Ping pong? */

        if (!action) {
            /* Shoot */
            pl->shot_state = 2;
            pl->golf_strokes++;

            float fwd[3];
            v_cpy(fwd, pl->view.e[2]);
            v_scl(fwd, fwd, -1.0f);
            /* Flatten Y */
            fwd[1] = 0.0f;
            v_nrm(fwd, fwd);

            float force = pl->shot_power * 60.0f; /* Stronger than billiards */
            v_mad(b->v, b->v, fwd, force);

            audio_play(AUD_BUMPL, pl->shot_power);
        }
    }
    else if (pl->shot_state == 2) /* Rolling */
    {
        /* High Friction */
        if (v_len(b->v) > 0.0f) {
             v_scl(b->v, b->v, 0.98f); /* Grass friction */
             if (v_len(b->v) < 0.1f) {
                 v_zero(b->v);
                 pl->shot_state = 0;
                 pl->shot_power = 0.0f;
             }
        }

        /* Sink Detection */
        if (pl->sim_state->base->zc > 0) {
            struct b_goal *goal = &pl->sim_state->base->zv[0];
            float dist_vec[3];
            v_sub(dist_vec, b->p, goal->p);
            /* Flatten Y for cylinder hit check, or use 3D dist */
            dist_vec[1] = 0.0f;
            float dist = v_len(dist_vec);

            if (dist < goal->r && v_len(b->v) < 5.0f && b->p[1] >= goal->p[1]) {
                /* Ball is over the hole and moving slow enough to drop in */
                /* Apply strong artificial gravity towards hole center */
                b->v[0] += (goal->p[0] - b->p[0]) * dt * 10.0f;
                b->v[2] += (goal->p[2] - b->p[2]) * dt * 10.0f;

                /* Once it falls deep enough, consider it sunk */
                if (b->p[1] < goal->p[1] - b->r) {
                    if (pl->status == GAME_NONE) {
                        v_zero(b->v);
                        b->p[1] = -1000.0f; /* Remove */
                        pl->status = GAME_GOAL;
                        game_cmd_status(p);
                        audio_play(AUD_GOAL, 1.0f);
                        hud_show_toast("Hole in One!");
                    }
                }
            }
        }
    }
}

static void game_bowling_step(int p, float dt)
{
    struct server_player *pl = &players[p];

    if (pl->shot_state == 0)
    {
        float dx = input_get_x(p);
        struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
        b->p[0] += dx * 5.0f * dt;

        if (b->p[0] < -5.0f) b->p[0] = -5.0f;
        if (b->p[0] >  5.0f) b->p[0] =  5.0f;

        if (input_get_action(p)) {
             pl->shot_state = 1;
             pl->shot_power = 0.0f;
        }
    }
    else if (pl->shot_state == 1)
    {
        pl->shot_power += dt * 2.0f;
        if (pl->shot_power > 1.0f) pl->shot_power = 1.0f;

        if (!input_get_action(p)) {
            pl->shot_state = 2;
            struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
            float fwd[3] = {0,0,1};

            float force = pl->shot_power * 40.0f;
            v_mad(b->v, b->v, fwd, force);
            audio_play(AUD_BUMPL, pl->shot_power);
        }
    }
    else if (pl->shot_state == 2)
    {
        int moving = 0;
        int i;
        for (i = 0; i < pl->sim_state->uc; i++) {
            struct v_ball *b = &pl->sim_state->uv[i];

             v_scl(b->v, b->v, 0.99f);
             if (v_len(b->v) < 0.05f) v_zero(b->v);
             else moving = 1;
        }

        if (!moving) {
             int pins_down = 0;
             for (i = 1; i < pl->sim_state->uc; i++) {
                 struct v_ball *b = &pl->sim_state->uv[i];
                 float d[3];
                 v_sub(d, b->p, pl->bowling_pin_start[i-1]);
                 if (v_len(d) > 0.5f) {
                     pins_down++;
                     b->p[1] = -1000.0f;
                 }
             }

             int new_points = pins_down;
             pl->coins = new_points * 100;
             game_cmd_coins(p);

             if (pins_down == 10 || pl->bowling_throw == 2) {
                 pl->bowling_frame++;
                 pl->bowling_throw = 1;
                 game_respawn(p);
                 for (i = 1; i < pl->sim_state->uc; i++) {
                     v_cpy(pl->sim_state->uv[i].p, pl->bowling_pin_start[i-1]);
                     v_zero(pl->sim_state->uv[i].v);
                 }
             } else {
                 pl->bowling_throw++;
                 v_cpy(pl->sim_state->uv[0].p, pl->start_p);
                 v_zero(pl->sim_state->uv[0].v);
             }

             pl->shot_state = 0;
        }
    }
}

static void game_hammer_step(int p, float dt)
{
    if (p < 0 || p >= MAX_PLAYERS) return;
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    if (!pl->hammer_thrown) {
        /* Player spins the stick (or uses Left/Right tilt) to build speed */
        float spin_input = input_get_x(p); /* Simplified: tilt X left/right to spin */

        pl->hammer_spin_speed += fabsf(spin_input) * dt;
        if (pl->hammer_spin_speed > 20.0f) pl->hammer_spin_speed = 20.0f;

        /* If no input, decay spin */
        if (fabsf(spin_input) < 0.1f) {
            pl->hammer_spin_speed -= dt * 2.0f;
            if (pl->hammer_spin_speed < 0.0f) pl->hammer_spin_speed = 0.0f;
        }

        pl->hammer_angle += pl->hammer_spin_speed * dt;

        /* Sync visual HUD metric using dash charge meter */
        game_cmd_dash_charge(p, pl->hammer_spin_speed / 20.0f);

        if (input_get_action(p) && !pl->action_prev) {
            /* Throw! */
            pl->hammer_thrown = 1;
            game_cmd_dash_charge(p, 0.0f);

            /* Release angle determines direction, but for simplicity let's aim forward and use speed */
            float fwd[3];
            v_cpy(fwd, pl->view.e[2]);
            v_scl(fwd, fwd, -1.0f);

            /* Add arc */
            fwd[1] += 0.5f;
            v_nrm(fwd, fwd);

            v_scl(b->v, fwd, pl->hammer_spin_speed * 5.0f);
            audio_play(AUD_JUMP, 2.0f);
        }
    } else {
        /* Wait for ball to land/stop to score */
        if (v_len(b->v) < 0.1f && b->p[1] < 1.0f) {
            /* Calculate distance from origin */
            float dist = sqrtf(b->p[0]*b->p[0] + b->p[2]*b->p[2]);
            if (pl->status == GAME_NONE) {
                pl->coins = (int)dist;
                game_cmd_coins(p);
                hud_show_toast("Throw Complete!");
                pl->status = GAME_GOAL;
                game_cmd_status(p);
            }
        }
    }
}

static void game_mole_step(int p, float dt)
{
    if (p < 0 || p >= MAX_PLAYERS) return;
    struct server_player *pl = &players[p];

    if (pl->sim_owner) {
        int mi, j;
        for (mi = 0; mi < 16; mi++) {
            struct v_ball *mole = &pl->sim_state->uv[player_count + mi];

            pl->mole_timers[mi] -= dt;

            if (pl->mole_grid[mi] == 0) {
                /* Down */
                if (mole->p[1] > -2.0f) mole->p[1] -= dt * 5.0f;
                if (pl->mole_timers[mi] <= 0.0f) {
                    if ((rand() % 100) < 2) { /* Random chance to pop up */
                        pl->mole_grid[mi] = 1;
                        pl->mole_timers[mi] = 2.0f + (rand() % 200) / 100.0f; /* Stay up for 2-4 seconds */
                        audio_play(AUD_BUMPS, 1.0f);
                    }
                }
            } else {
                /* Up */
                if (mole->p[1] < 1.0f) mole->p[1] += dt * 10.0f;

                /* Check collision with players */
                for (j = 0; j < player_count; j++) {
                    struct v_ball *player_ball = &pl->sim_state->uv[j];
                    float dist_vec[3];
                    v_sub(dist_vec, player_ball->p, mole->p);
                    if (v_len(dist_vec) < (player_ball->r + mole->r + 0.5f)) {
                        /* Whacked! */
                        pl->mole_grid[mi] = 0;
                        pl->mole_timers[mi] = 1.0f; /* Cooldown before popping up again */

                        /* Give points to player j */
                        players[j].coins += 10;
                        game_cmd_coins(j);

                        /* Bounce player slightly */
                        player_ball->v[1] += 5.0f;
                        audio_play(AUD_COIN, 1.0f);
                    }
                }

                if (pl->mole_timers[mi] <= 0.0f) {
                    pl->mole_grid[mi] = 0; /* Go back down */
                    pl->mole_timers[mi] = 1.0f;
                }
            }
        }
    }
}

static void game_hub_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct s_vary *vary = pl->sim_state;
    int i;

    /* Check Switches for Warp */
    for (i = 0; i < vary->xc; i++) {
        struct v_swch *xp = vary->xv + i;
        struct v_ball *b = &vary->uv[pl->ball_index];
        float d[3];
        v_sub(d, b->p, xp->base->p);

        /* Simple check radius */
        if (v_len(d) < (b->r + xp->base->r)) {
             pl->status = GAME_WARP;
             pl->warp_id = i;
             game_cmd_status(p);
             return;
        }
    }
}

static void game_grav_test(int p)
{
    struct server_player *pl = &players[p];
    struct s_vary *vary = pl->sim_state;
    struct v_ball *b = &vary->uv[pl->ball_index];
    int ji;

    /* Iterate all Jumps to find Gravity Zones */
    for (ji = 0; ji < vary->base->jc; ji++)
    {
        struct b_jump *jp = vary->base->jv + ji;
        float d, r[3];
        float ball_p[3];

        /* Transform ball position into jump space (assuming local space logic same as sol_jump_test) */
        sol_entity_local(ball_p, vary, vary->jv[ji].mi, vary->jv[ji].mj, b->p);

        r[0] = ball_p[0] - jp->p[0];
        r[1] = ball_p[2] - jp->p[2];
        r[2] = 0;

        d = v_len(r) + b->r - jp->r;

        /* Check collision */
        if (d <= b->r &&
            ball_p[1] > jp->p[1] &&
            ball_p[1] < jp->p[1] + JUMP_HEIGHT / 2)
        {
            /* Inside Trigger */

            /* Heuristic: If target vector magnitude is small, it's a gravity vector */
            float q_len = v_len(jp->q);
            if (q_len > 0.1f && q_len < 20.0f)
            {
                /* Apply Gravity */
                float new_g[3];
                v_nrm(new_g, jp->q);
                v_scl(new_g, new_g, -9.8f); /* Assuming standard gravity magnitude */

                v_cpy(pl->gravity_dir, new_g);

                /* Auto-Rotate Camera to Match Gravity (Wall Walking) */
                /* If gravity changes significantly, align view Up vector */
                /* Currently view.e[1] is Up. Gravity is Down (-Up). */
                /* We want view.e[1] ~= -gravity_dir. */
                /* Full 6DOF camera logic requires more math in game_update_view. */
                /* For now, just setting gravity affects physics (sol_step). */
            }
        }
    }
}

static int game_step(int p, const float g[3], float dt, int bt)
{
    if (p < 0 || p >= MAX_PLAYERS) return GAME_NONE;
    struct server_player *pl = &players[p];
    if (server_state)
    {
        float h[3];
        int i;

        /* Spin Dash */
        int dash = input_get_dash(p);

        if (dash)
        {
            /* Charging */
            if (pl->spin_charge < 1.0f)
            {
                pl->spin_charge += dt * 1.5f; /* Charge rate */
                if (pl->spin_charge > 1.0f) pl->spin_charge = 1.0f;

                game_cmd_dash_charge(p, pl->spin_charge);
            }
        }
        else
        {
            /* Release */
            if (pl->spin_charge > 0.0f)
            {
                /* Apply Impulse */
                struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
                float dash_force = 25.0f * pl->spin_charge;

                /* Direction: Camera Forward */
                float dir[3];
                v_cpy(dir, pl->view.e[2]);
                v_scl(dir, dir, -1.0f);

                /* Flatten Y to ensure horizontal dash */
                dir[1] = 0.0f;
                v_nrm(dir, dir);

                v_mad(b->v, b->v, dir, dash_force);

                audio_play(AUD_JUMP, 1.5f + pl->spin_charge);

                pl->spin_charge = 0.0f;
                game_cmd_dash_charge(p, 0.0f);
            }
        }

        /* Toggle Flight */
        int action = input_get_action(p);
        if (action && !pl->action_prev)
        {
            if (game_mode == MODE_TARGET)
            {
                if (!pl->fly_active && !pl->fly_done)
                {
                    pl->fly_active = 1;
                    pl->fly_pitch = 0.0f;
                    audio_play(AUD_JUMP, 1.0f); /* Feedback */
                }
                else if (pl->fly_active)
                {
                    pl->fly_active = 0;
                    pl->fly_done = 1; /* Locked */
                }
            }
            else if (game_mode == MODE_DOGFIGHT)
            {
                /* Fire Weapon */
                if (pl->ammo > 0) {
                    pl->ammo--;
                    audio_play(AUD_BUMPS, 1.0f);

                    /* Hitscan logic for Dogfight */
                    int j;
                    float fwd[3];
                    v_cpy(fwd, pl->view.e[2]);
                    v_scl(fwd, fwd, -1.0f);

                    for (j = 0; j < player_count; j++) {
                        if (j == p) continue;
                        struct v_ball *other = &pl->sim_state->uv[players[j].ball_index];
                        float vec[3];
                        v_sub(vec, other->p, pl->sim_state->uv[pl->ball_index].p);

                        float dist = v_len(vec);
                        v_nrm(vec, vec);

                        /* Forward cone hit detection */
                        if (dist < 50.0f && v_dot(fwd, vec) > 0.95f) {
                            /* Hit! Apply damage/knockback */
                            audio_play(AUD_BUMPL, 1.0f);
                            players[j].coins += 10; /* Use coins as damage taken */
                            game_cmd_coins(j); /* Update UI for victim */

                            /* Knockback */
                            float force[3];
                            v_cpy(force, fwd);
                            v_scl(force, force, 30.0f);
                            v_add(other->v, other->v, force);

                            /* If damage > 100, they die */
                            if (players[j].coins >= 100) {
                                players[j].coins = 0;
                                game_cmd_coins(j); /* Reset victim UI */
                                hud_show_toast("Target Destroyed!");
                                pl->coins += 50; /* Reward shooter */
                                game_cmd_coins(p);
                                game_respawn(j);
                            }
                            break; /* Only hit one per shot */
                        }
                    }
                } else {
                    audio_play(AUD_BUMPM, 0.5f); /* Click */
                }
            }
            else if (game_mode == MODE_NORMAL || game_mode == MODE_CHALLENGE || game_mode == MODE_BATTLE || game_mode == MODE_STANDALONE)
            {
                /* Active Jump */
                if (pl->can_jump)
                {
                    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
                    float jump_force = 12.0f * player_stats[p].jump;
                    b->v[1] += jump_force;
                    pl->can_jump = 0;
                    audio_play(AUD_JUMP, 1.0f);

                    if (!pl->is_cpu) profile_add_stat(STAT_JUMPS, 1);
                }
            }
        }

        if (game_mode == MODE_FIGHT)
        {
            game_fight_step(p, dt);
        }
        else if (game_mode == MODE_BILLIARDS)
        {
            game_billiards_step(p, dt);
        }
        else if (game_mode == MODE_BOWLING)
        {
            game_bowling_step(p, dt);
        }
        else if (game_mode == MODE_GOLF)
        {
            game_golf_step(p, dt);
        }
        else if (game_mode == MODE_BOAT)
        {
            game_boat_step(p, dt);
        }
        else if (game_mode == MODE_SHOT)
        {
            game_shot_step(p, dt);
        }
        else if (game_mode == MODE_SOCCER)
        {
            game_soccer_step(p, dt);
        }
        else if (game_mode == MODE_TENNIS)
        {
            game_tennis_step(p, dt);
        }
        else if (game_mode == MODE_BASEBALL)
        {
            game_baseball_step(p, dt);
        }
        else if (game_mode == MODE_HUB)
        {
            game_hub_step(p, dt);
        }
        else if (game_mode == MODE_MOLE)
        {
            game_mole_step(p, dt);
        }
        else if (game_mode == MODE_HAMMER)
        {
            game_hammer_step(p, dt);
        }

        if (game_mode == MODE_BATTLE || game_mode == MODE_TARGET) /* Allow items in Target? Sure. */
        {
            game_race_item_step(p, dt);
        }

        if (pl->sim_owner && game_mode != MODE_EDITOR)
        {
            game_grav_test(p);
        }

        pl->action_prev = action;

        /* Reload logic for Dogfight */
        if (game_mode == MODE_DOGFIGHT && pl->ammo < 20) {
            pl->reload_timer += dt;
            if (pl->reload_timer > 0.5f) {
                pl->ammo++;
                pl->reload_timer = 0.0f;
            }
        }

        if (pl->fly_active)
        {
            /* Flight Mode */
            game_fly_step(p, dt);

            /* Disable board tilt input, dampen existing tilt */
            pl->tilt.rx *= 0.9f;
            pl->tilt.rz *= 0.9f;
        }
        else if (game_mode == MODE_BILLIARDS || game_mode == MODE_BOWLING)
        {
            /* No board tilt */
            pl->tilt.rx = 0.0f;
            pl->tilt.rz = 0.0f;
        }
        else
        {
            /* Normal Tilt */
            /* Apply acceleration stat here: modify response time (s) or directly multiplier */
            float accel_mult = player_stats[p].acceleration;
            float response = input_get_s(p) / accel_mult; /* Higher accel = lower response time (faster) */

            pl->tilt.rx += (input_get_x(p) - pl->tilt.rx) * dt / MAX(dt, response);
            pl->tilt.rz += (input_get_z(p) - pl->tilt.rz) * dt / MAX(dt, response);
        }

        /* Ground Check for Jump */
        if (pl->sim_owner)
        {
            /* HACK: Allow jump if y-velocity is near zero. */
            /* Proper ground check would require access to collision manifold which is deep in solid_sim. */
            struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
            if (fabsf(b->v[1]) < 0.1f)
            {
                pl->can_jump = 1;
        game_cmd_jump_ready(p, 1);
            }
            else
            {
                pl->can_jump = 0;
        game_cmd_jump_ready(p, 0);
            }
        }

        /* Monkey Target Landing Logic */
        if (game_mode == MODE_TARGET && !pl->fly_active)
        {
             if (pl->fly_done || pl->time_elapsed > 3.0f)
             {
                 struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
                 float speed = v_len(b->v);

                 /* If speed is very low and we are not falling */
                 if (speed < 0.05f)
                 {
                     /* Landed */
                     float dist = sqrtf(b->p[0] * b->p[0] + b->p[2] * b->p[2]);
                     int points = 0;
                     int j;
                     int count = game_get_zone_count();
                     const struct target_zone *z = game_get_zones();

                     for (j = 0; j < count; j++) {
                         if (dist <= z[j].radius) {
                             points = z[j].score;
                             break;
                         }
                     }

                     if (pl->status == GAME_NONE)
                     {
                         pl->coins += points;
                         game_cmd_coins(p);
                         return GAME_GOAL;
                     }
                 }
             }
        }

        game_tilt_axes(&pl->tilt, pl->view.e);

        game_cmd_tiltaxes(p);
        game_cmd_tiltangles(p);

        grow_step(p, dt);

        if (pl->gyro_active)
        {
            pl->gyro_timer -= dt;
            if (pl->gyro_timer <= 0.0f)
            {
                pl->gyro_active = 0;
                game_cmd_gyro_state(p, 0, 0.0f);
            }
            else
            {
                /* Slow Fall Physics */
                struct v_ball *b = &pl->sim_state->uv[pl->ball_index];
                if (b->v[1] < -2.0f)
                {
                    b->v[1] += ((-2.0f - b->v[1]) * 2.0f * dt);
                }
                game_cmd_gyro_state(p, 1, pl->gyro_timer);
            }
        }

        if (pl->shock_active)
        {
            pl->shock_timer -= dt;
            if (pl->shock_timer <= 0.0f) pl->shock_active = 0;
        }

        if (pl->speed_active)
        {
            pl->speed_timer -= dt;
            if (pl->speed_timer <= 0.0f) pl->speed_active = 0;
            else {
                /* Boost speed limit/accel? */
                /* For now, just increase tilt angle slightly */
                pl->tilt.rx *= 1.5f;
                pl->tilt.rz *= 1.5f;
            }
        }

        if (game_mode == MODE_EDITOR) {
            /* Zero gravity in editor */
            v_zero(h);
        } else {
            game_tilt_grav(h, g, &pl->tilt);
        }

        if (pl->jump_b > 0)
        {
            pl->jump_dt += dt;

            if (pl->jump_dt >= 0.5f)
            {
                if (pl->jump_b == 1)
                {
                    float dp[3];

                    v_sub(dp, pl->jump_p, pl->sim_state->uv[pl->ball_index].p);
                    v_add(pl->view.p, pl->view.p, dp);

                    pl->jump_b = 2;
                }

                v_cpy(pl->sim_state->uv[pl->ball_index].p, pl->jump_p);
            }

            if (pl->jump_dt >= 1.0f)
                pl->jump_b = 0;
        }
        else
        {
        /* Track Distance */
        if (!pl->is_cpu)
        {
            float speed = v_len(pl->sim_state->uv[pl->ball_index].v);
            pl->dist_accumulator += speed * dt;
            if (pl->dist_accumulator >= 1.0f) {
                int d = (int)pl->dist_accumulator;
                profile_add_stat(STAT_DISTANCE, d);
                pl->dist_accumulator -= d;
            }
        }

            /* Run the sim */
            if (pl->sim_owner)
            {
                /* Sync ALL balls position to clients */
                if (pl->sim_owner) {
                    game_cmd_upd_all_balls(p);
                }

                for (i = 0; i < pl->sim_state->uc; i++)
                {
                    float b = sol_step(pl->sim_state, game_proxy_enq, h, dt, i, NULL);

                    if (b > 0.5f)
                    {
                        float k = (b - 0.5f) * 2.0f;
                        if      (pl->sim_state->uv[i].r > pl->sim_state->uv[i].sizes[1]) audio_play(AUD_BUMPL, k);
                        else if (pl->sim_state->uv[i].r < pl->sim_state->uv[i].sizes[1]) audio_play(AUD_BUMPS, k);
                        else                                                             audio_play(AUD_BUMPM, k);
                    }
                }
            }
        }

        /* We already called game_cmd_upd_all_balls if owner */
        game_cmd_updball(p);

        game_update_view(p, dt);
        game_update_time(p, dt, bt);

        return game_update_state(p, bt);
    }
    return GAME_NONE;
}

static void game_server_iter(float dt)
{
    int p;
    for (p = 0; p < player_count; p++)
    {
        if (p < 0 || p >= MAX_PLAYERS) continue;

        if (players[p].is_cpu)
            game_ai_step(p, dt);

        switch (players[p].status)
        {
        case GAME_GOAL: game_step(p, GRAVITY_UP, dt, 0); break;
        case GAME_FALL: game_step(p, GRAVITY_DN, dt, 0); break;

        case GAME_WARP:
            /* Halt processing for this player, waiting for client transition */
            break;

        case GAME_NONE:
            if ((players[p].status = game_step(p, players[p].gravity_dir, dt, 1)) != GAME_NONE)
                game_cmd_status(p);
            break;
        }
    }

    game_cmd_eou();
}

static struct lockstep server_step = { game_server_iter, DT };

void game_server_step(float dt)
{
    lockstep_run(&server_step, dt);
}

float game_server_blend(void)
{
    return lockstep_blend(&server_step);
}

/*---------------------------------------------------------------------------*/

void game_set_goal(int p)
{
    audio_play(AUD_SWITCH, 1.0f);
    if (p >= 0 && p < MAX_PLAYERS)
    {
        players[p].goal_e = 1;
        game_cmd_goalopen(p);
    }
}

void game_respawn(int p)
{
    struct server_player *pl = &players[p];
    if (p >= 0 && p < MAX_PLAYERS)
    {
        v_cpy(pl->sim_state->uv[pl->ball_index].p, pl->start_p);
        v_scl(pl->sim_state->uv[pl->ball_index].v, pl->sim_state->uv[pl->ball_index].v, 0.0f);
        v_scl(pl->sim_state->uv[pl->ball_index].w, pl->sim_state->uv[pl->ball_index].w, 0.0f);

        pl->status = GAME_NONE;

        /* Reset flight state */
        pl->fly_active = 0;
        pl->fly_done = 0;
        pl->fly_pitch = 0.0f;

        pl->punch_state = 0;
        pl->punch_timer = 0.0f;

        pl->shot_power = 0.0f;

        game_view_fly(&pl->view, pl->sim_state, 0.0f);

        game_cmd_status(p);
        game_cmd_updball(p);
        game_cmd_updview(p);
    }
}

/*---------------------------------------------------------------------------*/

void game_set_x(float k, int p)
{
    input_set_x(p, -get_angle_bound(p) * k);
    input_set_s(p, config_get_d(CONFIG_JOYSTICK_RESPONSE) * 0.001f);
}

void game_set_z(float k, int p)
{
    input_set_z(p, +get_angle_bound(p) * k);
    input_set_s(p, config_get_d(CONFIG_JOYSTICK_RESPONSE) * 0.001f);
}

void game_set_ang(float x, float z, int p)
{
    input_set_x(p, x);
    input_set_z(p, z);
}

void game_set_pos(int x, int y, int p)
{
    const float range = ANGLE_BOUND * 2;

    input_set_x(p, input_get_x(p) + range * y / config_get_d(CONFIG_MOUSE_SENSE));
    input_set_z(p, input_get_z(p) + range * x / config_get_d(CONFIG_MOUSE_SENSE));

    input_set_s(p, config_get_d(CONFIG_MOUSE_RESPONSE) * 0.001f);
}

void game_set_cam(int c, int p)
{
    input_set_c(p, c);
}

void game_set_rot(float r, int p)
{
    input_set_r(p, r);
}

void game_set_action(int a, int p)
{
    input_set_action(p, a);
}

void game_set_dash(int d, int p)
{
    input_set_dash(p, d);
}

/*---------------------------------------------------------------------------*/

float curr_time_elapsed(int p)
{
    if (p >= 0 && p < MAX_PLAYERS)
        return players[p].time_elapsed;
    return 0.0f;
}

float curr_speed(int p)
{
    if (p >= 0 && p < MAX_PLAYERS)
    {
        struct v_ball *b = &players[p].sim_state->uv[players[p].ball_index];
        return v_len(b->v);
    }
    return 0.0f;
}

float curr_altitude(int p)
{
    if (p >= 0 && p < MAX_PLAYERS)
    {
        struct v_ball *b = &players[p].sim_state->uv[players[p].ball_index];
        return b->p[1];
    }
    return 0.0f;
}

/*---------------------------------------------------------------------------*/

void game_ai_step(int p, float dt)
{
    struct server_player *pl = &players[p];
    struct v_ball *b = &pl->sim_state->uv[pl->ball_index];

    /* Reset Inputs */
    input_set_x(p, 0.0f);
    input_set_z(p, 0.0f);
    input_set_action(p, 0);

    /* Very Basic Steering */
    /* Seek Center (0,0) by default to stay on board */
    float target[3] = {0.0f, 0.0f, 0.0f};

    /* If Target/Race Mode, seek next waypoint/goal */
    if (game_mode == MODE_BATTLE || game_mode == MODE_TARGET) {
        if (pl->sim_state->base->zc > 0) {
            v_cpy(target, pl->sim_state->base->zv[0].p);
        }
    }

    if (game_mode == MODE_SOCCER || game_mode == MODE_TENNIS) {
        /* Seek Ball (Index 0) */
        v_cpy(target, pl->sim_state->uv[0].p);

        if (game_mode == MODE_TENNIS) {
            float diff[3];
            v_sub(diff, target, b->p);
            float dist = v_len(diff);
            if (dist < 2.0f) {
                input_set_action(p, 1);
            }
        }
    }

    /* If Fight Mode, seek nearest opponent */
    if (game_mode == MODE_FIGHT) {
        int i;
        float best_dist = 1000.0f;
        int target_idx = -1;

        for (i=0; i<player_count; i++) {
            if (i==p) continue;
            struct v_ball *other = &pl->sim_state->uv[players[i].ball_index];
            float d[3];
            v_sub(d, other->p, b->p);
            float dist = v_len(d);
            if (dist < best_dist) {
                best_dist = dist;
                target_idx = i;
            }
        }

        if (target_idx != -1) {
            v_cpy(target, pl->sim_state->uv[players[target_idx].ball_index].p);

            /* Attack logic */
            if (best_dist < 5.0f) {
                input_set_action(p, 1);
            }
        }
    }

    /* Calculate Steering */
    /* Desired vector from self to target */
    float desired[3];
    v_sub(desired, target, b->p);

    /* Transform to Camera Space (View Basis) */
    /* AI "Camera" usually faces velocity or target */
    /* Let's assume input x/z are relative to world X/Z for simplicity? */
    /* No, game_step uses game_tilt_grav which uses tilt.x/z derived from view. */
    /* We need to set input_x/z such that they produce world tilt towards target. */

    /* Transform desired (world) into View Space */
    /* dx = dot(desired, view.e[0]) */
    /* dz = dot(desired, view.e[2]) */

    float dx = v_dot(desired, pl->view.e[0]);
    float dz = v_dot(desired, pl->view.e[2]);

    /* Normalize */
    float len = sqrtf(dx*dx + dz*dz);
    if (len > 0.01f) {
        dx /= len;
        dz /= len;

        /* Apply to Input (Tilt) */
        /* Tilt X is Left/Right (e[0]) */
        /* Tilt Z is Fwd/Back (e[2]) */

        /* Neverball Inputs: */
        /* +Z input tilts forward (ball rolls forward -Z in view space?) */
        /* Actually: +Z input -> Tilt Rz + -> Rotates world around Z axis? */
        /* Let's look at game_step: tilt.rx += input_x... */
        /* game_tilt_grav rotates G by Rz then Rx. */

        /* Let's just use simple feedback. */
        /* If target is to the right (+X view), tilt board right (-Z world? No). */

        /* Mapping: */
        /* Input Z+ -> Tilt Rz+ -> Rolls View -X? */
        /* Let's assume standard controls: Stick Up (Z+) moves ball Fwd (View -Z??) */

        /* Trial and error or copy from player code: */
        /* player input z maps to tilt.rz. */
        /* player input x maps to tilt.rx. */

        /* We want to roll towards 'desired'. */
        /* To roll along view X (Right), we need to tilt around view Z axis? No. */

        /* Simplified: */
        /* Input Z controls movement along View Z. */
        /* Input X controls movement along View X. */

        input_set_x(p, dx * ANGLE_BOUND);
        input_set_z(p, dz * ANGLE_BOUND);
    }
}

int curr_warp_id(int p)
{
    if (p >= 0 && p < MAX_PLAYERS)
        return players[p].warp_id;
    return -1;
}

void game_ai_step(int p, float dt);

void game_set_gravity(int p, float x, float y, float z)
{
    if (p >= 0 && p < MAX_PLAYERS)
    {
        players[p].gravity_dir[0] = x;
        players[p].gravity_dir[1] = y;
        players[p].gravity_dir[2] = z;
    }
}

/*---------------------------------------------------------------------------*/
