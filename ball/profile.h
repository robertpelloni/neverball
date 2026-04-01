#ifndef PROFILE_H
#define PROFILE_H

enum {
    STAT_JUMPS,
    STAT_DISTANCE,
    STAT_WINS_RACE,
    STAT_WINS_FIGHT,
    STAT_WINS_TARGET,
    STAT_COINS,
    STAT_PLAYTIME,
    STAT_MAX = 2048
};

void profile_init(void);
void profile_save(void);

int  profile_get_currency(void);
void profile_add_currency(int amount);

int  profile_get_stat(int stat_id);
void profile_add_stat(int stat_id, int amount);

int  profile_is_achieved(int id);
void profile_set_achieved(int id);

#endif
