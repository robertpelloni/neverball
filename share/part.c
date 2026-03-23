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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "glext.h"
#include "part.h"
#include "vec3.h"
#include "image.h"
#include "geom.h"
<<<<<<< HEAD
#include "hmd.h"
#include "video.h"
=======

/*---------------------------------------------------------------------------*/
/*
#define PARTICLEVBO 1
*/

struct part_vary
{
    GLfloat v[3];             /* Velocity                                    */
};

struct part_draw
{
    GLfloat p[3];             /* Position                                    */
    GLfloat c[3];             /* Color                                       */
    GLfloat t;                /* Time until death. Doubles as opacity.       */
};

static struct part_vary coin_vary[PART_MAX_COIN];
static struct part_draw coin_draw[PART_MAX_COIN];

static GLuint coin_vbo;
>>>>>>> origin/csy-extras

/*---------------------------------------------------------------------------*/

static struct b_mtrl coin_base_mtrl =
{
<<<<<<< HEAD
    float v[3];               /* Velocity                                    */
    float w;                  /* Angular velocity (degrees)                  */
    float p[3];               /* Position                                    */
    float c[3];               /* Color                                       */
    float t;                  /* Time until death. Doubles as opacity.       */
};

static struct part coin_part[PART_MAX_COIN];

static GLuint coin_vbo;
static GLuint coin_ebo;

/*---------------------------------------------------------------------------*/

static struct b_mtrl coin_base_mtrl =
{
    { 0.8f, 0.8f, 0.8f, 1.0f },
    { 0.2f, 0.2f, 0.2f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f }, 0.0f, M_TRANSPARENT, IMG_PART_STAR
};

static int coin_mtrl;
=======
    { 0.8f, 0.8f, 0.8f, 1.0f },
    { 0.2f, 0.2f, 0.2f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f },
    { 0.0f }, 0.0f, M_TRANSPARENT, IMG_PART_STAR
};

static struct d_mtrl coin_draw_mtrl;
>>>>>>> origin/csy-extras

/*---------------------------------------------------------------------------*/

#define PI 3.1415927f

static float rnd(float l, float h)
{
    return l + (h - l) * rand() / (float) RAND_MAX;
}

/*---------------------------------------------------------------------------*/

#define CURR 0
#define PREV 1

struct part_lerp
{
    float p[2][3];
};

static struct part_lerp part_lerp_coin[PART_MAX_COIN];

void part_lerp_copy(void)
{
    int i;

    for (i = 0; i < PART_MAX_COIN; i++)
        v_cpy(part_lerp_coin[i].p[PREV],
              part_lerp_coin[i].p[CURR]);
}

void part_lerp_init(void)
{
}

void part_lerp_burst(int i)
{
<<<<<<< HEAD
    if (coin_part[i].t >= 1.0f)
    {
        v_cpy(part_lerp_coin[i].p[PREV], coin_part[i].p);
        v_cpy(part_lerp_coin[i].p[CURR], coin_part[i].p);
=======
    if (coin_draw[i].t >= 1.0f)
    {
        v_cpy(part_lerp_coin[i].p[PREV], coin_draw[i].p);
        v_cpy(part_lerp_coin[i].p[CURR], coin_draw[i].p);
>>>>>>> origin/csy-extras
    }
}

void part_lerp_apply(float a)
{
    int i;

    for (i = 0; i < PART_MAX_COIN; i++)
<<<<<<< HEAD
        if (coin_part[i].t > 0.0f)
            v_lerp(coin_part[i].p,
                   part_lerp_coin[i].p[PREV],
                   part_lerp_coin[i].p[CURR], a);
}

/*---------------------------------------------------------------------------*/

void part_reset(void)
{
    int i;

    for (i = 0; i < PART_MAX_COIN; i++)
        coin_part[i].t = 0.0f;

    part_lerp_init();
}

void part_init(void)
{
    static const GLfloat verts[4][5] = {
        { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f },
        { +0.5f, -0.5f, 0.0f, 1.0f, 0.0f },
        { -0.5f, +0.5f, 0.0f, 0.0f, 1.0f },
        { +0.5f, +0.5f, 0.0f, 1.0f, 1.0f },
    };

    static const GLushort elems[4] = {
        0u, 1u, 2u, 3u
    };

    coin_mtrl = mtrl_cache(&coin_base_mtrl);

    memset(coin_part, 0, sizeof (coin_part));

    glGenBuffers_(1,              &coin_vbo);
    glBindBuffer_(GL_ARRAY_BUFFER, coin_vbo);
    glBufferData_(GL_ARRAY_BUFFER, sizeof (verts), verts, GL_STATIC_DRAW);
    glBindBuffer_(GL_ARRAY_BUFFER, 0);

    glGenBuffers_(1, &coin_ebo);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, coin_ebo);
    glBufferData_(GL_ELEMENT_ARRAY_BUFFER, sizeof (elems), elems, GL_STATIC_DRAW);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, 0);
