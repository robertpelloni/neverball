#ifndef CHAR_H
#define CHAR_H

#include "array.h"

struct char_def {
    char *path; /* e.g. "ball/basic-ball/basic-ball" */
    char *name; /* Display Name */
    int   cost;
    int   unlocked;
};

void char_init(void);
void char_quit(void);

int char_count(void);
struct char_def *char_get(int index);

/* Returns index of currently selected character */
int char_get_current(void);
void char_set_current(int index);

/* Unlock a character if affordable */
int char_buy(int index);

#endif
