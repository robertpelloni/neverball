#ifndef GAME_CLIENT_H
#define GAME_CLIENT_H

#include "fs.h"

/*---------------------------------------------------------------------------*/

enum
{
    POSE_NONE = 0,
    POSE_LEVEL,
    POSE_BALL
};

int   game_client_init(const char *);
void  game_client_free(const char *);
void  game_client_sync(fs_file);
void  game_client_draw(int, float);
void  game_client_blend(float);

void  game_client_ghost_init(void);
void  game_client_ghost_sync(fs_file);
void  game_client_draw_ghost(int, float);
void  game_client_draw_debug(int p);

int   curr_clock(int);
int   curr_coins(int);
int   curr_status(int);
int   curr_jump_ready(int);
float curr_dash_charge(int);
float curr_gyro_timer(int);

void curr_map_bounds(float *min_v, float *max_v);
void curr_ball_pos(int p, float *pos);
int curr_goal_count(void);
void curr_goal_pos(int i, float *pos);

void  game_look(float, float);

void  game_kill_fade(void);
void  game_step_fade(float);
void  game_fade(float);

void game_client_fly(float);

/*---------------------------------------------------------------------------*/

extern int game_compat_map;

/*---------------------------------------------------------------------------*/

#endif
