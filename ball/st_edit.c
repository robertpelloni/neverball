/*
 * Level Editor GUI State
 */

#include "gui.h"
#include "transition.h"
#include "video.h"
#include "key.h"
#include "config.h"

#include "game_client.h"
#include "game_server.h"
#include "progress.h"

#include "st_edit.h"
#include "st_title.h"
#include "st_shared.h"

static int gui_root_id;
static int pos_id;

static int edit_gui(void)
{
    int root;

    if ((root = gui_vstack(0)))
    {
        gui_root_id = root;

        /* Editor Toolbar Placeholder */
        int top_bar = gui_hstack(root);
        gui_label(top_bar, "EDITOR MODE (Phase 4 Prototype)", GUI_MED, gui_yel, gui_red);
        gui_filler(top_bar);
        pos_id = gui_label(top_bar, "Pos: 0.0, 0.0, 0.0", GUI_SML, gui_wht, gui_wht);

        gui_filler(root);

        int bottom_bar = gui_hstack(root);
        gui_label(bottom_bar, "[WASD/Stick] Move | [Mouse/Stick] Look | [A] Ascend | [ESC] Exit", GUI_SML, gui_gry, gui_gry);

        gui_layout(root, 0, 0);
    }
    return root;
}

static int edit_enter(struct state *st, struct state *prev, int intent)
{
    video_set_grab(1);

    /* Initialize progress with MODE_EDITOR and load an empty/base map */
    /* If there is no map, we just load level 0 for now to get a skybox */
    progress_init(MODE_EDITOR);
    progress_play(get_level(0));

    return transition_slide(edit_gui(), 1, intent);
}

static int edit_leave(struct state *st, struct state *next, int id, int intent)
{
    video_set_grab(0);
    progress_stop();
    return transition_slide(id, 0, intent);
}

static void edit_paint(int id, float t)
{
    game_client_draw(0, t);
    gui_paint(id);
}

static void edit_timer(int id, float dt)
{
    game_server_step(dt);
    game_client_sync(NULL);
    game_client_blend(game_server_blend());

    /* Update POS readout */
    float p[3];
    curr_ball_pos(0, p);
    char buf[64];
    sprintf(buf, "Pos: %.1f, %.1f, %.1f", p[0], p[1], p[2]);
    gui_set_label(pos_id, buf);

    gui_timer(id, dt);
}

static void edit_stick(int id, int a, float v, int bump, int device_id)
{
    /* Reuse standard movement */
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X0, a))
        game_set_z(v, device_id);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y0, a))
        game_set_x(v, device_id);

    /* View Rotation */
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X1, a))
        game_set_rot(-v, device_id);
}

static void edit_point(int id, int x, int y, int dx, int dy)
{
    /* Mouselook */
    game_set_pos(dx, dy, 0);
}

static int edit_keybd(int c, int d)
{
    if (d)
    {
        /* Basic movement */
        if (c == SDLK_w) game_set_z(1.0f, 0);
        if (c == SDLK_s) game_set_z(-1.0f, 0);
        if (c == SDLK_a) game_set_x(-1.0f, 0);
        if (c == SDLK_d) game_set_x(1.0f, 0);

        if (c == 27) /* ESC */
            return goto_state(&st_title);
    }
    else
    {
        if (c == SDLK_w || c == SDLK_s) game_set_z(0.0f, 0);
        if (c == SDLK_a || c == SDLK_d) game_set_x(0.0f, 0);
    }
    return 1;
}

static int edit_buttn(int b, int d, int device_id)
{
    /* Action button ascends */
    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        game_set_action(d, device_id);

    return 1;
}

struct state st_edit = {
    edit_enter,
    edit_leave,
    edit_paint,
    edit_timer,
    edit_point,
    edit_stick,
    shared_angle,
    shared_click,
    edit_keybd,
    edit_buttn
};
