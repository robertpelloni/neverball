/*
 * Copyright (C) 2003-2010 Neverball authors
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
<<<<<<< HEAD
#include "transition.h"
=======
>>>>>>> origin/csy-extras
#include "state.h"
#include "array.h"
#include "dir.h"
#include "config.h"
#include "fs.h"
#include "common.h"
#include "ball.h"
#include "cmd.h"
#include "audio.h"
#include "geom.h"
#include "video.h"
#include "demo.h"
<<<<<<< HEAD
#include "key.h"
=======
>>>>>>> origin/csy-extras

#include "game_server.h"
#include "game_proxy.h"
#include "game_client.h"
#include "game_common.h"

#include "st_ball.h"
#include "st_conf.h"
#include "st_shared.h"

<<<<<<< HEAD
=======
enum
{
    BALL_NEXT = GUI_LAST,
    BALL_PREV,
    BALL_BACK
};

>>>>>>> origin/csy-extras
static Array balls;
static int   curr_ball;
static char  ball_file[64];

static int name_id;

static int has_ball_sols(struct dir_item *item)
{
    char *solid, *inner, *outer;
    int yes;

    solid = concat_string(item->path,
                          "/",
                          base_name(item->path),
                          "-solid.sol",
                          NULL);
    inner = concat_string(item->path,
                          "/",
                          base_name(item->path),
                          "-inner.sol",
                          NULL);
    outer = concat_string(item->path,
                          "/",
                          base_name(item->path),
                          "-outer.sol",
                          NULL);

    yes = (fs_exists(solid) || fs_exists(inner) || fs_exists(outer));

    free(solid);
    free(inner);
    free(outer);

    return yes;
}

<<<<<<< HEAD
static int cmp_dir_items(const void *A, const void *B)
{
    const struct dir_item *a = A, *b = B;
    return strcmp(a->path, b->path);
}

=======
>>>>>>> origin/csy-extras
static void scan_balls(void)
{
    int i;

    SAFECPY(ball_file, config_get_s(CONFIG_BALL_FILE));

    if ((balls = fs_dir_scan("ball", has_ball_sols)))
    {
<<<<<<< HEAD
        array_sort(balls, cmp_dir_items);

=======
>>>>>>> origin/csy-extras
        for (i = 0; i < array_len(balls); i++)
        {
            const char *path = DIR_ITEM_GET(balls, i)->path;

            if (strncmp(ball_file, path, strlen(path)) == 0)
            {
                curr_ball = i;
                break;
            }
        }
    }
}

static void free_balls(void)
{
    fs_dir_free(balls);
    balls = NULL;
}

<<<<<<< HEAD
static void set_curr_ball(int ball_index)
{
    sprintf(ball_file, "%s/%s",
            DIR_ITEM_GET(balls, ball_index)->path,
            base_name(DIR_ITEM_GET(balls, ball_index)->path));
=======
static void set_curr_ball(void)
{
    sprintf(ball_file, "%s/%s",
            DIR_ITEM_GET(balls, curr_ball)->path,
            base_name(DIR_ITEM_GET(balls, curr_ball)->path));
>>>>>>> origin/csy-extras

    config_set_s(CONFIG_BALL_FILE, ball_file);

    ball_free();
    ball_init();

    gui_set_label(name_id, base_name(ball_file));
}

static int ball_action(int tok, int val)
{
    audio_play(AUD_MENU, 1.0f);

    switch (tok)
    {
<<<<<<< HEAD
    case GUI_NEXT:
        if (++curr_ball == array_len(balls))
            curr_ball = 0;

        set_curr_ball(curr_ball);

        break;

    case GUI_PREV:
        if (--curr_ball == -1)
            curr_ball = array_len(balls) - 1;

        set_curr_ball(curr_ball);

        break;

    case GUI_BACK:
        exit_state(&st_conf);
=======
    case BALL_NEXT:
        if (++curr_ball == array_len(balls))
            curr_ball = 0;

        set_curr_ball();

        break;

    case BALL_PREV:
        if (--curr_ball == -1)
            curr_ball = array_len(balls) - 1;

        set_curr_ball();

        break;

    case BALL_BACK:
        goto_state(&st_conf);
>>>>>>> origin/csy-extras
        break;
    }

    return 1;
}

static void load_ball_demo(void)
{
    int g;

    /* "g" is a stupid hack to keep the goal locked. */

    if (!demo_replay_init("gui/ball.nbr", &g, NULL, NULL, NULL, NULL))
    {
<<<<<<< HEAD
        ball_action(GUI_BACK, 0);
=======
        ball_action(BALL_BACK, 0);
>>>>>>> origin/csy-extras
        return;
    }

    audio_music_fade_to(0.0f, "bgm/inter.ogg");
    game_client_fly(0);
    game_kill_fade();
    back_init("back/gui.png");
}

