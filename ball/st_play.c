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

#include "gui.h"
#include "transition.h"
#include "hud.h"
#include "demo.h"
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "video.h"
#include "cmd.h"
<<<<<<< HEAD
#include "key.h"
#include "set.h"
=======
>>>>>>> origin/csy-extras

#include "game_common.h"
#include "game_server.h"
#include "game_proxy.h"
#include "game_client.h"

#include "st_play.h"
#include "st_goal.h"
#include "st_fail.h"
#include "st_pause.h"
#include "st_level.h"
#include "st_shared.h"

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
static void set_camera(int c)
{
    config_set_d(CONFIG_CAMERA, c);
    hud_cam_pulse(c);
}

static void toggle_camera(void)
{
    int cam = (config_tst_d(CONFIG_CAMERA, CAM_3) ?
               CAM_1 : CAM_3);

    set_camera(cam);
}

static void next_camera(void)
{
    int cam = config_get_d(CONFIG_CAMERA) + 1;

    if (cam <= CAM_NONE || cam >= CAM_MAX)
        cam = CAM_1;

    set_camera(cam);
}

static void keybd_camera(int c)
{
    if (config_tst_d(CONFIG_KEY_CAMERA_1, c))
        set_camera(CAM_1);
    if (config_tst_d(CONFIG_KEY_CAMERA_2, c))
        set_camera(CAM_2);
    if (config_tst_d(CONFIG_KEY_CAMERA_3, c))
        set_camera(CAM_3);

    if (config_tst_d(CONFIG_KEY_CAMERA_TOGGLE, c))
        toggle_camera();
}

static void click_camera(int b)
{
    if (config_tst_d(CONFIG_MOUSE_CAMERA_1, b))
        set_camera(CAM_1);
    if (config_tst_d(CONFIG_MOUSE_CAMERA_2, b))
        set_camera(CAM_2);
    if (config_tst_d(CONFIG_MOUSE_CAMERA_3, b))
        set_camera(CAM_3);

    if (config_tst_d(CONFIG_MOUSE_CAMERA_TOGGLE, b))
        toggle_camera();
}

static void buttn_camera(int b)
{
    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_X, b))
        next_camera();
=======
static int pause_or_exit(void)
{
    if (config_tst_d(CONFIG_KEY_PAUSE, SDLK_ESCAPE))
    {
        return goto_state(&st_pause);
    }
    else
    {
        progress_stat(GAME_NONE);
        progress_stop();

        video_clr_grab();

        return goto_state(&st_exit);
    }
>>>>>>> origin/csy-extras
}

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
static const float time_in   = 0.5f;
static const float time_out  = 0.6f;
static const int   flags_in  = GUI_E | GUI_FLING | GUI_EASE_BACK;
static const int   flags_out = GUI_W | GUI_FLING | GUI_EASE_BACK | GUI_BACKWARD;
=======
static void set_camera(int c)
{
    config_set_d(CONFIG_CAMERA, c);
    hud_view_pulse(c);
}

static void toggle_camera(void)
{
    int cam = (config_tst_d(CONFIG_CAMERA, VIEW_MANUAL) ?
               VIEW_CHASE : VIEW_MANUAL);

    set_camera(cam);
}

static void keybd_camera(int c)
{
    if (config_tst_d(CONFIG_KEY_CAMERA_1, c))
        set_camera(VIEW_CHASE);
    if (config_tst_d(CONFIG_KEY_CAMERA_2, c))
        set_camera(VIEW_LAZY);
    if (config_tst_d(CONFIG_KEY_CAMERA_3, c))
        set_camera(VIEW_MANUAL);

    if (config_tst_d(CONFIG_KEY_CAMERA_TOGGLE, c))
        toggle_camera();

    if (config_cheat() && c == SDLK_F4)
        set_camera(config_tst_d(CONFIG_CAMERA, VIEW_TEST1) ?
                   VIEW_TEST2 : VIEW_TEST1);
}

static void click_camera(int b)
{
    if (config_tst_d(CONFIG_MOUSE_CAMERA_1, b))
        set_camera(VIEW_CHASE);
    if (config_tst_d(CONFIG_MOUSE_CAMERA_2, b))
        set_camera(VIEW_LAZY);
    if (config_tst_d(CONFIG_MOUSE_CAMERA_3, b))
        set_camera(VIEW_MANUAL);

    if (config_tst_d(CONFIG_MOUSE_CAMERA_TOGGLE, b))
        toggle_camera();
}

