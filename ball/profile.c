#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fs.h"
#include "common.h"
#include "profile.h"

#define USER_PROFILE_FILE "profile"

static int currency = 0;
static int dirty = 0;

void profile_init(void)
{
    fs_file fh;
    char *line;

    currency = 0;

    if ((fh = fs_open_read(USER_PROFILE_FILE)))
    {
        while (read_line(&line, fh))
        {
            char key[MAXSTR];
            int val;

            if (sscanf(line, "%s %d", key, &val) == 2)
            {
                if (strcmp(key, "currency") == 0)
                    currency = val;
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
        if ((fh = fs_open_write(USER_PROFILE_FILE)))
        {
            fs_printf(fh, "currency %d\n", currency);
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
