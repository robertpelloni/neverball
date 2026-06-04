#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "common.h"
#include "profile.h"
#include "achieve.h"

#define USER_PROFILE_FILE "profile"

static int currency = 0;
static int worlds[32]; /* Bitmask or max level per world? Using unlocked count per world */
static int stats[STAT_MAX];
static int achievements[ACH_MAX];
static int dirty = 0;

void profile_init(void)
{
    fs_file fh;
    char *line;
    int i;

    currency = 0;
    for(i=0; i<32; i++) worlds[i] = 0;
    for(i=0; i<STAT_MAX; i++) stats[i] = 0;
    for(i=0; i<ACH_MAX; i++) achievements[i] = 0;

    if ((fh = fs_open_read(USER_PROFILE_FILE)))
    {
        while (read_line(&line, fh))
        {
            char key[MAXSTR];
            int val, idx;

            if (sscanf(line, "currency %d", &val) == 1)
            {
                currency = val;
            }
            else if (sscanf(line, "world_%d %d", &idx, &val) == 2)
            {
                if (idx >= 0 && idx < 32)
                    worlds[idx] = val;
            }
            else if (sscanf(line, "stat_%d %d", &idx, &val) == 2)
            {
                if (idx >= 0 && idx < STAT_MAX)
                    stats[idx] = val;
            }
            else if (sscanf(line, "achieve_%d %d", &idx, &val) == 2)
            {
                if (idx >= 0 && idx < ACH_MAX)
                    achievements[idx] = val;
            }
            free(line);
        }
        fs_close(fh);
    }
    dirty = 0;
}

void profile_save(void)
{
    if (dirty)
    {
        fs_file fh;
        int i;
        if ((fh = fs_open_write(USER_PROFILE_FILE)))
        {
            fs_printf(fh, "currency %d\n", currency);
            for(i=0; i<32; i++) {
                if (worlds[i] > 0)
                    fs_printf(fh, "world_%d %d\n", i, worlds[i]);
            }
            for(i=0; i<STAT_MAX; i++) {
                if (stats[i] > 0)
                    fs_printf(fh, "stat_%d %d\n", i, stats[i]);
            }
            for(i=0; i<ACH_MAX; i++) {
                if (achievements[i])
                    fs_printf(fh, "achieve_%d 1\n", i);
            }
            fs_close(fh);
            dirty = 0;
        }
    }
}

int profile_get_currency(void)
{
    return currency;
}

void profile_add_currency(int amount)
{
    if (amount != 0)
    {
        currency += amount;
        if (currency < 0) currency = 0;
        dirty = 1;
    }
}

int profile_get_stat(int stat_id)
{
    if (stat_id >= 0 && stat_id < STAT_MAX)
        return stats[stat_id];
    return 0;
}

void profile_add_stat(int stat_id, int amount)
{
    if (stat_id >= 0 && stat_id < STAT_MAX && amount != 0)
    {
        stats[stat_id] += amount;
        dirty = 1;
        achieve_check();
    }
}

int profile_is_achieved(int id)
{
    if (id >= 0 && id < ACH_MAX) return achievements[id];
    return 0;
}

void profile_set_achieved(int id)
{
    if (id >= 0 && id < ACH_MAX && !achievements[id]) {
        achievements[id] = 1;
        dirty = 1;
    }
}