static void buttn_camera(int b)
{
    if (config_tst_d(CONFIG_JOYSTICK_CAMERA_1, b))
        set_camera(VIEW_CHASE);
    if (config_tst_d(CONFIG_JOYSTICK_CAMERA_2, b))
        set_camera(VIEW_LAZY);
    if (config_tst_d(CONFIG_JOYSTICK_CAMERA_3, b))
        set_camera(VIEW_MANUAL);

    if (config_tst_d(CONFIG_JOYSTICK_CAMERA_TOGGLE, b))
        toggle_camera();
}
>>>>>>> origin/csy-extras

/*---------------------------------------------------------------------------*/

static int play_ready_gui(void)
{
    int id;

    if ((id = gui_label(0, _("Ready?"), GUI_LRG, 0, 0)))
    {
        gui_layout(id, 0, 0);
        gui_pulse(id, 1.2f);
    }

    return id;
}

<<<<<<< HEAD
static int play_ready_enter(struct state *st, struct state *prev, int intent)
{
    int id;

    audio_play(AUD_READY, 1.0f);
    video_set_grab(1);

    hud_cam_pulse(config_get_d(CONFIG_CAMERA));

    id = play_ready_gui();
    gui_slide(id, flags_in, 0, time_in, 0);
    return id;
}

