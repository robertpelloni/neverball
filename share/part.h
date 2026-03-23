#ifndef PART_H
#define PART_H

#include "solid_draw.h"

/*---------------------------------------------------------------------------*/

#define IMG_PART_STAR     "png/part"
<<<<<<< HEAD

#define PART_MAX_COIN  256

=======

#define PART_MAX_COIN  64
#define PART_MAX_GOAL  64
#define PART_MAX_JUMP  64

>>>>>>> origin/csy-extras
#define PART_SIZE       0.1f

/*---------------------------------------------------------------------------*/

void part_reset(void);
void part_init(void);
void part_free(void);

void part_burst(const float *, const float *);
void part_step(const float *, float);

<<<<<<< HEAD
void part_draw_coin(const struct s_draw *draw, struct s_rend *rend, const float *M, float t);
=======
void part_draw_coin(struct s_rend *);
>>>>>>> origin/csy-extras

void part_lerp_apply(float);

/*---------------------------------------------------------------------------*/

#endif
