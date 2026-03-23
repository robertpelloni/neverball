/*
<<<<<<< HEAD
<<<<<<<< HEAD:share/fs_png.c
 * Copyright (C) 2003-2010 Neverball authors
========
 * Copyright (C) 2003 Robert Kooima
>>>>>>>> origin/csy-extras:share/video.h
=======
 * Copyright (C) 2003-2010 Neverball authors
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
<<<<<<<< HEAD:share/fs_png.c
=======
>>>>>>> origin/csy-extras
#include <png.h>
#include <string.h>
#include "fs_png.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

void fs_png_read(png_structp readp, png_bytep data, png_size_t length)
{
<<<<<<< HEAD
    int read = fs_read(data, length, png_get_io_ptr(readp));
=======
    int read = fs_read(data, 1, length, png_get_io_ptr(readp));
>>>>>>> origin/csy-extras

    if (read < length)
        memset(data + read, 0, length - read);
}

void fs_png_write(png_structp writep, png_bytep data, png_size_t length)
{
<<<<<<< HEAD
    fs_write(data, length, png_get_io_ptr(writep));
=======
    fs_write(data, 1, length, png_get_io_ptr(writep));
>>>>>>> origin/csy-extras
}

void fs_png_flush(png_structp writep)
{
    fs_flush(png_get_io_ptr(writep));
}
<<<<<<< HEAD
========
#ifndef VIDEO_H
#define VIDEO_H

/*---------------------------------------------------------------------------*/

int video_init(const char *, const char *);

/*---------------------------------------------------------------------------*/

int  video_mode(int, int, int);

int  video_perf(void);
void video_swap(void);

void video_set_grab(int w);
void video_clr_grab(void);
int  video_get_grab(void);
>>>>>>>> origin/csy-extras:share/video.h

/*---------------------------------------------------------------------------*/

void video_push_persp(float, float, float);
void video_push_ortho(void);
void video_pop_matrix(void);
void video_clear(void);

/*---------------------------------------------------------------------------*/

#endif
=======

/*---------------------------------------------------------------------------*/
>>>>>>> origin/csy-extras