static int play_ready_leave(struct state *st, struct state *next, int id, int intent)
{
    gui_slide(id, flags_out | GUI_REMOVE, 0, time_out, 0);
    transition_add(id);
    return id;
=======
static int play_ready_enter(struct state *st, struct state *prev)
{
    audio_play(AUD_READY, 1.0f);
    video_set_grab(1);

    hud_view_pulse(config_get_d(CONFIG_CAMERA));

    return play_ready_gui();
>>>>>>> origin/csy-extras
}

static void play_ready_paint(int id, float t)
{
    game_client_draw(0, t);
<<<<<<< HEAD
    hud_cam_paint();
=======
    hud_view_paint();
>>>>>>> origin/csy-extras
    gui_paint(id);
}

static void play_ready_timer(int id, float dt)
{
    float t = time_state();

    game_client_fly(1.0f - 0.5f * t);

    if (dt > 0.0f && t > 1.0f)
        goto_state(&st_play_set);

    game_step_fade(dt);
<<<<<<< HEAD
    hud_cam_timer(dt);
=======
    hud_view_timer(dt);
>>>>>>> origin/csy-extras
    gui_timer(id, dt);
}

static int play_ready_click(int b, int d)
{
    if (d)
    {
        click_camera(b);

        if (b == SDL_BUTTON_LEFT)
            goto_state(&st_play_loop);
    }
    return 1;
}

static int play_ready_keybd(int c, int d)
{
    if (d)
    {
        keybd_camera(c);

<<<<<<< HEAD
        if (c == KEY_EXIT)
=======
        if (config_tst_d(CONFIG_KEY_PAUSE, c))
>>>>>>> origin/csy-extras
            goto_state(&st_pause);
    }
    return 1;
}

static int play_ready_buttn(int b, int d, int device_id)
{
    if (d)
    {
        buttn_camera(b);

        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return goto_state(&st_play_loop);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_START, b))
            return goto_state(&st_pause);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int play_set_gui(void)
{
    int id;

    if ((id = gui_label(0, _("Set?"), GUI_LRG, 0, 0)))
    {
        gui_layout(id, 0, 0);
        gui_pulse(id, 1.2f);
    }

    return id;
}

<<<<<<< HEAD
static int play_set_enter(struct state *st, struct state *prev, int intent)
{
    int id;

    audio_play(AUD_SET, 1.f);

    id = play_set_gui();
    gui_slide(id, flags_in, 0, time_in, 0);
    return id;
=======
static int play_set_enter(struct state *st, struct state *prev)
{
    audio_play(AUD_SET, 1.f);

    return play_set_gui();
}

static void play_set_paint(int id, float t)
{
    game_client_draw(0, t);
    hud_view_paint();
    gui_paint(id);
>>>>>>> origin/csy-extras
}

static int play_set_leave(struct state *st, struct state *next, int id, int intent)
{
    gui_slide(id, flags_out | GUI_REMOVE, 0, time_out, 0);
    transition_add(id);
    return id;
}

static void play_set_paint(int id, float t)
{
    game_client_draw(0, t);
    hud_cam_paint();
    gui_paint(id);
}

static void play_set_timer(int id, float dt)
{
    float t = time_state();

    game_client_fly(0.5f - 0.5f * t);

    if (dt > 0.0f && t > 1.0f)
        goto_state(&st_play_loop);

    game_step_fade(dt);
<<<<<<< HEAD
    hud_cam_timer(dt);
=======
    hud_view_timer(dt);
>>>>>>> origin/csy-extras
    gui_timer(id, dt);
}

static int play_set_click(int b, int d)
{
    if (d)
    {
        click_camera(b);

        if (b == SDL_BUTTON_LEFT)
            goto_state(&st_play_loop);
    }
    return 1;
}

static int play_set_keybd(int c, int d)
{
    if (d)
    {
        keybd_camera(c);

<<<<<<< HEAD
        if (c == KEY_EXIT)
=======
        if (config_tst_d(CONFIG_KEY_PAUSE, c))
>>>>>>> origin/csy-extras
            goto_state(&st_pause);
    }
    return 1;
}

static int play_set_buttn(int b, int d, int device_id)
{
    if (d)
    {
        buttn_camera(b);

        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return goto_state(&st_play_loop);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_START, b))
            return goto_state(&st_pause);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
enum
{
    ROT_NONE = 0,
    ROT_ROTATE,
    ROT_HOLD
};

#define DIR_R 0x1
#define DIR_L 0x2

static int   rot_dir;
static float rot_val;

static void rot_init(void)
{
    rot_val = 0.0f;
    rot_dir = 0;
}

static void rot_set(int dir, float value, int exclusive)
{
    rot_val = value;

    if (exclusive)
        rot_dir  = dir;
    else
        rot_dir |= dir;
}

static void rot_clr(int dir)
{
    rot_dir &= ~dir;
}

static int rot_get(float *v)
{
    *v = 0.0f;

    if ((rot_dir & DIR_R) && (rot_dir & DIR_L))
    {
        return ROT_HOLD;
    }
    else if (rot_dir & DIR_R)
    {
        *v = +rot_val;
        return ROT_ROTATE;
    }
    else if (rot_dir & DIR_L)
    {
        *v = -rot_val;
        return ROT_ROTATE;
    }
    return ROT_NONE;
}

/*---------------------------------------------------------------------------*/

static int fast_rotate;
static int show_hud;
static int loop_transition;

static float respawn_timer[MAX_PLAYERS];
static int last_status[MAX_PLAYERS];
static int finished_mask = 0;
=======
struct
{
    float R;
    float L;

    enum
    {
        DIR_R = 1,
        DIR_L
    } d;
} view_rotate;

#define VIEWR_SET_R(v) do {                                    \
    view_rotate.R = (v);                                       \
    view_rotate.d = (v) ? DIR_R : (view_rotate.L ? DIR_L : 0); \
} while (0)

#define VIEWR_SET_L(v) do {                                    \
    view_rotate.L = (v);                                       \
    view_rotate.d = (v) ? DIR_L : (view_rotate.R ? DIR_R : 0); \
} while (0)

static int fast_rotate;
static int show_hud;
>>>>>>> origin/csy-extras

static int play_loop_gui(void)
{
    int id;

    if ((id = gui_label(0, _("GO!"), GUI_LRG, gui_blu, gui_grn)))
    {
        gui_layout(id, 0, 0);
        gui_pulse(id, 1.2f);
    }

    return id;
}

