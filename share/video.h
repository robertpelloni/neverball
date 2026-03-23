/*
<<<<<<< HEAD
 * Copyright (C) 2003 Robert Kooima
=======
<<<<<<<< HEAD:share/fs_png.c
 * Copyright (C) 2003-2010 Neverball authors
========
 * Copyright (C) 2003 Robert Kooima
>>>>>>>> origin/csy-extras:share/video.h
>>>>>>> origin/csy-extras
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

<<<<<<< HEAD
=======
<<<<<<<< HEAD:share/fs_png.c
#include <png.h>
#include <string.h>
#include "fs_png.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

void fs_png_read(png_structp readp, png_bytep data, png_size_t length)
{
    int read = fs_read(data, length, png_get_io_ptr(readp));

    if (read < length)
        memset(data + read, 0, length - read);
}

void fs_png_write(png_structp writep, png_bytep data, png_size_t length)
{
    fs_write(data, length, png_get_io_ptr(writep));
}

void fs_png_flush(png_structp writep)
{
    fs_flush(png_get_io_ptr(writep));
}
========
>>>>>>> origin/csy-extras
#ifndef VIDEO_H
#define VIDEO_H

/*---------------------------------------------------------------------------*/

<<<<<<< HEAD
struct video
{
    int device_w, device_h;
    int window_w, window_h;

    float device_scale;
};

extern struct video video;

int  video_init(void);
void video_quit(void);
=======
int video_init(const char *, const char *);
>>>>>>> origin/csy-extras

/*---------------------------------------------------------------------------*/

int  video_mode(int, int, int);

<<<<<<< HEAD
void video_snap(const char *);
int  video_perf(void);
void video_swap(void);

void video_show_cursor(void);
void video_hide_cursor(void);

void video_set_grab(int w);
void video_clr_grab(void);
int  video_get_grab(void);

int  video_fullscreen(int);

void video_resize(int, int);

int  video_display(void);

void video_set_window_size(int w, int h);

/*---------------------------------------------------------------------------*/

void video_calc_view(float *, const float *,
                              const float *,
                              const float *);

void video_push_persp(float, float, float);
void video_push_persp_ex(float, float, float, int, int, int, int);
=======
int  video_perf(void);
void video_swap(void);

void video_set_grab(int w);
void video_clr_grab(void);
int  video_get_grab(void);
>>>>>>>> origin/csy-extras:share/video.h

/*---------------------------------------------------------------------------*/

void video_push_persp(float, float, float);
>>>>>>> origin/csy-extras
void video_push_ortho(void);
void video_pop_matrix(void);
void video_clear(void);

/*---------------------------------------------------------------------------*/

#endif
