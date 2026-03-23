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
#include "set.h"
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "util.h"
#include "common.h"
<<<<<<< HEAD
#include "key.h"
=======
>>>>>>> origin/csy-extras

#include "game_common.h"

#include "st_set.h"
#include "st_title.h"
#include "st_start.h"
#include "st_shared.h"

/*---------------------------------------------------------------------------*/

#define SET_STEP 6

static int total = 0;
static int first = 0;

static int shot_id;
static int desc_id;

static int do_init = 1;

enum
{
    SET_SELECT = GUI_LAST
};

static int set_action(int tok, int val)
{
    audio_play(AUD_MENU, 1.0f);

    switch (tok)
    {
    case GUI_BACK:
        set_quit();
<<<<<<< HEAD
        return exit_state(&st_title);
=======
        return goto_state(&st_title);
>>>>>>> origin/csy-extras
        break;

    case GUI_PREV:

        first -= SET_STEP;
<<<<<<< HEAD
        do_init = 0;
        return exit_state(&st_set);
=======

        do_init = 0;
        return goto_state(&st_set);
>>>>>>> origin/csy-extras

        break;

    case GUI_NEXT:

        first += SET_STEP;
<<<<<<< HEAD
=======

>>>>>>> origin/csy-extras
        do_init = 0;
        return goto_state(&st_set);

        break;

    case SET_SELECT:
<<<<<<< HEAD
        set_goto(val);
        return goto_state(&st_start);
        break;
=======
        if (set_exists(val))
        {
            set_goto(val);
            return goto_state(&st_start);
        }
>>>>>>> origin/csy-extras
    }

    return 1;
}

static void gui_set(int id, int i)
{
    if (set_exists(i))
<<<<<<< HEAD
    {
        int name_id;

        if (i % SET_STEP == 0)
            name_id = gui_start(id, "IJKLMNOPQRSTUVWXYZ", GUI_SML, SET_SELECT, i);
        else
            name_id = gui_state(id, "IJKLMNOPQRSTUVWXYZ", GUI_SML, SET_SELECT, i);

        gui_set_trunc(name_id, TRUNC_TAIL);
        gui_set_label(name_id, set_name(i));
    }
=======
        gui_state(id, set_name(i), GUI_SML, SET_SELECT, i);
>>>>>>> origin/csy-extras
    else
        gui_label(id, "", GUI_SML, 0, 0);
}

static int set_gui(void)
{
    int w = video.device_w;
    int h = video.device_h;

    int id, jd, kd, ld;

    int i;

    if ((id = gui_vstack(0)))
    {
        if ((jd = gui_hstack(id)))
        {
            gui_label(jd, _("Level Set"), GUI_SML, gui_yel, gui_red);
            gui_filler(jd);
<<<<<<< HEAD
            gui_navig(jd, total, first, SET_STEP);
        }

        if ((jd = gui_vstack(id)))
        {
            gui_space(jd);
=======
            gui_navig(jd, first > 0, first + SET_STEP < total);
        }

        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            shot_id = gui_image(jd, set_shot(first), 7 * w / 16, 7 * h / 16);
>>>>>>> origin/csy-extras

            if ((kd = gui_harray(jd)))
            {
                const int ww = MIN(w, h) * 7 / 12;
                const int hh = ww / 4 * 3;

<<<<<<< HEAD
                shot_id = gui_image(kd, set_shot(first), ww, hh);

                if ((ld = gui_varray(kd)))
                {
                    for (i = first; i < first + SET_STEP; i++)
                        gui_set(ld, i);
                }
            }

            gui_space(jd);

            desc_id = gui_multi(jd, " \n \n \n \n \n", GUI_SML, gui_yel, gui_wht);
        }
=======
        gui_space(id);
        desc_id = gui_multi(id, " \\ \\ \\ \\ \\", GUI_SML, gui_yel, gui_wht);
>>>>>>> origin/csy-extras

        gui_layout(id, 0, 0);
    }

    return id;
}

<<<<<<< HEAD
static int set_enter(struct state *st, struct state *prev, int intent)
=======
static int set_enter(struct state *st, struct state *prev)
>>>>>>> origin/csy-extras
{
    if (do_init)
    {
        total = set_init();
        first = MIN(first, (total - 1) - ((total - 1) % SET_STEP));

        audio_music_fade_to(0.5f, "bgm/inter.ogg");
        audio_play(AUD_START, 1.f);
    }
    else do_init = 1;

<<<<<<< HEAD
    if (prev == &st_set)
        return transition_page(set_gui(), 1, intent);

    return transition_slide(set_gui(), 1, intent);
}

static int set_leave(struct state *st, struct state *next, int id, int intent)
{
    if (next == &st_set)
        return transition_page(id, 0, intent);

    return transition_slide(id, 0, intent);
=======
    return set_gui();
>>>>>>> origin/csy-extras
}

static void set_over(int i)
{
    gui_set_image(shot_id, set_shot(i));
    gui_set_multi(desc_id, set_desc(i));
}

static void set_point(int id, int x, int y, int dx, int dy)
{
    int jd = shared_point_basic(id, x, y);

    if (jd && gui_token(jd) == SET_SELECT)
        set_over(gui_value(jd));
}

<<<<<<< HEAD
static void set_stick(int id, int a, float v, int bump, int device_id)
{
    int jd = shared_stick_basic(id, a, v, bump, device_id);
=======
static void set_stick(int id, int a, float v, int bump)
{
    int jd = shared_stick_basic(id, a, v, bump);
>>>>>>> origin/csy-extras

    if (jd && gui_token(jd) == SET_SELECT)
        set_over(gui_value(jd));
}

static int set_keybd(int c, int d)
{
    if (d)
    {
<<<<<<< HEAD
        if (c == KEY_EXIT)
            return set_action(GUI_BACK, 0);
    }
    return 1;
}

static int set_buttn(int b, int d, int device_id)
{
    if (d)
    {
=======
>>>>>>> origin/csy-extras
        int active = gui_active();

        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return set_action(gui_token(active), gui_value(active));
<<<<<<< HEAD
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return set_action(GUI_BACK, 0);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_L1, b) && first > 0)
            return set_action(GUI_PREV, 0);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_R1, b) && first + SET_STEP < total)
            return set_action(GUI_NEXT, 0);
=======
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return set_action(GUI_BACK, 0);
>>>>>>> origin/csy-extras
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_set = {
    set_enter,
    set_leave,
    shared_paint,
    shared_timer,
    set_point,
    set_stick,
    shared_angle,
    shared_click,
<<<<<<< HEAD
    set_keybd,
=======
    NULL,
>>>>>>> origin/csy-extras
    set_buttn
};