<<<<<<< HEAD
static int play_loop_enter(struct state *st, struct state *prev, int intent)
{
    int id;
    int i;

    rot_init();
    fast_rotate = 0;
=======
static int play_loop_enter(struct state *st, struct state *prev)
{
    VIEWR_SET_R(0);
    VIEWR_SET_L(0);
    fast_rotate = 0;

    if (prev == &st_pause)
        return 0;

    audio_play(AUD_GO, 1.f);

    game_client_fly(0.0f);

    show_hud = 1;
    hud_update(0);

    return play_loop_gui();
}

static void play_loop_paint(int id, float t)
{
    game_client_draw(0, t);

    if (show_hud)
        hud_paint();
>>>>>>> origin/csy-extras

    for (i = 0; i < MAX_PLAYERS; i++)
    {
        respawn_timer[i] = 0.0f;
        last_status[i] = GAME_NONE;
    }
    finished_mask = 0;

    if (prev == &st_pause)
    {
        hud_show(0.0f);
        return 0;
    }

    audio_play(AUD_GO, 1.f);

    game_client_fly(0.0f);

    show_hud = 1;
    hud_update(0, 0);
    hud_show(0.0f);
    loop_transition = 0;

    id = play_loop_gui();
    gui_slide(id, flags_in, 0, time_in, 0);
    return id;
}

static int play_loop_leave(struct state *st, struct state *next, int id, int intent)
{
    hud_hide();
    gui_delete(id);
    return 0;
}

static void play_loop_paint(int id, float t)
{
    game_client_draw(0, t);

    if (show_hud)
        hud_paint(0, 0, video.device_w, video.device_h);

    gui_paint(id);
}

static void play_loop_timer(int id, float dt)
{
    float k = (fast_rotate ?
               (float) config_get_d(CONFIG_ROTATE_FAST) / 100.0f :
               (float) config_get_d(CONFIG_ROTATE_SLOW) / 100.0f);

<<<<<<< HEAD
    float r = 0.0f;
    int p;
    int count = config_get_d(CONFIG_MULTIBALL);
    if (count < 1) count = 1;
    if (count > MAX_PLAYERS) count = MAX_PLAYERS;

    gui_timer(id, dt);
    hud_timer(dt);

    if (time_state() >= 1.0f && !loop_transition)
    {
        gui_slide(id, flags_out, 0, time_out, 0);
        loop_transition = 1;
    }

    switch (rot_get(&r))
    {
    case ROT_HOLD:
        game_set_rot(0.0f, 0);
        game_set_cam(CAM_3, 0);
        break;

    case ROT_ROTATE:
    case ROT_NONE:
        game_set_rot(r * k, 0);
        game_set_cam(config_get_d(CONFIG_CAMERA), 0);
        break;
    }
=======
    gui_timer(id, dt);
    hud_timer(dt);
    game_set_rot(view_rotate.d == DIR_R ?
                 view_rotate.R * k :
                 view_rotate.L * k);
    game_set_cam(config_get_d(CONFIG_CAMERA));
>>>>>>> origin/csy-extras

    game_step_fade(dt);

    game_server_step(dt);
    game_client_sync(demo_fp);
    game_client_blend(game_server_blend());

<<<<<<< HEAD
    for (p = 0; p < count; p++)
    {
        int status = curr_status(p);

        if (status != last_status[p])
        {
            if (status != GAME_NONE)
                progress_stat(status, p);
            last_status[p] = status;
        }

        switch (status)
        {
        case GAME_GOAL:
            if (!(finished_mask & (1 << p)))
            {
                progress_stat(GAME_GOAL, p);
                finished_mask |= (1 << p);
            }
            break;

        case GAME_WARP:
            /* Transition to next level immediately */
            if (!(finished_mask & (1 << p)))
            {
                 int next_level_ready = 0;

                 /* Mark as done */
                 finished_mask |= (1 << p);
                 progress_stat(GAME_GOAL, p);

                 /* Determine Target Level */
                 if (curr_mode() == MODE_HUB)
                 {
                     /* Map Switch Index (warp_id) to Level Index */
                     /* MVP: Hub Switch X -> Level X+1 */
                     /* This assumes standard set ordering. */
                     int warp_id = curr_warp_id(p);
                     /* Note: get_level() takes absolute index in current set */

                     /* We want to warp to (warp_id + 1).
                        Wait, usually Hub is level 0.
                        So Switch 0 -> Level 1.
                        Switch 1 -> Level 2.
                        So index = warp_id + 1. */

                     struct level *target = get_level(warp_id + 1);
                     if (target) {
                         progress_play(target);
                         next_level_ready = 1;
                     }
                 }

                 if (!next_level_ready)
                 {
                     /* Fallback: Linear Progress */
                     if (progress_next()) {
                         next_level_ready = 1;
                     }
                 }

                 if (next_level_ready) {
                     goto_state(&st_level);
                 } else {
                     goto_state(&st_goal);
                 }

                 return;
            }
            break;

        case GAME_FALL:
            respawn_timer[p] += dt;
            if (respawn_timer[p] > 1.5f)
            {
                game_respawn(p);
                respawn_timer[p] = 0.0f;
                last_status[p] = GAME_NONE;
            }
            break;

        case GAME_TIME:
            if (!(finished_mask & (1 << p)))
            {
                progress_stat(GAME_TIME, p);
                finished_mask |= (1 << p);
            }
            break;
        }
    }

    progress_step();

    {
        int finished_count = 0;
        for (p = 0; p < count; p++)
            if (finished_mask & (1 << p)) finished_count++;

        if (finished_count == count)
            goto_state(&st_goal);
=======
    switch (curr_status())
    {
    case GAME_GOAL:
        progress_stat(GAME_GOAL);
        goto_state(&st_goal);
        break;

    case GAME_FALL:
        progress_stat(GAME_FALL);
        goto_state(&st_fail);
        break;

    case GAME_TIME:
        progress_stat(GAME_TIME);
        goto_state(&st_fail);
        break;

    default:
        progress_step();
        break;
>>>>>>> origin/csy-extras
    }
}

static void play_loop_point(int id, int x, int y, int dx, int dy)
{
    game_set_pos(dx, dy, 0);
}

<<<<<<< HEAD
static void play_loop_stick(int id, int a, float v, int bump, int device_id)
{
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X0, a))
        game_set_z(v, device_id);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y0, a))
        game_set_x(v, device_id);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X1, a))
    {
        if      (v > 0.0f)
            rot_set(DIR_R, +v, 1);
        else if (v < 0.0f)
            rot_set(DIR_L, -v, 1);
        else
            rot_clr(DIR_R | DIR_L);
=======
static void play_loop_stick(int id, int a, float v, int bump)
{
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X, a))
        game_set_z(v);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y, a))
        game_set_x(v);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_U, a))
    {
        VIEWR_SET_R(0);
        VIEWR_SET_L(0);

        if (v > 0) VIEWR_SET_R(v);
        if (v < 0) VIEWR_SET_L(v);
>>>>>>> origin/csy-extras
    }
}

