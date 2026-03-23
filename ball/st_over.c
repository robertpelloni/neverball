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
#include "video.h"
#include "demo.h"
<<<<<<< HEAD
#include "key.h"
=======
>>>>>>> origin/csy-extras

#include "game_common.h"

#include "st_over.h"
<<<<<<< HEAD
#include "st_start.h"
=======
#include "st_level.h"
>>>>>>> origin/csy-extras
#include "st_shared.h"

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
static int over_exit(void)
{
    audio_play(AUD_MENU, 1.0f);
    return goto_state(&st_start);
}

=======
>>>>>>> origin/csy-extras
static int over_gui(void)
{
    int id;

    if ((id = gui_label(0, _("GAME OVER"), GUI_LRG, gui_gry, gui_red)))
    {
        gui_layout(id, 0, 0);
        gui_pulse(id, 1.2f);
    }

    return id;
}

<<<<<<< HEAD
static int over_enter(struct state *st, struct state *prev, int intent)
=======
static int over_enter(struct state *st, struct state *prev)
>>>>>>> origin/csy-extras
{
    audio_music_fade_out(2.0f);
    audio_play(AUD_OVER, 1.f);

    video_clr_grab();

<<<<<<< HEAD
    return transition_slide(over_gui(), 1, intent);
=======
    return over_gui();
>>>>>>> origin/csy-extras
}

static void over_timer(int id, float dt)
{
<<<<<<< HEAD
=======
    if (time_state() > 3.f)
        goto_state(&st_exit);

>>>>>>> origin/csy-extras
    gui_timer(id, dt);

    if (time_state() > 3.f)
        over_exit();
}

static int over_click(int b, int d)
{
<<<<<<< HEAD
    return (b == SDL_BUTTON_LEFT && d == 1) ? over_exit() : 1;
=======
    return (b == SDL_BUTTON_LEFT && d == 1) ? goto_state(&st_exit) : 1;
>>>>>>> origin/csy-extras
}

static int over_keybd(int c, int d)
{
    if (d)
    {
        if (c == KEY_EXIT)
            return over_exit();
    }
    return 1;
}

static int over_buttn(int b, int d, int device_id)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b) ||
<<<<<<< HEAD
            config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return over_exit();
=======
            config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_state(&st_exit);
>>>>>>> origin/csy-extras
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_over = {
    over_enter,
    shared_leave,
    shared_paint,
    over_timer,
    NULL,
    NULL,
<<<<<<< HEAD
    NULL,
    over_click,
    over_keybd,
=======
    NULL,
    over_click,
    NULL,
>>>>>>> origin/csy-extras
    over_buttn
};
