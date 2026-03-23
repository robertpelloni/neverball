/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERPUTT is  free software; you can redistribute  it and/or modify
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
#include "geom.h"
#include "ball.h"
#include "part.h"
#include "game.h"
#include "audio.h"
#include "config.h"
#include "video.h"
<<<<<<< HEAD
#include "version.h"
#include "lang.h"
=======
>>>>>>> origin/csy-extras

#include "st_conf.h"
#include "st_all.h"
#include "st_common.h"

/*---------------------------------------------------------------------------*/

enum
{
<<<<<<< HEAD
    CONF_VIDEO = 1,
    CONF_LANG,
    CONF_BACK
=======
    CONF_FULL = 1,
    CONF_WIN,
    CONF_TEXHI,
    CONF_TEXLO,
    CONF_SHDON,
    CONF_SHDOF,
    CONF_BACK,
    CONF_RESOL
>>>>>>> origin/csy-extras
};

static int music_id[11];
static int sound_id[11];

static int conf_action(int i)
{
    int s = config_get_d(CONFIG_SOUND_VOLUME);
    int m = config_get_d(CONFIG_MUSIC_VOLUME);
    int r = 1;

    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
<<<<<<< HEAD
=======
    case CONF_FULL:
        goto_state(&st_null);
        r = video_mode(1, w, h);
        goto_state(&st_conf);
        break;

    case CONF_WIN:
        goto_state(&st_null);
        r = video_mode(0, w, h);
        goto_state(&st_conf);
        break;

    case CONF_TEXHI:
        goto_state(&st_null);
        config_set_d(CONFIG_TEXTURES, 1);
        goto_state(&st_conf);
        break;

    case CONF_TEXLO:
        goto_state(&st_null);
        config_set_d(CONFIG_TEXTURES, 2);
        goto_state(&st_conf);
        break;

    case CONF_SHDON:
        goto_state(&st_null);
        config_set_d(CONFIG_SHADOW, 1);
        goto_state(&st_conf);
        break;

    case CONF_SHDOF:
        goto_state(&st_null);
        config_set_d(CONFIG_SHADOW, 0);
        goto_state(&st_conf);
        break;

>>>>>>> origin/csy-extras
    case CONF_BACK:
        exit_state(&st_title);
        break;

    case CONF_VIDEO:
        goto_state(&st_video);
        break;

    case CONF_LANG:
        goto_state(&st_lang);
        break;

    default:
        if (100 <= i && i <= 110)
        {
            int n = i - 100;

            config_set_d(CONFIG_SOUND_VOLUME, n);
            audio_volume(n, m);
            audio_play(AUD_BUMP, 1.f);

            gui_toggle(sound_id[n]);
            gui_toggle(sound_id[s]);
        }
        if (200 <= i && i <= 210)
        {
            int n = i - 200;

            config_set_d(CONFIG_MUSIC_VOLUME, n);
            audio_volume(s, n);
            audio_play(AUD_BUMP, 1.f);

            gui_toggle(music_id[n]);
            gui_toggle(music_id[m]);
        }
    }

    return r;
}

