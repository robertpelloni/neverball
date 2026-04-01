#ifndef ACHIEVE_H
#define ACHIEVE_H

enum {
    ACH_FIRST_JUMP,
    ACH_JUMP_MASTER,
    ACH_MARATHON,
    ACH_RICH_MONKEY,
    ACH_FIRST_WIN,
    ACH_TARGET_MASTER,
    ACH_FIGHT_CLUB,
    ACH_UNLOCK_GOLD = 100,
    ACH_UNLOCK_MASTER,
    ACH_UNLOCK_8BALL,
    ACH_UNLOCK_BGM,
    ACH_UNLOCK_ART,
    ACH_MAX = 256
};

void achieve_init(void);
void achieve_check(void);
int  achieve_unlocked(int id);

const char *achieve_name(int id);
const char *achieve_desc(int id);

#endif
