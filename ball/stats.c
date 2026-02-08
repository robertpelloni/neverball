#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "stats.h"
#include "fs.h"
#include "common.h"

void stats_default(struct character_stats *stats)
{
    stats->speed = 1.0f;
    stats->acceleration = 1.0f;
    stats->jump = 1.0f;
    stats->control = 1.0f;
    stats->cost = 0;
    strcpy(stats->name, "Unknown");
}

int stats_load(struct character_stats *stats, const char *ball_name)
{
    char path[MAXSTR];
    fs_file fh;
    char *line;

    stats_default(stats);
    strcpy(stats->name, ball_name); /* Fallback name */

    /* Construct path: data/ball/<name>/stats.txt */
    /* Note: ball_name usually is "ball/name/name" or "ball/name".
       Let's assume the argument is the full path to the ball sol prefix,
       e.g. "ball/basic-ball/basic-ball".
       We need to find the directory. */

    /* Actually, let's look at how config stores it. "ball/basic-ball/basic-ball".
       The stats file should probably be "ball/basic-ball/stats.txt". */

    char *dir = strdup(ball_name);
    char *last_sep = strrchr(dir, '/');
    if (last_sep) *last_sep = 0;

    snprintf(path, sizeof(path), "%s/stats.txt", dir);
    free(dir);

    if ((fh = fs_open_read(path)))
    {
        while (read_line(&line, fh))
        {
            char key[64];
            char val_s[64];

            if (sscanf(line, "%s %s", key, val_s) == 2)
            {
                if (strcmp(key, "name") == 0) {
                    /* Read rest of line as name */
                    char *p = strstr(line, "name");
                    if (p) {
                        p += 4;
                        while (*p == ' ' || *p == '\t') p++;
                        SAFECPY(stats->name, p);
                    }
                }
                else if (strcmp(key, "speed") == 0) stats->speed = atof(val_s);
                else if (strcmp(key, "accel") == 0) stats->acceleration = atof(val_s);
                else if (strcmp(key, "jump") == 0) stats->jump = atof(val_s);
                else if (strcmp(key, "control") == 0) stats->control = atof(val_s);
                else if (strcmp(key, "cost") == 0) stats->cost = atoi(val_s);
            }
            free(line);
        }
        fs_close(fh);
        return 1;
    }
    return 0;
}
