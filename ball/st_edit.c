/*
 * Level Editor GUI State (Phase 4 In-Engine Editor)
 *
 * Provides a dynamic in-engine layout editor with grid-snapped object placement,
 * tile cycling, coordinate HUD, and level layout persistence.
 */

#include <stdio.h>
#include <math.h>

#include "gui.h"
#include "transition.h"
#include "video.h"
#include "key.h"
#include "config.h"
#include "glext.h"

#include "game_client.h"
#include "game_server.h"
#include "progress.h"

#include "st_edit.h"
#include "st_title.h"
#include "st_shared.h"

#define MAX_EDITOR_TILES 128
#define GRID_SNAP_SIZE 2.0f

enum editor_tile_type {
    TILE_COIN_YELLOW = 0,
    TILE_COIN_RED,
    TILE_COIN_BLUE,
    TILE_GOAL,
    TILE_BUMPER,
    TILE_HAZARD,
    TILE_COUNT
};

static const char *tile_names[TILE_COUNT] = {
    "Banana / Coin (Yellow)",
    "Coin (Red +5)",
    "Coin (Blue +10)",
    "Goal Tape",
    "Bumper / Spring",
    "Hazard / Spike"
};

struct editor_tile {
    int type;
    float pos[3];
};

static struct editor_tile placed_tiles[MAX_EDITOR_TILES];
static int placed_tile_count = 0;
static int current_tile_type = TILE_COIN_YELLOW;

static int gui_root_id;
static int pos_id;
static int tile_info_id;

static void editor_save_layout(const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f) return;
    fprintf(f, "# Neverball Editor Level Layout\n");
    fprintf(f, "count %d\n", placed_tile_count);
    for (int i = 0; i < placed_tile_count; i++)
    {
        fprintf(f, "tile %d %.2f %.2f %.2f\n",
                placed_tiles[i].type,
                placed_tiles[i].pos[0],
                placed_tiles[i].pos[1],
                placed_tiles[i].pos[2]);
    }
    fclose(f);
}

static void editor_load_layout(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) return;
    placed_tile_count = 0;
    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        int type;
        float x, y, z;
        if (sscanf(line, "tile %d %f %f %f", &type, &x, &y, &z) == 4)
        {
            if (placed_tile_count < MAX_EDITOR_TILES)
            {
                placed_tiles[placed_tile_count].type = type;
                placed_tiles[placed_tile_count].pos[0] = x;
                placed_tiles[placed_tile_count].pos[1] = y;
                placed_tiles[placed_tile_count].pos[2] = z;
                placed_tile_count++;
            }
        }
    }
    fclose(f);
}

static void editor_update_hud(void)
{
    char buf[128];
    sprintf(buf, "[Q/E] Select: %s | Tiles Placed: %d/%d",
            tile_names[current_tile_type], placed_tile_count, MAX_EDITOR_TILES);
    if (tile_info_id)
        gui_set_label(tile_info_id, buf);
}

static int edit_gui(void)
{
    int root;

    if ((root = gui_vstack(0)))
    {
        gui_root_id = root;

        /* Top Bar: Title & Coordinates */
        int top_bar = gui_hstack(root);
        gui_label(top_bar, "LEVEL EDITOR (Phase 4)", GUI_MED, gui_yel, gui_red);
        gui_filler(top_bar);
        pos_id = gui_label(top_bar, "Pos: 0.0, 0.0, 0.0", GUI_SML, gui_wht, gui_wht);

        /* Middle Bar: Active Tile & Selection */
        int mid_bar = gui_hstack(root);
        tile_info_id = gui_label(mid_bar, "", GUI_SML, gui_yel, gui_yel);
        gui_filler(mid_bar);

        gui_filler(root);

        /* Bottom Bar: Action Hotkeys */
        int bottom_bar = gui_hstack(root);
        gui_label(bottom_bar, "[WASD] Fly | [Q/E] Cycle Tile | [SPACE] Place | [BACKSPACE] Remove | [F5/F9] Save/Load | [ESC] Exit", GUI_SML, gui_gry, gui_gry);

        gui_layout(root, 0, 0);
        editor_update_hud();
    }
    return root;
}

