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

#include <string.h>
#include <ctype.h>

#include "gui.h"
<<<<<<< HEAD
#include "transition.h"
=======
>>>>>>> origin/csy-extras
#include "util.h"
#include "audio.h"
#include "config.h"
#include "demo.h"
#include "progress.h"
#include "text.h"
#include "common.h"
<<<<<<< HEAD
#include "key.h"
=======
>>>>>>> origin/csy-extras

#include "game_common.h"

#include "st_save.h"
#include "st_shared.h"
<<<<<<< HEAD
=======

static char filename[MAXSTR];
>>>>>>> origin/csy-extras

/*---------------------------------------------------------------------------*/

static struct state *ok_state;
static struct state *cancel_state;

int goto_save(struct state *ok, struct state *cancel)
{
<<<<<<< HEAD
=======
    const char *name;

    name = demo_format_name(config_get_s(CONFIG_REPLAY_NAME),
                            set_id(curr_set()),
                            level_name(curr_level()));

    SAFECPY(filename, name);

>>>>>>> origin/csy-extras
    ok_state     = ok;
    cancel_state = cancel;

    return goto_state(&st_save);
}

/*---------------------------------------------------------------------------*/

static int file_id;

enum
{
<<<<<<< HEAD
    SAVE_SAVE = GUI_LAST
=======
    SAVE_SAVE = GUI_LAST,
    SAVE_CANCEL
>>>>>>> origin/csy-extras
};

static int save_action(int tok, int val)
{
    audio_play(AUD_MENU, 1.0f);

    switch (tok)
    {
<<<<<<< HEAD
    case GUI_BACK:
        return exit_state(cancel_state);

    case SAVE_SAVE:
        if (strlen(text_input) == 0)
            return 1;

        if (demo_exists(text_input))
=======
    case SAVE_SAVE:
        if (strlen(filename) == 0)
            return 1;

        if (demo_exists(filename))
>>>>>>> origin/csy-extras
        {
            return goto_state(&st_clobber);
        }
        else
        {
<<<<<<< HEAD
            demo_rename(text_input);
=======
            demo_rename(filename);
>>>>>>> origin/csy-extras
            return goto_state(ok_state);
        }

    case GUI_CL:
        gui_keyboard_lock();
        break;

    case GUI_BS:
        text_input_del();
        break;

    case GUI_CHAR:
<<<<<<< HEAD
        text_input_char(val);
        break;
=======
        if (!path_is_sep(val) &&
            text_add_char(val, filename, sizeof (filename)))
            gui_set_label(file_id, filename);
>>>>>>> origin/csy-extras
    }
    return 1;
}

static int enter_id;

static int save_gui(void)
{
    int id, jd;

    if ((id = gui_vstack(0)))
    {
        gui_label(id, _("Replay Name"), GUI_MED, 0, 0);
        gui_space(id);

        file_id = gui_label(id, " ", GUI_MED, gui_yel, gui_yel);

        gui_space(id);
        gui_keyboard(id);
        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            enter_id = gui_start(jd, _("Save"), GUI_SML, SAVE_SAVE, 0);
            gui_space(jd);
<<<<<<< HEAD
            gui_state(jd, _("Cancel"), GUI_SML, GUI_BACK, 0);
=======
            gui_state(jd, _("Cancel"), GUI_SML, SAVE_CANCEL, 0);
>>>>>>> origin/csy-extras
        }

        gui_layout(id, 0, 0);

        gui_set_trunc(file_id, TRUNC_HEAD);
<<<<<<< HEAD
        gui_set_label(file_id, text_input);
=======
        gui_set_label(file_id, filename);
>>>>>>> origin/csy-extras
    }

    return id;
}

<<<<<<< HEAD
static void on_text_input(int typing)
=======
static int save_enter(struct state *st, struct state *prev)
{
    SDL_EnableUNICODE(1);

    return save_gui();
}

static void save_leave(struct state *st, struct state *next, int id)
>>>>>>> origin/csy-extras
{
    if (file_id)
    {
        gui_set_label(file_id, text_input);

        if (typing)
            audio_play(AUD_MENU, 1.0f);
    }
}

static int save_enter(struct state *st, struct state *prev, int intent)
{
    const char *name;

    name = demo_format_name(config_get_s(CONFIG_REPLAY_NAME),
                            set_id(curr_set()),
                            level_name(curr_level()));

    text_input_start(on_text_input);
    text_input_str(name, 0);

    return transition_slide(save_gui(), 1, intent);
}

