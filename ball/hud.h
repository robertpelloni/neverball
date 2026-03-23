#ifndef HUD_H
#define HUD_H

<<<<<<< HEAD
#include "SDL_events.h"

=======
>>>>>>> origin/csy-extras
/*---------------------------------------------------------------------------*/

void hud_init(void);
void hud_free(void);

void hud_paint(int x, int y, int w, int h);
void hud_timer(float);
void hud_update(int, int);

<<<<<<< HEAD
void hud_show(float delay);
void hud_hide(void);

int hud_touch(const SDL_TouchFingerEvent *);

void hud_cam_pulse(int);
void hud_cam_timer(float);
void hud_cam_paint(void);

void hud_speed_pulse(int);
void hud_speed_timer(float);
void hud_speed_paint(void);

void hud_touch_timer(float);
void hud_touch_paint(void);
=======
void hud_view_pulse(int);
void hud_view_timer(float);
void hud_view_paint();

void hud_speed_pulse(int);
void hud_speed_timer(float);
void hud_speed_paint();
>>>>>>> origin/csy-extras

/*---------------------------------------------------------------------------*/

#endif
