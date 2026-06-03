/*
 * Party Mode Menu State
 */

#include <stdio.h>

#include "gui.h"
#include "transition.h"
#include "audio.h"
#include "config.h"
#include "progress.h"
#include "set.h"
#include "st_party.h"
#include "st_title.h"
#include "st_play.h"
#include "st_shared.h"
#include "game_common.h"
#include "game_server.h"

enum {
    PARTY_MODE = GUI_LAST,
    PARTY_PLAYERS,
    PARTY_CPU,
    PARTY_PHYSICS,
    PARTY_START
};

static int party_mode = MODE_TARGET;
static int party_players = 1;
static int party_cpu = 0;
static int party_physics = 0;

static int mode_id;
static int player_id;
static int cpu_id;
static int physics_id;
static int desc_id;

static const char *get_mode_label(int m) {
    if (m == MODE_TARGET)    return "Mode: Monkey Target";
    if (m == MODE_BATTLE)    return "Mode: Battle (Shared)";
    if (m == MODE_FIGHT)     return "Mode: Monkey Fight";
    if (m == MODE_BILLIARDS) return "Mode: Monkey Billiards";
    if (m == MODE_BOWLING)   return "Mode: Monkey Bowling";
    if (m == MODE_GOLF)      return "Mode: Monkey Golf";
    if (m == MODE_BOAT)      return "Mode: Monkey Boat";
    if (m == MODE_SHOT)      return "Mode: Monkey Shot";
    if (m == MODE_SOCCER)    return "Mode: Monkey Soccer";
    if (m == MODE_TENNIS)    return "Mode: Monkey Tennis";
    if (m == MODE_BASEBALL)  return "Mode: Monkey Baseball";
    if (m == MODE_MOLE)      return "Mode: Whack-a-Mole";
    if (m == MODE_HAMMER)    return "Mode: Hammer Throw";
    if (m == MODE_DOGFIGHT)  return "Mode: Monkey Dogfight";
    return "Mode: Unknown";
}

static const char *get_mode_desc(int m) {
    if (m == MODE_TARGET)
        return "Fly your monkey ball to the target!\n"
               "Tilt to steer, Action (A) to toggle wings.\n"
               "Pitch Up/Down to control speed and altitude.\n"
               "Land in the center for max points!";
    if (m == MODE_BATTLE)
        return "Race against friends in a shared world.\n"
               "Be the first to the goal!\n"
               "Collect items to zap opponents.";
    if (m == MODE_FIGHT)
        return "Knock everyone else off the stage!\n"
               "Press Action (A) to Punch.\n"
               "Last monkey rolling wins.";
    if (m == MODE_BILLIARDS)
        return "Sink all the balls to win.\n"
               "Hold Action (A) to charge power, release to shoot.\n"
               "Don't scratch the cue ball!";
    if (m == MODE_BOWLING)
        return "Ten-pin bowling!\n"
               "1. Position (Left/Right + A)\n"
               "2. Aim (A)\n"
               "3. Power (A)";
    if (m == MODE_GOLF)
        return "Minigolf on Monkey Ball levels!\n"
               "1. Aim with Camera.\n"
               "2. Hold (A) to Charge.\n"
               "3. Release to Putt.";
    if (m == MODE_BOAT)
        return "Row your boat down the river!\n"
               "Alternate Left/Right (L1/R1) to paddle.\n"
               "Press both to brake.\n"
               "Reach the goal before time runs out!";
    if (m == MODE_SHOT)
        return "FPS Rail Shooter!\n"
               "Move Cursor with Stick.\n"
               "Shoot with (A).\n"
               "Reload with (B).";
    if (m == MODE_SOCCER)
        return "Score goals in a giant soccer match!\n"
               "Red Team vs Blue Team.\n"
               "Push the giant ball into the opponent's goal.\n"
               "First to 3 goals wins!";
    if (m == MODE_TENNIS)
        return "1v1 or 2v2 Tennis Match!\n"
               "Use Action (A) to swing your racket.\n"
               "Hit the ball over the net.\n"
               "Standard scoring applies.";
    if (m == MODE_BASEBALL)
        return "Home Run Derby!\n"
               "Pitcher: Aim with Stick, Throw with A.\n"
               "Batter: Time your swing (A) to hit the ball.\n"
               "Score points for distance!";
    if (m == MODE_MOLE)
        return "Whack the Moles!\n"
               "Roll over the moles as they pop up to score points.\n"
               "Collect the most points before time runs out.";
    if (m == MODE_HAMMER)
        return "Hammer Throw!\n"
               "Tilt Left/Right to build up spin power.\n"
               "Press Action (A) to release the throw.\n"
               "Score points based on the distance thrown.";
    if (m == MODE_DOGFIGHT)
        return "Aerial Combat!\n"
               "Steer your glider and shoot down opponents.\n"
               "Press Action (A) to fire machine gun.\n"
               "100 damage destroys a target!";
    return "";
}

static void update_labels(void) {
    char buf[32];

    gui_set_label(mode_id, get_mode_label(party_mode));
    gui_set_multi(desc_id, get_mode_desc(party_mode));

    sprintf(buf, "Players: %d", party_players);
    gui_set_label(player_id, buf);

    sprintf(buf, "CPUs: %d", party_cpu);
    gui_set_label(cpu_id, buf);

    gui_set_label(physics_id, party_physics ? "Physics: Arcade" : "Physics: Normal");
}