static int play_loop_click(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_MOUSE_CAMERA_R, b))
<<<<<<< HEAD
            rot_set(DIR_R, 1.0f, 0);
        if (config_tst_d(CONFIG_MOUSE_CAMERA_L, b))
            rot_set(DIR_L, 1.0f, 0);
=======
            VIEWR_SET_R(+1);
        if (config_tst_d(CONFIG_MOUSE_CAMERA_L, b))
            VIEWR_SET_L(-1);
>>>>>>> origin/csy-extras

        click_camera(b);
    }
    else
    {
        if (config_tst_d(CONFIG_MOUSE_CAMERA_R, b))
<<<<<<< HEAD
            rot_clr(DIR_R);
        if (config_tst_d(CONFIG_MOUSE_CAMERA_L, b))
            rot_clr(DIR_L);
=======
            VIEWR_SET_R(0);
        if (config_tst_d(CONFIG_MOUSE_CAMERA_L, b))
            VIEWR_SET_L(0);
>>>>>>> origin/csy-extras
    }

    return 1;
}

static int play_loop_keybd(int c, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_KEY_CAMERA_R, c))
<<<<<<< HEAD
            rot_set(DIR_R, 1.0f, 0);
        if (config_tst_d(CONFIG_KEY_CAMERA_L, c))
            rot_set(DIR_L, 1.0f, 0);
=======
            VIEWR_SET_R(+1);
        if (config_tst_d(CONFIG_KEY_CAMERA_L, c))
            VIEWR_SET_L(-1);
>>>>>>> origin/csy-extras
        if (config_tst_d(CONFIG_KEY_ROTATE_FAST, c))
            fast_rotate = 1;

        keybd_camera(c);

        if (config_tst_d(CONFIG_KEY_RESTART, c) &&
            progress_same_avail())
        {
            if (progress_same())
                goto_state(&st_play_ready);
        }
