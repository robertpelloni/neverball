#ifndef ST_STORY_H
#define ST_STORY_H

#include "state.h"

extern struct state st_story;

void story_set(const char *image, const char *text, struct state *next);
void story_set_hub(const char *img, const char *txt, int warp_id);
int  story_load_script(const char *filename, struct state *nxt);

#endif