static int party_action(int tok, int val) {
    switch (tok) {
        case GUI_BACK:
            return goto_state(&st_title);

        case PARTY_MODE:
            if (party_mode == MODE_TARGET)         party_mode = MODE_BATTLE;
            else if (party_mode == MODE_BATTLE)    party_mode = MODE_FIGHT;
            else if (party_mode == MODE_FIGHT)     party_mode = MODE_BILLIARDS;
            else if (party_mode == MODE_BILLIARDS) party_mode = MODE_BOWLING;
            else if (party_mode == MODE_BOWLING)   party_mode = MODE_GOLF;
            else if (party_mode == MODE_GOLF)      party_mode = MODE_BOAT;
            else if (party_mode == MODE_BOAT)      party_mode = MODE_SHOT;
            else if (party_mode == MODE_SHOT)      party_mode = MODE_SOCCER;
            else if (party_mode == MODE_SOCCER)    party_mode = MODE_TENNIS;
            else if (party_mode == MODE_TENNIS)    party_mode = MODE_BASEBALL;
            else if (party_mode == MODE_BASEBALL)  party_mode = MODE_MOLE;
            else if (party_mode == MODE_MOLE)      party_mode = MODE_HAMMER;
            else if (party_mode == MODE_HAMMER)    party_mode = MODE_DOGFIGHT;
            else if (party_mode == MODE_DOGFIGHT)  party_mode = MODE_TARGET;
            update_labels();
            break;

        case PARTY_PLAYERS:
            party_players++;
            if (party_players > 4) party_players = 1;
            /* Clamp CPUs if total > 4 */
            if (party_players + party_cpu > 4) party_cpu = 4 - party_players;
            update_labels();
            break;

        case PARTY_CPU:
            party_cpu++;
            if (party_players + party_cpu > 4) party_cpu = 0;
            update_labels();
            break;

        case PARTY_PHYSICS:
            party_physics = !party_physics;
            update_labels();
            break;

        case PARTY_START:
            config_set_d(CONFIG_MULTIBALL, party_players + party_cpu);
            /* HACK: We need to pass CPU count to game_server. */
            /* We can store it in a config variable or global. */
            /* Let's reuse CONFIG_MULTIBALL for total and add logic to set CPU flags */
            /* Actually, we need to pass this info to game_server_init */
            /* But progress_init -> init_level -> game_server_init flow doesn't take CPU args easily without global state or config. */
            /* Let's use a new temporary config or just a global in game_common.c */
            /* Better: config_set_d(CONFIG_CPU_COUNT, party_cpu); */
            /* We don't have CONFIG_CPU_COUNT. Let's use a static variable in game_server.c exposed via function? */
            game_set_cpu_count(party_cpu);

            config_set_d(CONFIG_PHYSICS, party_physics);
            progress_init(party_mode);

            /* Initialize sets if not done (st_set usually does this) */
            /* set_init returns total sets. */
            if (set_init() > 0) {
                set_goto(0); /* Easy Set */
                if (progress_play(get_level(0))) {
                    return goto_state(&st_play_ready);
                }
            }
            break;
    }
    return 1;
}

static int party_gui(void) {
    int root;
    if ((root = gui_vstack(0))) {
        gui_label(root, "Party Games", GUI_LRG, gui_yel, gui_red);
        gui_space(root);

        mode_id = gui_state(root, get_mode_label(party_mode), GUI_MED, PARTY_MODE, 0);

        desc_id = gui_multi(root, get_mode_desc(party_mode), GUI_SML, gui_wht, gui_wht);

        player_id = gui_state(root, "Players: 1", GUI_MED, PARTY_PLAYERS, 0);
        cpu_id    = gui_state(root, "CPUs: 0",    GUI_MED, PARTY_CPU,     0);
        physics_id = gui_state(root, "Physics: Normal", GUI_MED, PARTY_PHYSICS, 0);

        gui_space(root);
        gui_state(root, "Start Game", GUI_MED, PARTY_START, 0);
        gui_state(root, "Back", GUI_MED, GUI_BACK, 0);

        gui_layout(root, 0, 0);
        update_labels();
    }
    return root;
}

static int party_enter(struct state *st, struct state *prev, int intent) {
    audio_music_fade_to(0.5f, "bgm/inter.ogg");
    return transition_slide(party_gui(), 1, intent);
}

static int party_leave(struct state *st, struct state *next, int id, int intent) {
    return transition_slide(id, 0, intent);
}

static int party_click(int b, int d) {
    if (gui_click(b, d))
        return party_action(gui_token(gui_active()), gui_value(gui_active()));
    return 1;
}

static int party_buttn(int b, int d, int device_id) {
    if (d) {
        int active = gui_active();
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return party_action(gui_token(active), gui_value(active));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return party_action(GUI_BACK, 0);
    }
    return 1;
}

static int party_keybd(int c, int d) {
    if (d) {
        if (c == 27) /* KEY_EXIT / ESC */
            return party_action(GUI_BACK, 0);
    }
    return 1;
}

/* Use shared handlers for paint/stick/etc */
struct state st_party = {
    party_enter,
    party_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    party_click,
    party_keybd,
    party_buttn
};