static int save_leave(struct state *st, struct state *next, int id, int intent)
{
    text_input_stop();

    return transition_slide(id, 0, intent);
}

static int save_keybd(int c, int d)
{
    if (d)
    {
        if (c == KEY_EXIT)
            return save_action(GUI_BACK, 0);

        if (c == '\b' || c == 0x7F)
<<<<<<< HEAD
        {
            gui_focus(enter_id);
            return save_action(GUI_BS, 0);
        }
        else
        {
            gui_focus(enter_id);
            return 1;
        }
=======
            return save_action(GUI_BS, 0);
        if (c >= ' ')
            return save_action(GUI_CHAR, c);
>>>>>>> origin/csy-extras
    }
    return 1;
}

static int save_buttn(int b, int d, int device_id)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            int tok = gui_token(gui_active());
            int val = gui_value(gui_active());

            return save_action(tok, (tok == GUI_CHAR ?
                                     gui_keyboard_char(val) :
                                     val));
        }
<<<<<<< HEAD
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return save_action(GUI_BACK, 0);
=======
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return save_action(SAVE_CANCEL, 0);
>>>>>>> origin/csy-extras
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int clobber_action(int tok, int val)
{
    audio_play(AUD_MENU, 1.0f);

    if (tok == SAVE_SAVE)
    {
<<<<<<< HEAD
        demo_rename(text_input);
=======
        demo_rename(filename);
>>>>>>> origin/csy-extras
        return goto_state(ok_state);
    }
    return exit_state(&st_save);
}

static int clobber_gui(void)
{
<<<<<<< HEAD
    int id, jd, kd, ld;
=======
    int id, jd, kd;
    int file_id;
>>>>>>> origin/csy-extras

    if ((id = gui_vstack(0)))
    {
        kd = gui_label(id, _("Overwrite?"), GUI_MED, gui_red, gui_red);
<<<<<<< HEAD
        ld = gui_label(id, "MMMMMMMM", GUI_MED, gui_yel, gui_yel);

        if ((jd = gui_harray(id)))
        {
            gui_start(jd, _("Cancel"),    GUI_SML, GUI_BACK, 0);
            gui_state(jd, _("Overwrite"), GUI_SML, SAVE_SAVE, 0);
=======

        file_id = gui_label(id, "MMMMMMMM", GUI_MED, gui_yel, gui_yel);

        if ((jd = gui_harray(id)))
        {
            gui_start(jd, _("No"),  GUI_SML, SAVE_CANCEL, 0);
            gui_state(jd, _("Yes"), GUI_SML, SAVE_SAVE,   0);
>>>>>>> origin/csy-extras
        }

        gui_pulse(kd, 1.2f);
        gui_layout(id, 0, 0);

<<<<<<< HEAD
        gui_set_trunc(ld, TRUNC_TAIL);
        gui_set_label(ld, text_input);
=======
        gui_set_trunc(file_id, TRUNC_TAIL);
        gui_set_label(file_id, filename);
>>>>>>> origin/csy-extras
    }

    return id;
}

<<<<<<< HEAD
static int clobber_enter(struct state *st, struct state *prev, int intent)
{
    return transition_slide(clobber_gui(), 1, intent);
}

static int clobber_keybd(int c, int d)
{
    if (d)
    {
        if (c == KEY_EXIT)
            return clobber_action(GUI_BACK, 0);
    }
    return 1;
}

static int clobber_buttn(int b, int d, int device_id)
{
    if (d)
    {
=======
static int clobber_enter(struct state *st, struct state *prev)
{
    return clobber_gui();
}

static int clobber_buttn(int b, int d)
{
    if (d)
    {
>>>>>>> origin/csy-extras
        int active = gui_active();

        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return clobber_action(gui_token(active), gui_value(active));
<<<<<<< HEAD
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return clobber_action(GUI_BACK, 0);
=======
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return clobber_action(SAVE_CANCEL, 0);
>>>>>>> origin/csy-extras
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_save = {
    save_enter,
    save_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shared_click,
    save_keybd,
    save_buttn
};

struct state st_clobber = {
    clobber_enter,
    shared_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shared_click,
<<<<<<< HEAD
    clobber_keybd,
=======
    NULL,
>>>>>>> origin/csy-extras
    clobber_buttn
};
