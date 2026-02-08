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

#ifndef KEY_H
#define KEY_H

#include <SDL.h>

enum {
    KEY_NONE,
    KEY_EXIT,
    KEY_SCREENSHOT,
    KEY_FPS,
    KEY_WIREFRAME,
    KEY_RESOURCES,
    KEY_FULLSCREEN,
    KEY_CAMERA,
    KEY_POSE,
    KEY_LOOKAROUND,
    KEY_LEVELSHOTS,
    KEY_MAX
};

/* Virtual keys for game actions */
/* These map to actual SDL keys via config */

/* ... existing code ... */

#endif
