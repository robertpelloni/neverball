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

#include "vec3.h"
#include "glext.h"
#include "state.h"
#include "config.h"
#include "video.h"
#include "common.h"
<<<<<<< HEAD
#include "hmd.h"
#include "geom.h"
#include "gui.h"
#include "transition.h"
#include "log.h"

/*---------------------------------------------------------------------------*/

#define STICK_MAX 32
#define STICK_HOLD_TIME 0.5f
#define STICK_REPEAT_TIME 0.2f

struct stick_cache
{
    int a;                              /* Axis index */
    float v, p;                         /* Axis value */
    float t;                            /* Repeat time */
    int device_id;
};

static struct stick_cache stick_cache[STICK_MAX];
static int                stick_count;

static void cache_stick(int a, float v, float t, int device_id)
{
    int i;

    /* Cache new values. */

    for (i = 0; i < stick_count; i++)
    {
        struct stick_cache *sc = &stick_cache[i];

        if (sc->a == a && sc->device_id == device_id)
        {
            sc->p = sc->v;
            sc->v = v;

            if (fabsf(v) >= 0.5f && sc->t == 0.0f)
                sc->t = t;
            else if (fabsf(v) < 0.5f)
                sc->t = 0.0f;

            return;
        }
    }

    /* Cache new axis. */

    if (stick_count < STICK_MAX)
    {
        struct stick_cache *sc = &stick_cache[stick_count];

        sc->a = a;
        sc->p = 0.0f;
        sc->v = v;
        sc->device_id = device_id;

        if (fabsf(v) >= 0.5f)
            sc->t = t;
        else if (fabsf(v) < 0.5f)
            sc->t = 0.0f;

        stick_count++;
    }
}

