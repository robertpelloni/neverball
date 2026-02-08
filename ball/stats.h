#ifndef STATS_H
#define STATS_H

struct character_stats
{
    float speed;        /* Max speed multiplier */
    float acceleration; /* Acceleration multiplier */
    float jump;         /* Jump force multiplier */
    float control;      /* Handling/Turn speed */
    int   cost;         /* Unlock cost in Bananas */
    char  name[64];     /* Display name */
};

/* Load stats from a file (e.g., data/ball/<name>/stats.txt) */
int stats_load(struct character_stats *stats, const char *ball_name);

/* Get default stats */
void stats_default(struct character_stats *stats);

#endif