=======
        if (coin_draw[i].t > 0.0f)
            v_lerp(coin_draw[i].p,
                   part_lerp_coin[i].p[PREV],
                   part_lerp_coin[i].p[CURR], a);

    /* Upload the current state of the particles. It would be best to limit  */
    /* this upload to only active particles, but it's more important to do   */
    /* it all in a single call.                                              */

#ifdef PARTICLEVBO
    glBindBuffer_   (GL_ARRAY_BUFFER, coin_vbo);
    glBufferSubData_(GL_ARRAY_BUFFER, 0, sizeof (coin_draw), coin_draw);
    glBindBuffer_   (GL_ARRAY_BUFFER, 0);
#endif
}

/*---------------------------------------------------------------------------*/

void part_reset(void)
{
    int i;

    for (i = 0; i < PART_MAX_COIN; i++)
        coin_draw[i].t = 0.0f;

    part_lerp_init();
}

void part_init(void)
{
    sol_load_mtrl(&coin_draw_mtrl, &coin_base_mtrl);

    memset(coin_vary, 0, PART_MAX_COIN * sizeof (struct part_vary));
    memset(coin_draw, 0, PART_MAX_COIN * sizeof (struct part_draw));

#ifdef PARTICLEVBO
    glGenBuffers_(1,              &coin_vbo);
    glBindBuffer_(GL_ARRAY_BUFFER, coin_vbo);
    glBufferData_(GL_ARRAY_BUFFER, sizeof (coin_draw),
                                          coin_draw, GL_DYNAMIC_DRAW);
    glBindBuffer_(GL_ARRAY_BUFFER, 0);
#endif
>>>>>>> origin/csy-extras

    part_reset();
}

void part_free(void)
{
    glDeleteBuffers_(1, &coin_vbo);

<<<<<<< HEAD
    mtrl_free(coin_mtrl);
    coin_mtrl = 0;
=======
    sol_free_mtrl(&coin_draw_mtrl);
>>>>>>> origin/csy-extras
}

/*---------------------------------------------------------------------------*/

