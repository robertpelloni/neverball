#ifndef STATE_H
#define STATE_H

<<<<<<< HEAD
#include "SDL_events.h"

=======
>>>>>>> origin/csy-extras
/*---------------------------------------------------------------------------*/

struct state
{
<<<<<<< HEAD
    int  (*enter)(struct state *, struct state *prev, int intent);
    int  (*leave)(struct state *, struct state *next, int id, int intent);
    void (*paint)(int id, float t);
    void (*timer)(int id, float dt);
    void (*point)(int id, int x, int y, int dx, int dy);
    void (*stick)(int id, int a, float v, int bump, int device_id);
=======
    int  (*enter)(struct state *, struct state *prev);
    void (*leave)(struct state *, struct state *next, int id);
    void (*paint)(int id, float t);
    void (*timer)(int id, float dt);
    void (*point)(int id, int x, int y, int dx, int dy);
    void (*stick)(int id, int a, float v, int bump);
>>>>>>> origin/csy-extras
    void (*angle)(int id, float x, float z);
    int  (*click)(int b,  int d);
    int  (*keybd)(int c,  int d);
    int  (*buttn)(int b,  int d, int device_id);
    void (*wheel)(int x,  int y);

    int gui_id;

    int  (*touch)(const SDL_TouchFingerEvent *);
};

struct state *curr_state(void);

float time_state(void);
void  init_state(struct state *);
int   goto_state(struct state *);
int   exit_state(struct state *);

void st_paint(float);
void st_timer(float);
void st_point(int, int, int, int);
<<<<<<< HEAD
void st_stick(int, float, int);
void st_angle(float, float);
void st_wheel(int, int);
=======
void st_stick(int, float);
void st_angle(float, float);
>>>>>>> origin/csy-extras
int  st_click(int, int);
int  st_keybd(int, int);
int  st_buttn(int, int, int);
int  st_touch(const SDL_TouchFingerEvent *);

/*---------------------------------------------------------------------------*/

#endif
