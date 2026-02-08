/*
 * Character Selection Screen
 */

#include "gui.h"
#include "transition.h"
#include "audio.h"
#include "char.h"
#include "stats.h"
#include "st_shared.h"
#include "st_title.h"
#include "st_set.h"
#include "profile.h"
#include "config.h"
#include "key.h"

extern struct state st_char;

enum {
    CHAR_NEXT = GUI_LAST,
    CHAR_PREV,
    CHAR_SELECT,
    CHAR_BUY
};

static int char_index = 0;
static int name_id, cost_id, money_id, buy_id, sel_id;
static int stat_spd, stat_acc, stat_jmp, stat_ctrl;

static void update_gui(void)
{
    struct char_def *def = char_get(char_index);
    struct character_stats stats;
    char buf[64];

    stats_load(&stats, def->path);

    gui_set_label(name_id, def->name);

    sprintf(buf, "Speed: %.1f", stats.speed);
    gui_set_label(stat_spd, buf);
    sprintf(buf, "Accel: %.1f", stats.acceleration);
    gui_set_label(stat_acc, buf);
    sprintf(buf, "Jump:  %.1f", stats.jump);
    gui_set_label(stat_jmp, buf);
    sprintf(buf, "Turn:  %.1f", stats.control);
    gui_set_label(stat_ctrl, buf);

    sprintf(buf, "Bananas: %d", profile_get_currency());
    gui_set_label(money_id, buf);

    if (def->unlocked)
    {
        gui_set_hidden(buy_id, 1);
        gui_set_hidden(sel_id, 0);
        gui_set_label(cost_id, "Owned");
    }
    else
    {
        gui_set_hidden(buy_id, 0);
        gui_set_hidden(sel_id, 1);
        sprintf(buf, "Cost: %d", def->cost);
        gui_set_label(cost_id, buf);
    }
}

static int char_action(int tok, int val)
{
    switch (tok)
    {
        case GUI_BACK:
            return goto_state(&st_title);

        case CHAR_NEXT:
            char_index = (char_index + 1) % char_count();
            update_gui();
            break;

        case CHAR_PREV:
            char_index = (char_index - 1 + char_count()) % char_count();
            update_gui();
            break;

        case CHAR_SELECT:
            char_set_current(char_index);
            return goto_state(&st_set);

        case CHAR_BUY:
            if (char_buy(char_index))
            {
                audio_play(AUD_COIN, 1.0f);
                update_gui();
            }
            else
            {
                audio_play(AUD_FAIL, 1.0f);
            }
            break;
    }
    return 1;
}

static int char_gui(void)
{
    int root, id;

    if ((root = gui_vstack(0)))
    {
        gui_label(root, "Select Character", GUI_LRG, gui_yel, gui_red);
        gui_space(root);

        /* Navigation */
        if ((id = gui_hstack(root)))
        {
            gui_state(id, "<", GUI_LRG, CHAR_PREV, 0);
            name_id = gui_label(id, "Unknown", GUI_LRG, gui_wht, gui_wht);
            gui_state(id, ">", GUI_LRG, CHAR_NEXT, 0);
        }

        gui_space(root);

        /* Stats */
        stat_spd = gui_label(root, "Speed: ?", GUI_MED, gui_wht, gui_wht);
        stat_acc = gui_label(root, "Accel: ?", GUI_MED, gui_wht, gui_wht);
        stat_jmp = gui_label(root, "Jump:  ?", GUI_MED, gui_wht, gui_wht);
        stat_ctrl = gui_label(root, "Turn:  ?", GUI_MED, gui_wht, gui_wht);

        gui_space(root);
        cost_id = gui_label(root, "Cost: 0", GUI_MED, gui_yel, gui_red);
        money_id = gui_label(root, "Bananas: 0", GUI_MED, gui_yel, gui_red);

        gui_space(root);

        /* Action Buttons */
        sel_id = gui_state(root, "Select", GUI_MED, CHAR_SELECT, 0);
        buy_id = gui_state(root, "Buy",    GUI_MED, CHAR_BUY,    0);

        gui_space(root);
        gui_state(root, "Back", GUI_MED, GUI_BACK, 0);

        gui_layout(root, 0, 0);
        update_gui();
    }
    return root;
}

static int char_enter(struct state *st, struct state *prev, int intent)
{
    if (char_count() == 0) char_init();
    char_index = char_get_current();
    return transition_slide(char_gui(), 1, intent);
}

static int char_leave(struct state *st, struct state *next, int id, int intent)
{
    return transition_slide(id, 0, intent);
}

static int char_click(int b, int d)
{
    if (gui_click(b, d))
        return char_action(gui_token(gui_active()), gui_value(gui_active()));
    return 1;
}

static int char_buttn(int b, int d, int device_id)
{
    if (d) {
        int active = gui_active();
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return char_action(gui_token(active), gui_value(active));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return char_action(GUI_BACK, 0);
        if (config_tst_d(CONFIG_JOYSTICK_DPAD_L, b) || config_tst_d(CONFIG_KEY_LEFT, b))
            return char_action(CHAR_PREV, 0);
        if (config_tst_d(CONFIG_JOYSTICK_DPAD_R, b) || config_tst_d(CONFIG_KEY_RIGHT, b))
            return char_action(CHAR_NEXT, 0);
    }
    return 1;
}

static int char_keybd(int c, int d)
{
    if (d) {
        if (c == 27) return char_action(GUI_BACK, 0);
        if (c == SDLK_LEFT) return char_action(CHAR_PREV, 0);
        if (c == SDLK_RIGHT) return char_action(CHAR_NEXT, 0);
    }
    return 1;
}

struct state st_char = {
    char_enter,
    char_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    char_click,
    char_keybd,
    char_buttn
};
