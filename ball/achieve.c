#include "achieve.h"
#include "profile.h"
#include "hud.h"
#include "lang.h"

struct achievement_def {
    int id;
    const char *name;
    const char *desc;
};

static const struct achievement_def ach_defs[ACH_MAX] = {
    { ACH_FIRST_JUMP, "First Hop", "Perform your first jump." },
    { ACH_JUMP_MASTER, "Bounce Master", "Jump 100 times." },
    { ACH_MARATHON, "Marathon Monkey", "Travel a long distance." },
    { ACH_RICH_MONKEY, "Rich Monkey", "Collect 1000 coins." },
    { ACH_FIRST_WIN, "First Victory", "Win a Race or Target game." },
    { ACH_TARGET_MASTER, "Target Master", "Win 10 Target games." },
    { ACH_FIGHT_CLUB, "Fight Club", "Win a Fight game." }
};

void achieve_init(void) {
    /* Nothing needed here as profile handles persistence */
}

void achieve_check(void) {
    if (!profile_is_achieved(ACH_FIRST_JUMP)) {
        if (profile_get_stat(STAT_JUMPS) >= 1) {
            profile_set_achieved(ACH_FIRST_JUMP);
            hud_show_toast(_(ach_defs[ACH_FIRST_JUMP].name));
        }
    }
    if (!profile_is_achieved(ACH_JUMP_MASTER)) {
        if (profile_get_stat(STAT_JUMPS) >= 100) {
            profile_set_achieved(ACH_JUMP_MASTER);
            hud_show_toast(_(ach_defs[ACH_JUMP_MASTER].name));
        }
    }
    if (!profile_is_achieved(ACH_MARATHON)) {
        if (profile_get_stat(STAT_DISTANCE) >= 10000) {
            profile_set_achieved(ACH_MARATHON);
            hud_show_toast(_(ach_defs[ACH_MARATHON].name));
        }
    }
    if (!profile_is_achieved(ACH_RICH_MONKEY)) {
        if (profile_get_stat(STAT_COINS) >= 1000) {
            profile_set_achieved(ACH_RICH_MONKEY);
            hud_show_toast(_(ach_defs[ACH_RICH_MONKEY].name));
        }
    }
    if (!profile_is_achieved(ACH_FIRST_WIN)) {
        if (profile_get_stat(STAT_WINS_RACE) >= 1 || profile_get_stat(STAT_WINS_TARGET) >= 1) {
            profile_set_achieved(ACH_FIRST_WIN);
            hud_show_toast(_(ach_defs[ACH_FIRST_WIN].name));
        }
    }
    if (!profile_is_achieved(ACH_TARGET_MASTER)) {
        if (profile_get_stat(STAT_WINS_TARGET) >= 10) {
            profile_set_achieved(ACH_TARGET_MASTER);
            hud_show_toast(_(ach_defs[ACH_TARGET_MASTER].name));
        }
    }
    if (!profile_is_achieved(ACH_FIGHT_CLUB)) {
        if (profile_get_stat(STAT_WINS_FIGHT) >= 1) {
            profile_set_achieved(ACH_FIGHT_CLUB);
            hud_show_toast(_(ach_defs[ACH_FIGHT_CLUB].name));
        }
    }
}

int achieve_unlocked(int id) {
    return profile_is_achieved(id);
}

const char *achieve_name(int id) {
    if (id >= 0 && id < ACH_MAX) return ach_defs[id].name;
    return "";
}

const char *achieve_desc(int id) {
    if (id >= 0 && id < ACH_MAX) return ach_defs[id].desc;
    return "";
}