<<<<<<< HEAD
static int conf_enter(struct state *st, struct state *prev, int intent)
{
    int root_id;
=======
static int conf_enter(struct state *st, struct state *prev)
{
    int id, jd, kd;
    int btn0, btn1;
    int i;
>>>>>>> origin/csy-extras

    back_init("back/gui.png");

    /* Initialize the configuration GUI. */

    if ((root_id = gui_root()))
    {
<<<<<<< HEAD
        int id, jd, kd;
        int i;
=======
        int f = config_get_d(CONFIG_FULLSCREEN);
        int t = config_get_d(CONFIG_TEXTURES);
        int h = config_get_d(CONFIG_SHADOW);
        int s = config_get_d(CONFIG_SOUND_VOLUME);
        int m = config_get_d(CONFIG_MUSIC_VOLUME);
>>>>>>> origin/csy-extras

        if ((id = gui_vstack(root_id)))
        {
<<<<<<< HEAD
            if ((jd = gui_harray(id)))
            {
                gui_label(jd, _("Options"), GUI_SML, 0, 0);
                gui_space(jd);
                gui_start(jd, _("Back"),    GUI_SML, CONF_BACK, 0);
            }

            gui_space(id);

            if ((jd = gui_harray(id)) &&
                (kd = gui_harray(jd)))
            {
                gui_state(kd, _("Configure"), GUI_SML, CONF_VIDEO, 0);

                gui_label(jd, _("Graphics"),  GUI_SML, 0, 0);
            }

            gui_space(id);

            if ((jd = gui_harray(id)) &&
                (kd = gui_harray(jd)))
            {
                /* A series of empty buttons forms the sound volume control. */

                int s = config_get_d(CONFIG_SOUND_VOLUME);

                for (i = 10; i >= 0; i--)
                {
                    sound_id[i] = gui_state(kd, NULL, GUI_SML, 100 + i, 0);
                    gui_set_hilite(sound_id[i], (s == i));
                }

                gui_label(jd, _("Sound Volume"), GUI_SML, 0, 0);
            }

            if ((jd = gui_harray(id)) &&
                (kd = gui_harray(jd)))
            {
                /* A series of empty buttons forms the music volume control. */

                int m = config_get_d(CONFIG_MUSIC_VOLUME);

                for (i = 10; i >= 0; i--)
                {
                    music_id[i] = gui_state(kd, NULL, GUI_SML, 200 + i, 0);
                    gui_set_hilite(music_id[i], (m == i));
                }

                gui_label(jd, _("Music Volume"), GUI_SML, 0, 0);
            }

            gui_space(id);

            if ((jd = gui_harray(id)) &&
                (kd = gui_harray(jd)))
            {
                gui_state(kd, _("Select"), GUI_SML, CONF_LANG, 0);

                gui_label(jd, _("Language"),  GUI_SML, 0, 0);
            }

            gui_layout(id, 0, 0);
=======
            gui_label(jd, _("Options"), GUI_SML, 0, 0);
            gui_space(jd);
            gui_start(jd, _("Back"),    GUI_SML, CONF_BACK, 0);
>>>>>>> origin/csy-extras
        }

        if ((id = gui_vstack(root_id)))
        {
<<<<<<< HEAD
            gui_label(id, "Neverputt " VERSION, GUI_TNY, gui_wht, gui_wht);
            gui_clr_rect(id);
            gui_layout(id, 0, -1);
        }
=======
            btn0 = gui_state(kd, _("No"),  GUI_SML, CONF_WIN,  0);
            btn1 = gui_state(kd, _("Yes"), GUI_SML, CONF_FULL, 0);

            if (f) gui_set_hilite(btn1, 1);
            else   gui_set_hilite(btn0, 1);

            gui_label(jd, _("Fullscreen"), GUI_SML, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            gui_state(kd, resolution, GUI_SML, CONF_RESOL, 0);

            gui_label(jd, _("Resolution"), GUI_SML, 0, 0);
        }

        gui_space(id);

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            btn0 = gui_state(kd, _("Low"),  GUI_SML, CONF_TEXLO, 0);
            btn1 = gui_state(kd, _("High"), GUI_SML, CONF_TEXHI, 0);

            gui_set_hilite(btn0, (t == 2));
            gui_set_hilite(btn1, (t == 1));

            gui_label(jd, _("Textures"), GUI_SML, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            btn0 = gui_state(kd, _("Off"),  GUI_SML, CONF_SHDOF, (h == 0));
            btn1 = gui_state(kd, _("On"),   GUI_SML, CONF_SHDON, (h == 1));

            if (h) gui_set_hilite(btn1, 1);
            else   gui_set_hilite(btn0, 1);

            gui_label(jd, _("Shadow"), GUI_SML, 0, 0);
        }

        gui_space(id);

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            /* A series of empty buttons forms the sound volume control. */

            for (i = 10; i >= 0; i--)
            {
                sound_id[i] = gui_state(kd, NULL, GUI_SML, 100 + i, 0);

                gui_set_hilite(sound_id[i], (s == i));
            }

            gui_label(jd, _("Sound Volume"), GUI_SML, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            /* A series of empty buttons forms the music volume control. */

            for (i = 10; i >= 0; i--)
            {
                music_id[i] = gui_state(kd, NULL, GUI_SML, 200 + i, 0);

                gui_set_hilite(music_id[i], (m == i));
            }

            gui_label(jd, _("Music Volume"), GUI_SML, 0, 0);
        }

        gui_layout(id, 0, 0);
>>>>>>> origin/csy-extras
    }

    audio_music_fade_to(0.5f, "bgm/inter.ogg");

    return transition_slide(root_id, 1, intent);
}

<<<<<<< HEAD
static int conf_leave(struct state *st, struct state *next, int id, int intent)
=======
static void conf_leave(struct state *st, struct state *next, int id)
>>>>>>> origin/csy-extras
{
    back_free();
    return transition_slide(id, 0, intent);
}

static void conf_paint(int id, float st)
{
    video_push_persp((float) config_get_d(CONFIG_VIEW_FOV), 0.1f, FAR_DIST);
    {
        back_draw_easy();
    }
    video_pop_matrix();
    gui_paint(id);
}

static void conf_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static void conf_point(int id, int x, int y, int dx, int dy)
{
    gui_pulse(gui_point(id, x, y), 1.2f);
}

<<<<<<< HEAD
static void conf_stick(int id, int a, float v, int bump, int device_id)
=======
static void conf_stick(int id, int a, float v, int bump)
>>>>>>> origin/csy-extras
{
    gui_pulse(gui_stick(id, a, v, bump), 1.2f);
}

static int conf_click(int b, int d)
{
    if (gui_click(b, d))
        return conf_action(gui_token(gui_active()));

    return 1;
}

static int conf_keybd(int c, int d)
{
    return (d && c == SDLK_ESCAPE) ? goto_state(&st_title) : 1;
}

static int conf_buttn(int b, int d, int device_id)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return conf_action(gui_token(gui_active()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return goto_state(&st_title);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
static int null_enter(struct state *st, struct state *prev, int intent)
=======
static int null_enter(struct state *st, struct state *prev)
>>>>>>> origin/csy-extras
{
    transition_quit();
    gui_free();
    geom_free();
    ball_free();
    shad_free();
    mtrl_free_objects();

    return 0;
}

<<<<<<< HEAD
static int null_leave(struct state *st, struct state *next, int id, int intent)
{
    mtrl_load_objects();
=======
static void null_leave(struct state *st, struct state *next, int id)
{
>>>>>>> origin/csy-extras
    shad_init();
    ball_init();
    geom_init();
    gui_init();
    transition_init();
    return 0;
}

/*---------------------------------------------------------------------------*/

struct state st_conf = {
    conf_enter,
    conf_leave,
    conf_paint,
    conf_timer,
    conf_point,
    conf_stick,
    NULL,
    conf_click,
    conf_keybd,
    conf_buttn
};

struct state st_null = {
    null_enter,
    null_leave,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};