static int bump_stick(int a, int device_id)
{
    int i;

    for (i = 0; i < stick_count; i++)
    {
        struct stick_cache *sc = &stick_cache[i];

        if (sc->a == a && sc->device_id == device_id)
        {
            /* Note the transition from centered to leaned position. */

            return ((-0.5f <= sc->p && sc->p <= +0.5f) &&
                    (sc->v < -0.5f || +0.5f < sc->v));
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

=======

/*---------------------------------------------------------------------------*/

>>>>>>> origin/csy-extras
static float         state_time;
static int           state_drawn;
static struct state *state;

struct state *curr_state(void)
{
    return state;
}

float time_state(void)
{
    return state_time;
}

void init_state(struct state *st)
{
    state = st;
}

static int goto_state_intent(struct state *st, int intent)
{
    struct state *prev = state;

    if (state && state->leave)
<<<<<<< HEAD
        state->leave(state, st, state->gui_id, intent);
=======
        state->leave(state, st, state->gui_id);
>>>>>>> origin/csy-extras

    state       = st;
    state_time  = 0;
    state_drawn = 0;
<<<<<<< HEAD

    memset(&stick_cache, 0, sizeof (stick_cache));
    stick_count = 0;

    if (state && state->enter)
        state->gui_id = state->enter(state, prev, intent);
=======

    if (state && state->enter)
        state->gui_id = state->enter(state, prev);
>>>>>>> origin/csy-extras

    return 1;
}

int goto_state(struct state *st)
{
    return goto_state_intent(st, INTENT_FORWARD);
}

int exit_state(struct state *st)
{
    return goto_state_intent(st, INTENT_BACK);
}

/*---------------------------------------------------------------------------*/

void st_paint(float t)
{
    state_drawn = 1;

    if (state && state->paint)
    {
<<<<<<< HEAD
        video_clear();

        if (hmd_stat())
        {
            hmd_prep_left();
            video_clear();
            state->paint(state->gui_id, t);

            transition_paint();

            hmd_prep_right();
            video_clear();
            state->paint(state->gui_id, t);

            transition_paint();
=======
        /* TODO: reimplement stereo using LR instead of quad-buffer.

        if (config_get_d(CONFIG_STEREO))
        {
            glDrawBuffer(GL_BACK_LEFT);
            video_clear();
            state->paint(state->gui_id, t);

            glDrawBuffer(GL_BACK_RIGHT);
            video_clear();
            state->paint(state->gui_id, t);
>>>>>>> origin/csy-extras
        }
        else
        */
        {
<<<<<<< HEAD
            state->paint(state->gui_id, t);

            transition_paint();
=======
            video_clear();
            state->paint(state->gui_id, t);
>>>>>>> origin/csy-extras
        }
    }
}

void st_timer(float dt)
{
<<<<<<< HEAD
    int i;

    if (!state_drawn)
        return;

    transition_timer(dt);

=======
    if (!state_drawn)
        return;

>>>>>>> origin/csy-extras
    state_time += dt;

    if (state && state->timer)
        state->timer(state->gui_id, dt);

    for (i = 0; i < stick_count; i++)
    {
        struct stick_cache *sc = &stick_cache[i];

        if (sc->t > 0.0f && state_time >= sc->t)
        {
            state->stick(state->gui_id, sc->a, sc->v, 1, sc->device_id);
            sc->t = state_time + STICK_REPEAT_TIME;
        }
    }

    /* Step SOL animations. (This is not the best place to put this.) */

    geom_step(dt);
}

void st_point(int x, int y, int dx, int dy)
{
    if (state && state->point)
    {
        if (hmd_stat())
            state->point(state->gui_id, x * 2, y, dx, dy);
        else
            state->point(state->gui_id, x,     y, dx, dy);
    }
}

<<<<<<< HEAD
void st_stick(int a, float v, int device_id)
=======
void st_stick(int a, float v)
>>>>>>> origin/csy-extras
{
    static struct
    {
        const int *num;
        const int *inv;
<<<<<<< HEAD
    } axes[] = {
        { &CONFIG_JOYSTICK_AXIS_X0, &CONFIG_JOYSTICK_AXIS_X0_INVERT },
        { &CONFIG_JOYSTICK_AXIS_Y0, &CONFIG_JOYSTICK_AXIS_Y0_INVERT },
        { &CONFIG_JOYSTICK_AXIS_X1, &CONFIG_JOYSTICK_AXIS_X1_INVERT },
        { &CONFIG_JOYSTICK_AXIS_Y1, &CONFIG_JOYSTICK_AXIS_Y1_INVERT }
    };

    int i;

    for (i = 0; i < ARRAYSIZE(axes); i++)
        if (config_tst_d(*axes[i].num, a) && config_get_d(*axes[i].inv))
        {
            v = -v;
            break;
        }

    /* Poor man's deadzone. */

    if (fabsf(v) < 0.05f)
        v = 0.0f;

    if (state && state->stick)
    {
        cache_stick(a, v, state_time + STICK_HOLD_TIME, device_id);

        state->stick(state->gui_id, a, v, bump_stick(a, device_id), device_id);
    }
=======

        float prev;
    } axes[] = {
        { &CONFIG_JOYSTICK_AXIS_X, &CONFIG_JOYSTICK_AXIS_X_INVERT },
        { &CONFIG_JOYSTICK_AXIS_Y, &CONFIG_JOYSTICK_AXIS_Y_INVERT },
        { &CONFIG_JOYSTICK_AXIS_U, &CONFIG_JOYSTICK_AXIS_U_INVERT }
    };

    int i, bump = 0;

    for (i = 0; i < ARRAYSIZE(axes); i++)
        if (config_tst_d(*axes[i].num, a))
        {
            float p = axes[i].prev;

            /* Note the transition from centered to leaned position. */

            bump = ((-0.5f <= p && p <= +0.5f) &&
                    (v < -0.5f || +0.5f < v));

            axes[i].prev = v;

            if (config_get_d(*axes[i].inv))
                v = -v;

            break;
        }

    if (state && state->stick)
        state->stick(state->gui_id, a, v, bump);
>>>>>>> origin/csy-extras
}

void st_angle(float x, float z)
{
    if (state && state->angle)
        state->angle(state->gui_id, x, z);
<<<<<<< HEAD
}


void st_wheel(int x, int y)
{
    if (state && state->wheel)
        state->wheel(x, y);
=======
>>>>>>> origin/csy-extras
}

/*---------------------------------------------------------------------------*/

int st_click(int b, int d)
{
    return (state && state->click) ? state->click(b, d) : 1;
}

int st_keybd(int c, int d)
{
    return (state && state->keybd) ? state->keybd(c, d) : 1;
}

int st_buttn(int b, int d, int device_id)
{
    return (state && state->buttn) ? state->buttn(b, d, device_id) : 1;
}

int st_touch(const SDL_TouchFingerEvent *event)
{
    int d = 1;

    /* If the state can handle it, do it. */

    if (state && state->touch)
        return state->touch(event);

    /* Otherwise, emulate mouse events. */

    st_point(video.device_w * event->x,
             video.device_h * (1.0f - event->y),
             video.device_w * event->dx,
             video.device_h * -event->dy);

    if (event->type == SDL_FINGERDOWN)
        d = st_click(SDL_BUTTON_LEFT, 1);
    else if (event->type == SDL_FINGERUP)
        d = st_click(SDL_BUTTON_LEFT, 0);

    return d;
}

/*---------------------------------------------------------------------------*/