static int ball_gui(void)
{
    int id, jd;
    int i;

    if ((id = gui_vstack(0)))
    {
        if ((jd = gui_harray(id)))
        {
            gui_label(jd, _("Ball Model"), GUI_SML, 0, 0);
            gui_space(jd);
<<<<<<< HEAD
            gui_start(jd, _("Back"), GUI_SML, GUI_BACK, 0);
=======
            gui_start(jd, _("Back"), GUI_SML, BALL_BACK, 0);
>>>>>>> origin/csy-extras
        }

        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
<<<<<<< HEAD
            gui_state(jd, " " GUI_TRIANGLE_RIGHT " ", GUI_SML, GUI_NEXT, 0);
=======
            gui_state(jd, " > ", GUI_SML, BALL_NEXT, 0);
>>>>>>> origin/csy-extras

            name_id = gui_label(jd, "very-long-ball-name", GUI_SML,
                                gui_wht, gui_wht);

            gui_set_trunc(name_id, TRUNC_TAIL);
            gui_set_fill(name_id);

<<<<<<< HEAD
            gui_state(jd, " " GUI_TRIANGLE_LEFT " ", GUI_SML, GUI_PREV, 0);
=======
            gui_state(jd, " < ", GUI_SML, BALL_PREV, 0);
>>>>>>> origin/csy-extras
        }

        for (i = 0; i < 12; i++)
            gui_space(id);

        gui_layout(id, 0, 0);

        gui_set_label(name_id, base_name(ball_file));
    }

    return id;
}

<<<<<<< HEAD
static int ball_enter(struct state *st, struct state *prev, int intent)
=======
static int ball_enter(struct state *st, struct state *prev)
>>>>>>> origin/csy-extras
{
    scan_balls();
    load_ball_demo();

<<<<<<< HEAD
    return transition_slide(ball_gui(), 1, intent);
}

static int ball_leave(struct state *st, struct state *next, int id, int intent)
{
    back_free();
    demo_replay_stop(0);
    free_balls();
    return transition_slide(id, 0, intent);
=======
    return ball_gui();
}

static void ball_leave(struct state *st, struct state *next, int id)
{
    gui_delete(id);
    back_free();
    demo_replay_stop(0);
    free_balls();
>>>>>>> origin/csy-extras
}

static void ball_paint(int id, float t)
{
    video_push_persp((float) config_get_d(CONFIG_VIEW_FOV), 0.1f, FAR_DIST);
    {
        back_draw_easy();
    }
    video_pop_matrix();

    game_client_draw(POSE_BALL, t);
    gui_paint(id);
}

static void ball_timer(int id, float dt)
{
    gui_timer(id, dt);

    if (!demo_replay_step(dt))
    {
        demo_replay_stop(0);
        load_ball_demo();
    }

    game_client_blend(demo_replay_blend());
}

<<<<<<< HEAD
static int ball_keybd(int c, int d)
{
    int initial_fov = config_get_d(CONFIG_VIEW_FOV);
    int initial_dc = config_get_d(CONFIG_VIEW_DC);
    int initial_dp = config_get_d(CONFIG_VIEW_DP);
    int initial_w = config_get_d(CONFIG_WIDTH);
    int initial_h = config_get_d(CONFIG_HEIGHT);

    int i;

    if (d)
    {
        switch (c)
        {
            case KEY_EXIT:
                return ball_action(GUI_BACK, 0);

            case KEY_LEVELSHOTS:

                video_set_window_size(800 / video.device_scale, 600 / video.device_scale);
                video_resize(800 / video.device_scale, 600 / video.device_scale);

                // Zoom in on the ball.

                config_set_d(CONFIG_VIEW_DC, 0);
                config_set_d(CONFIG_VIEW_DP, 50);
                config_set_d(CONFIG_VIEW_FOV, 20);

                game_client_fly(0.0f);

                // Take screenshots.

                for (i = 0; balls && i < array_len(balls); ++i)
                {
                    static char filename[64];

                    sprintf(filename, "Screenshots/ball-%s.png", base_name(DIR_ITEM_GET(balls, i)->path));

                    set_curr_ball(i);

                    video_clear();
                    video_push_persp((float) initial_fov, 0.1f, FAR_DIST);
                    {
                        back_draw_easy();
                    }
                    video_pop_matrix();

                    game_client_draw(POSE_BALL, 0);
                    video_snap(filename);
                    video_swap();
                }

                // Restore config.

                config_set_d(CONFIG_VIEW_FOV, initial_fov);
                config_set_d(CONFIG_VIEW_DC, initial_dc);
                config_set_d(CONFIG_VIEW_DP, initial_dp);

                video_set_window_size(initial_w, initial_h);
                video_resize(initial_w, initial_h);

                set_curr_ball(curr_ball);

                break;
        }
    }
    return 1;
}

static int ball_buttn(int b, int d, int device_id)
=======
static int ball_buttn(int b, int d)
>>>>>>> origin/csy-extras
{
    if (d)
    {
        int active = gui_active();

        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return ball_action(gui_token(active), gui_value(active));

<<<<<<< HEAD
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return ball_action(GUI_BACK, 0);

        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L1, b))
            return ball_action(GUI_PREV, 0);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_R1, b))
            return ball_action(GUI_NEXT, 0);
=======
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return ball_action(BALL_BACK, 0);
>>>>>>> origin/csy-extras
    }
    return 1;
}

struct state st_ball = {
    ball_enter,
    ball_leave,
    ball_paint,
    ball_timer,
    shared_point,
    shared_stick,
    NULL,
    shared_click,
<<<<<<< HEAD
    ball_keybd,
=======
    NULL,
>>>>>>> origin/csy-extras
    ball_buttn
};
