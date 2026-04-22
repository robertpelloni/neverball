/*
 * Shop / Unlockables State
 */

#include <stdio.h>

#include "gui.h"
#include "transition.h"
#include "audio.h"
#include "game_common.h"
#include "config.h"
#include "profile.h"
#include "achieve.h"
#include "st_shop.h"
#include "st_title.h"
#include "st_shared.h"

enum {
    SHOP_ITEM_0 = GUI_LAST,
    SHOP_ITEM_1,
    SHOP_ITEM_2,
    SHOP_ITEM_3,
    SHOP_ITEM_4
};

struct shop_item {
    const char *name;
    const char *desc;
    int cost;
    int achieved_id;
};

static struct shop_item items[] = {
    {"Gold Monkey", "Unlock the Gold Monkey character.", 1000, ACH_UNLOCK_GOLD},
    {"Master Mode", "Unlock Master difficulty levels.", 5000, ACH_UNLOCK_MASTER},
    {"8-Ball Decal", "A cool 8-ball texture for your monkey.", 500, ACH_UNLOCK_8BALL},
    {"Bonus Soundrack", "Unlock retro BGM tracks.", 2500, ACH_UNLOCK_BGM},
    {"Concept Art", "View original concept sketches.", 100, ACH_UNLOCK_ART}
};

static int NUM_ITEMS = sizeof(items) / sizeof(items[0]);
static int selected_item = 0;
static int desc_id;
static int balance_id;
static int item_ids[5];

static void update_shop_ui(void) {
    char buf[128];
    sprintf(buf, "Balance: %d Coins", profile_get_currency());
    gui_set_label(balance_id, buf);

    gui_set_multi(desc_id, items[selected_item].desc);

    int i;
    for (i = 0; i < NUM_ITEMS; i++) {
        if (profile_is_achieved(items[i].achieved_id)) {
            gui_set_label(item_ids[i], "SOLD OUT");
            gui_set_color(item_ids[i], gui_gry, gui_gry);
        } else {
            sprintf(buf, "%s - %d", items[i].name, items[i].cost);
            gui_set_label(item_ids[i], buf);
            if (profile_get_currency() >= items[i].cost)
                gui_set_color(item_ids[i], gui_wht, gui_grn);
            else
                gui_set_color(item_ids[i], gui_gry, gui_red);
        }
    }
}

static int shop_action(int tok, int val) {
    switch (tok) {
        case GUI_BACK:
            return goto_state(&st_title);

        case SHOP_ITEM_0:
        case SHOP_ITEM_1:
        case SHOP_ITEM_2:
        case SHOP_ITEM_3:
        case SHOP_ITEM_4:
            selected_item = tok - SHOP_ITEM_0;
            update_shop_ui();

            if (!profile_is_achieved(items[selected_item].achieved_id)) {
                if (profile_get_currency() >= items[selected_item].cost) {
                    /* Buy it */
                    profile_add_currency(-items[selected_item].cost);
                    profile_set_achieved(items[selected_item].achieved_id);
                    profile_save();
                    audio_play(AUD_GOAL, 1.0f); /* Cha-ching! */
                    update_shop_ui();
                } else {
                    audio_play(AUD_FALL, 1.0f); /* Error / Not enough funds */
                }
            } else {
                audio_play(AUD_MENU, 1.0f);
            }
            break;
    }
    return 1;
}

static int shop_gui(void) {
    int root;
    if ((root = gui_vstack(0))) {
        gui_label(root, "Monkey Shop", GUI_LRG, gui_yel, gui_red);

        balance_id = gui_label(root, "Balance: 0", GUI_MED, gui_grn, gui_grn);
        gui_space(root);

        int i;
        for (i = 0; i < NUM_ITEMS; i++) {
            item_ids[i] = gui_state(root, "Item", GUI_SML, SHOP_ITEM_0 + i, 0);
        }

        gui_space(root);
        desc_id = gui_multi(root, "Description", GUI_SML, gui_wht, gui_wht);

        gui_space(root);
        gui_state(root, "Back", GUI_MED, GUI_BACK, 0);

        gui_layout(root, 0, 0);
        update_shop_ui();
    }
    return root;
}

static int shop_enter(struct state *st, struct state *prev, int intent) {
    audio_music_fade_to(0.5f, "bgm/inter.ogg");
    selected_item = 0;
    return transition_slide(shop_gui(), 1, intent);
}

static int shop_leave(struct state *st, struct state *next, int id, int intent) {
    return transition_slide(id, 0, intent);
}

static int shop_click(int b, int d) {
    if (gui_click(b, d))
        return shop_action(gui_token(gui_active()), gui_value(gui_active()));
    return 1;
}

static int shop_buttn(int b, int d, int device_id) {
    if (d) {
        int active = gui_active();
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return shop_action(gui_token(active), gui_value(active));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return shop_action(GUI_BACK, 0);
    }
    return 1;
}

static int shop_keybd(int c, int d) {
    if (d) {
        if (c == 27) /* KEY_EXIT / ESC */
            return shop_action(GUI_BACK, 0);
    }
    return 1;
}

struct state st_shop = {
    shop_enter,
    shop_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shop_click,
    shop_keybd,
    shop_buttn
};
