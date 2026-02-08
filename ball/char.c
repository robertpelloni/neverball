#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "char.h"
#include "stats.h"
#include "fs.h"
#include "common.h"
#include "config.h"
#include "profile.h"

static Array chars;

static int is_ball_dir(struct dir_item *item)
{
    /* For now, assume everything in data/ball is a potential ball dir.
       fs_dir_scan generally only returns files/directories that exist.
       The directory structure is handled by PhysFS usually. */
    /* We can't easily check is_dir here because dir_item struct doesn't seem to have it exposed
       or we need to look closer at dir.h/dir.c. */
    /* Looking at dir.h, dir_item only has path and data. */
    /* So we probably shouldn't rely on is_dir. */

    return 1;
}

void char_init(void)
{
    Array items;
    int i;

    chars = array_new(sizeof(struct char_def));

    /* Scan data/ball directory */
    if ((items = fs_dir_scan("ball", is_ball_dir)))
    {
        for (i = 0; i < array_len(items); i++)
        {
            struct dir_item *item = DIR_ITEM_GET(items, i);
            struct char_def *def;
            struct character_stats stats;
            char full_path[MAXSTR];

            /* Filter out non-directories or common files manually if needed */
            if (str_ends_with(item->path, ".txt") || str_ends_with(item->path, ".png"))
                continue;

            snprintf(full_path, sizeof(full_path), "ball/%s/%s", item->path, item->path);

            /* Load stats to get name and cost */
            stats_load(&stats, full_path);

            def = array_add(chars);

            def->path = strdup(full_path);
            def->name = strdup(stats.name);
            def->cost = stats.cost;

            /* Check unlock status (TODO: Load from profile) */
            def->unlocked = (def->cost == 0);
        }
        fs_dir_free(items);
    }
}

void char_quit(void)
{
    int i;
    for (i = 0; i < array_len(chars); i++)
    {
        struct char_def *def = (struct char_def *) array_get(chars, i);
        free(def->path);
        free(def->name);
    }
    array_free(chars);
}

int char_count(void)
{
    return array_len(chars);
}

struct char_def *char_get(int index)
{
    return (struct char_def *) array_get(chars, index);
}

int char_get_current(void)
{
    const char *curr = config_get_s(CONFIG_BALL_FILE);
    int i;
    for (i = 0; i < array_len(chars); i++)
    {
        struct char_def *def = (struct char_def *) array_get(chars, i);
        if (strcmp(def->path, curr) == 0)
            return i;
    }
    return 0;
}

void char_set_current(int index)
{
    struct char_def *def = char_get(index);
    if (def)
    {
        config_set_s(CONFIG_BALL_FILE, def->path);
    }
}

int char_buy(int index)
{
    struct char_def *def = char_get(index);
    if (def && !def->unlocked)
    {
        int money = profile_get_currency();
        if (money >= def->cost)
        {
            profile_add_currency(-def->cost);
            def->unlocked = 1;
            /* TODO: Save unlock status to profile */
            return 1;
        }
    }
    return 0;
}