<<<<<<< HEAD
        if (c == KEY_EXIT)
=======
        if (config_tst_d(CONFIG_KEY_PAUSE, c))
>>>>>>> origin/csy-extras
            goto_state(&st_pause);
    }
    else
    {
        if (config_tst_d(CONFIG_KEY_CAMERA_R, c))
<<<<<<< HEAD
            rot_clr(DIR_R);
        if (config_tst_d(CONFIG_KEY_CAMERA_L, c))
            rot_clr(DIR_L);
=======
            VIEWR_SET_R(0);
        if (config_tst_d(CONFIG_KEY_CAMERA_L, c))
            VIEWR_SET_L(0);
>>>>>>> origin/csy-extras
        if (config_tst_d(CONFIG_KEY_ROTATE_FAST, c))
            fast_rotate = 0;
    }

    if (d && c == KEY_LOOKAROUND && config_cheat())
        return goto_state(&st_look);

<<<<<<< HEAD
    if (d && c == KEY_POSE)
=======
    if (d && c == SDLK_F6)
>>>>>>> origin/csy-extras
        show_hud = !show_hud;

    if (d && c == SDLK_c && config_cheat())
    {
<<<<<<< HEAD
        progress_stat(GAME_GOAL, 0);
=======
        progress_stat(GAME_GOAL);
>>>>>>> origin/csy-extras
        return goto_state(&st_goal);
    }
    return 1;
}

static int play_loop_buttn(int b, int d, int device_id)
{
    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        game_set_action(d, device_id);

    if (d == 1)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_START, b))
            goto_state(&st_pause);

<<<<<<< HEAD
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_R1, b))
            rot_set(DIR_R, 1.0f, 0);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L1, b))
            rot_set(DIR_L, 1.0f, 0);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L2, b))
=======
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_R, b))
            VIEWR_SET_R(+1);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L, b))
            VIEWR_SET_L(-1);
        if (config_tst_d(CONFIG_JOYSTICK_ROTATE_FAST, b))
>>>>>>> origin/csy-extras
            fast_rotate = 1;

        buttn_camera(b);
    }
    else
    {
<<<<<<< HEAD
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_R1, b))
            rot_clr(DIR_R);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L1, b))
            rot_clr(DIR_L);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L2, b))
=======
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_R, b))
            VIEWR_SET_R(0);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L, b))
            VIEWR_SET_L(0);
        if (config_tst_d(CONFIG_JOYSTICK_ROTATE_FAST, b))
>>>>>>> origin/csy-extras
            fast_rotate = 0;
    }
    return 1;
}

static int play_loop_touch(const SDL_TouchFingerEvent *event)
{
    static SDL_FingerID rotate_finger = -1;

    static float rotate = 0.0f; /* Filtered input. */

    /*
     * Make sure not to exceed rotate_fast rotation speed.
     *
     * Find the coefficient needed to reach rotate_fast speed and
     * clamp the scaled input at that value.
     *
     * Derivation:
     * Default rotate_slow is 150 or 1.5
     * Default rotate_fast is 300 or 3.0
     *
     *   x * slow = 1.0 * fast
     *   x = (1.0 * fast) / slow
     *   x = fast / slow
     */
    const float rs = config_get_d(CONFIG_ROTATE_SLOW) / 100.0f;
    const float rf = config_get_d(CONFIG_ROTATE_FAST) / 100.0f;
    const float rmax = rf / rs;

    int id;

    if ((id = hud_touch(event)))
    {
        int token = gui_token(id);

        gui_pulse(id, 1.2f);

        if (token == GUI_BACK)
        {
            exit_state(&st_pause);
        }
        else if (token == GUI_CAMERA)
        {
            toggle_camera();

            /* Weird hack: allow toggling the button. */

            gui_focus(0);
        }
    }
    else if (event->type == SDL_FINGERDOWN)
    {
        SDL_Finger *finger = SDL_GetTouchFinger(event->touchId, 1); /* Second finger. */

        if (finger && event->fingerId == finger->id)
        {
            rotate_finger = finger->id;
            rotate = 0.0f;
        }
    }
    else if (event->type == SDL_FINGERUP)
    {
        if (event->fingerId == rotate_finger)
        {
            rotate_finger = -1;
            rot_clr(DIR_R | DIR_L);
            rotate = 0.0f;
        }
    }
    else if (event->type == SDL_FINGERMOTION)
    {
        if (event->fingerId == rotate_finger)
        {
            /* Discard accumulated input when moving in the opposite direction. */

            if ((rotate < 0.0f && event->dx > 0.0f) || (event->dx < 0.0f && rotate > 0.0f))
                rotate = 0.0f;

            /* Filter the input for a smoother experience. */

            rotate += event->dx * 0.6f;

            /*
             * touch_rotate gives the fraction of the screen that you need to swipe
             * across to reach rotate_slow rotation speed. E.g., a value of 32
             * is 1/32 of screen.
             *
             * To rotate slower, swipe a smaller distance than that.
             * To rotate faster, swipe farther.
             */

            if (rotate != 0.0f)
            {
                const float scaled_rotate = (float) config_get_d(CONFIG_TOUCH_ROTATE) * rotate;
                rot_set(DIR_L, CLAMP(-rmax, scaled_rotate, +rmax), 1);
            }
        }
        else
        {
            int dx = (int) ((float) video.device_w * event->dx);
            int dy = (int) ((float) video.device_h * -event->dy);

            game_set_pos(dx, dy, 0);
        }
    }

    return 1;
}