static int edit_enter(struct state *st, struct state *prev, int intent)
{
    video_set_grab(1);

    /* Initialize progress with MODE_EDITOR and load level 0 for skybox */
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

static void editor_render_tiles(void)
{
    if (placed_tile_count == 0) return;

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);

    for (int i = 0; i < placed_tile_count; i++)
    {
        float x = placed_tiles[i].pos[0];
        float y = placed_tiles[i].pos[1];
        float z = placed_tiles[i].pos[2];
        float s = GRID_SNAP_SIZE * 0.45f;

        switch (placed_tiles[i].type)
        {
        case TILE_COIN_YELLOW:
            glColor4f(1.0f, 0.9f, 0.0f, 0.9f);
            break;
        case TILE_COIN_RED:
            glColor4f(1.0f, 0.1f, 0.1f, 0.9f);
            break;
        case TILE_COIN_BLUE:
            glColor4f(0.1f, 0.4f, 1.0f, 0.9f);
            break;
        case TILE_GOAL:
            glColor4f(0.1f, 1.0f, 0.2f, 0.9f);
            break;
        case TILE_BUMPER:
            glColor4f(0.8f, 0.2f, 1.0f, 0.9f);
            break;
        case TILE_HAZARD:
            glColor4f(1.0f, 0.4f, 0.0f, 0.9f);
            break;
        default:
            glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
            break;
        }

        /* Draw 3D wireframe bounding box representing placed tile */
        glBegin(GL_LINES);
        /* Bottom quad */
        glVertex3f(x - s, y - s, z - s); glVertex3f(x + s, y - s, z - s);
        glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y - s, z + s);
        glVertex3f(x + s, y - s, z + s); glVertex3f(x - s, y - s, z + s);
        glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y - s, z - s);

        /* Top quad */
        glVertex3f(x - s, y + s, z - s); glVertex3f(x + s, y + s, z - s);
        glVertex3f(x + s, y + s, z - s); glVertex3f(x + s, y + s, z + s);
        glVertex3f(x + s, y + s, z + s); glVertex3f(x - s, y + s, z + s);
        glVertex3f(x - s, y + s, z + s); glVertex3f(x - s, y + s, z - s);

        /* Pillars */
        glVertex3f(x - s, y - s, z - s); glVertex3f(x - s, y + s, z - s);
        glVertex3f(x + s, y - s, z - s); glVertex3f(x + s, y + s, z - s);
        glVertex3f(x + s, y - s, z + s); glVertex3f(x + s, y + s, z + s);
        glVertex3f(x - s, y - s, z + s); glVertex3f(x - s, y + s, z + s);
        glEnd();
    }
}

static void edit_paint(int id, float t)
{
    game_client_draw(0, t);
    editor_render_tiles();
    gui_paint(id);
}

static void edit_timer(int id, float dt)
{
    game_server_step(dt);
    game_client_sync(NULL);
    game_client_blend(game_server_blend());

    /* Update Position readout */
    float p[3];
    curr_ball_pos(0, p);
    char buf[64];
    sprintf(buf, "Pos: %.1f, %.1f, %.1f (Snap: %.1f)", p[0], p[1], p[2], GRID_SNAP_SIZE);
    gui_set_label(pos_id, buf);

    gui_timer(id, dt);
}

static void edit_stick(int id, int a, float v, int bump, int device_id)
{
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X0, a))
        game_set_z(v, device_id);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y0, a))
        game_set_x(v, device_id);

    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X1, a))
        game_set_rot(-v, device_id);
}

static void edit_point(int id, int x, int y, int dx, int dy)
{
    game_set_pos(dx, dy, 0);
}

static void editor_place_current_tile(void)
{
    if (placed_tile_count >= MAX_EDITOR_TILES) return;

    float p[3];
    curr_ball_pos(0, p);

    /* Snap to nearest grid coordinate */
    p[0] = roundf(p[0] / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;
    p[1] = roundf(p[1] / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;
    p[2] = roundf(p[2] / GRID_SNAP_SIZE) * GRID_SNAP_SIZE;

    placed_tiles[placed_tile_count].type = current_tile_type;
    placed_tiles[placed_tile_count].pos[0] = p[0];
    placed_tiles[placed_tile_count].pos[1] = p[1];
    placed_tiles[placed_tile_count].pos[2] = p[2];
    placed_tile_count++;

    /* Trigger engine item placement */
    game_set_dash(1, 0);
    game_set_dash(0, 0);

    editor_update_hud();
}

static void editor_remove_last_tile(void)
{
    if (placed_tile_count > 0)
    {
        placed_tile_count--;
        editor_update_hud();
    }
}

static int edit_keybd(int c, int d)
{
    if (d)
    {
        /* Movement */
        if (c == SDLK_w) game_set_z(1.0f, 0);
        if (c == SDLK_s) game_set_z(-1.0f, 0);
        if (c == SDLK_a) game_set_x(-1.0f, 0);
        if (c == SDLK_d) game_set_x(1.0f, 0);

        /* Tile Cycling */
        if (c == SDLK_q)
        {
            current_tile_type = (current_tile_type - 1 + TILE_COUNT) % TILE_COUNT;
            editor_update_hud();
        }
        if (c == SDLK_e)
        {
            current_tile_type = (current_tile_type + 1) % TILE_COUNT;
            editor_update_hud();
        }

        /* Placement and Deletion */
        if (c == SDLK_SPACE)
            editor_place_current_tile();
        if (c == SDLK_BACKSPACE)
            editor_remove_last_tile();

        /* Persistence */
        if (c == SDLK_F5)
            editor_save_layout("editor_level.txt");
        if (c == SDLK_F9)
        {
            editor_load_layout("editor_level.txt");
            editor_update_hud();
        }

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
    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        game_set_action(d, device_id);

    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b) && d)
        editor_place_current_tile();

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