void part_burst(const float *p, const float *c)
{
    int i, n = 0;

    for (i = 0; n < 10 && i < PART_MAX_COIN; i++)
<<<<<<< HEAD
        if (coin_part[i].t <= 0.f)
=======
        if (coin_draw[i].t <= 0.f)
>>>>>>> origin/csy-extras
        {
            float a = rnd(-1.0f * PI, +1.0f * PI);
            float b = rnd(+0.3f * PI, +0.5f * PI);

<<<<<<< HEAD
            coin_part[i].c[0] = c[0];
            coin_part[i].c[1] = c[1];
            coin_part[i].c[2] = c[2];

            coin_part[i].p[0] = p[0];
            coin_part[i].p[1] = p[1];
            coin_part[i].p[2] = p[2];

            coin_part[i].v[0] = 4.f * fcosf(a) * fcosf(b);
            coin_part[i].v[1] = 4.f *            fsinf(b);
            coin_part[i].v[2] = 4.f * fsinf(a) * fcosf(b);

            coin_part[i].w = V_DEG(w);

            coin_part[i].t = 1.f;
=======
            coin_draw[i].c[0] = c[0];
            coin_draw[i].c[1] = c[1];
            coin_draw[i].c[2] = c[2];

            coin_draw[i].p[0] = p[0];
            coin_draw[i].p[1] = p[1];
            coin_draw[i].p[2] = p[2];

            coin_vary[i].v[0] = 4.f * fcosf(a) * fcosf(b);
            coin_vary[i].v[1] = 4.f *            fsinf(b);
            coin_vary[i].v[2] = 4.f * fsinf(a) * fcosf(b);

            coin_draw[i].t = 1.f;
>>>>>>> origin/csy-extras

            part_lerp_burst(i);

            n++;
        }
}

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
static void part_fall(const float *g, float dt)
{
    int i;

    for (i = 0; i < PART_MAX_COIN; i++)
        if (coin_part[i].t > 0.f)
        {
            coin_part[i].t -= dt;

            v_mad(coin_part[i].v, coin_part[i].v, g, dt);

            v_mad(part_lerp_coin[i].p[CURR], part_lerp_coin[i].p[CURR], coin_part[i].v, dt);
        }
        else coin_part[i].t = 0.0f;
=======
static void part_fall(struct part_lerp *lerp,
                      struct part_vary *vary,
                      struct part_draw *draw,
                      int n, const float *g, float dt)
{
    int i;

    for (i = 0; i < n; i++)
        if (draw[i].t > 0.f)
        {
            draw[i].t -= dt;

            v_mad(vary[i].v, vary[i].v, g, dt);

            v_mad(lerp[i].p[CURR], lerp[i].p[CURR], vary[i].v, dt);
        }
        else draw[i].t = 0.0f;
>>>>>>> origin/csy-extras
}

void part_step(const float *g, float dt)
{
    part_lerp_copy();
<<<<<<< HEAD
    part_fall(g, dt);
=======
    part_fall(part_lerp_coin, coin_vary, coin_draw, PART_MAX_COIN, g, dt);
>>>>>>> origin/csy-extras
}

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
void part_draw_coin(const struct s_draw *draw, struct s_rend *rend, const float *M, float t)
{
    int i;

    r_apply_mtrl(rend, coin_mtrl);

    glBindBuffer_(GL_ARRAY_BUFFER, coin_vbo);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, coin_ebo);

    glDisableClientState(GL_NORMAL_ARRAY);
    {
        glVertexPointer  (3, GL_FLOAT, sizeof (GLfloat) * 5, (GLvoid *) (                   0u));
        glTexCoordPointer(2, GL_FLOAT, sizeof (GLfloat) * 5, (GLvoid *) (sizeof (GLfloat) * 3u));

        for (i = 0; i < PART_MAX_COIN; ++i)
            if (coin_part[i].t > 0.0f)
            {
                glColor4f(coin_part[i].c[0], coin_part[i].c[1], coin_part[i].c[2], coin_part[i].t);

                glPushMatrix();
                {
                    glTranslatef(coin_part[i].p[0], coin_part[i].p[1], coin_part[i].p[2]);

                    if (M)
                        glMultMatrixf(M);

                    glScalef(PART_SIZE * 2.0f, PART_SIZE * 2.0f, 1.0f);
                    glRotatef(t * coin_part[i].w, 0.0f, 0.0f, 1.0f);

                    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (GLvoid *) 0u);
                }
                glPopMatrix();
            }
    }
    glEnableClientState(GL_NORMAL_ARRAY);

    glBindBuffer_(GL_ARRAY_BUFFER, 0);
    glBindBuffer_(GL_ELEMENT_ARRAY_BUFFER, 0);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
=======
void part_draw_coin(struct s_rend *rend)
{
    const GLfloat c[3] = { 0.0f, 1.0f, 0.0f };
    GLint s = config_get_d(CONFIG_HEIGHT) / 8;

    sol_apply_mtrl(&coin_draw_mtrl, rend);

    /* Draw the entire buffer.  Dead particles have zero opacity anyway. */

#ifdef PARTICLEVBO
    glBindBuffer_(GL_ARRAY_BUFFER, coin_vbo);
#else
    glBindBuffer_(GL_ARRAY_BUFFER, 0);
#endif

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    {
#ifdef PARTICLEVBO
        glColorPointer (4, GL_FLOAT, sizeof (struct part_draw),
                        (GLvoid *) offsetof (struct part_draw, c));
        glVertexPointer(3, GL_FLOAT, sizeof (struct part_draw),
                        (GLvoid *) offsetof (struct part_draw, p));
#else
        glColorPointer (4, GL_FLOAT, sizeof (struct part_draw), coin_draw[0].c);
        glVertexPointer(3, GL_FLOAT, sizeof (struct part_draw), coin_draw[0].p);
#endif

        glEnable(GL_POINT_SPRITE);
        {
            glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
            glPointParameterfv_(GL_POINT_DISTANCE_ATTENUATION, c);
            glPointSize(s);

            glDrawArrays(GL_POINTS, 0, PART_MAX_COIN);
        }
        glDisable(GL_POINT_SPRITE);
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
>>>>>>> origin/csy-extras
}

/*---------------------------------------------------------------------------*/