/*---------------------------------------------------------------------------*/

static float phi;
static float theta;

<<<<<<< HEAD
static int look_enter(struct state *st, struct state *prev, int intent)
=======
static int look_enter(struct state *st, struct state *prev)
>>>>>>> origin/csy-extras
{
    phi   = 0;
    theta = 0;
    return 0;
}

<<<<<<< HEAD
static int look_leave(struct state *st, struct state *next, int id, int intent)
=======
static void look_leave(struct state *st, struct state *next, int id)
>>>>>>> origin/csy-extras
{
    return 0;
}

static void look_paint(int id, float t)
{
    game_client_draw(0, t);
}

static void look_point(int id, int x, int y, int dx, int dy)
{
    phi   +=  90.0f * dy / video.device_h;
    theta += 180.0f * dx / video.device_w;

    if (phi > +90.0f) phi = +90.0f;
    if (phi < -90.0f) phi = -90.0f;

    if (theta > +180.0f) theta -= 360.0f;
    if (theta < -180.0f) theta += 360.0f;

    game_look(phi, theta);
}

static int look_keybd(int c, int d)
{
    if (d)
    {
        if (c == KEY_EXIT || c == KEY_LOOKAROUND)
            return goto_state(&st_play_loop);
    }

    return 1;
}

static int look_buttn(int b, int d, int device_id)
{
    if (d && (config_tst_d(CONFIG_JOYSTICK_BUTTON_START, b)))
        return goto_state(&st_play_loop);

    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_play_ready = {
    play_ready_enter,
<<<<<<< HEAD
    play_ready_leave,
=======
    shared_leave,
>>>>>>> origin/csy-extras
    play_ready_paint,
    play_ready_timer,
    NULL,
    NULL,
    NULL,
    play_ready_click,
    play_ready_keybd,
    play_ready_buttn
};

struct state st_play_set = {
    play_set_enter,
<<<<<<< HEAD
    play_set_leave,
=======
    shared_leave,
>>>>>>> origin/csy-extras
    play_set_paint,
    play_set_timer,
    NULL,
    NULL,
    NULL,
    play_set_click,
    play_set_keybd,
    play_set_buttn
};

struct state st_play_loop = {
    play_loop_enter,
    play_loop_leave,
    play_loop_paint,
    play_loop_timer,
    play_loop_point,
    play_loop_stick,
    shared_angle,
    play_loop_click,
    play_loop_keybd,
<<<<<<< HEAD
    play_loop_buttn,

    .touch = play_loop_touch
=======
    play_loop_buttn
>>>>>>> origin/csy-extras
};

struct state st_look = {
    look_enter,
    look_leave,
    look_paint,
    NULL,
    look_point,
    NULL,
    NULL,
    NULL,
    look_keybd,
    look_buttn
};
