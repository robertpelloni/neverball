/*
 * Level Editor GUI State
 */

#include "gui.h"
#include "transition.h"
#include "video.h"
#include "key.h"
#include "config.h"
#include <math.h>

#include "game_client.h"
#include "game_server.h"
#include "progress.h"

#include "st_edit.h"
#include "st_title.h"
#include "st_shared.h"

enum {
    TILE_STRAIGHT = 0,
    TILE_TURN,
    TILE_SPAWN,
    TILE_GOAL,
    TILE_MAX
};

struct editor_tile {
    int type;
    float x;
    float y;
    float z;
};

#define MAX_EDITOR_TILES 128
static struct editor_tile tiles[MAX_EDITOR_TILES];
static int tile_count = 0;
static int current_tile_type = 0;
static float snapped_pos[3];

static int gui_root_id;
static int pos_id;
static int tile_id;

static const char *get_tile_name(int type)
{
    switch (type) {
        case TILE_STRAIGHT: return "Straight Track";
        case TILE_TURN:     return "Turn Track";
        case TILE_SPAWN:    return "Player Spawn";
        case TILE_GOAL:     return "Level Goal";
        default:            return "Unknown";
    }
}

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
        tile_id = gui_label(top_bar, "Tile: Straight Track", GUI_SML, gui_wht, gui_wht);
        gui_filler(top_bar);
        pos_id = gui_label(top_bar, "Pos: 0.0, 0.0, 0.0", GUI_SML, gui_wht, gui_wht);

        gui_filler(root);

        int bottom_bar = gui_hstack(root);
        gui_label(bottom_bar, "[WASD] Move | [Mouse] Look | [Q/E] Change Tile | [SPACE] Place | [BACKSPACE] Remove | [ESC] Exit", GUI_SML, gui_gry, gui_gry);

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

    /* Draw Editor Geometry Prototyping */
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);

    /* Draw Placed Tiles */
    for (int i = 0; i < tile_count; i++) {
        glPushMatrix();
        glTranslatef(tiles[i].x, tiles[i].y, tiles[i].z);


        switch (tiles[i].type) {
            case TILE_STRAIGHT: glColor3f(0.0f, 1.0f, 0.0f); break; /* Green */
            case TILE_TURN:     glColor3f(0.0f, 0.0f, 1.0f); break; /* Blue */
            case TILE_SPAWN:    glColor3f(1.0f, 1.0f, 0.0f); break; /* Yellow */
            case TILE_GOAL:     glColor3f(1.0f, 0.0f, 0.0f); break; /* Red */
            default:            glColor3f(1.0f, 1.0f, 1.0f); break;
        }

        /* Draw a wireframe box 10x10 units centered */
        float s = 5.0f; /* half size */
        glBegin(GL_LINE_LOOP); glVertex3f(-s, -s, -s); glVertex3f( s, -s, -s); glVertex3f( s,  s, -s); glVertex3f(-s,  s, -s); glEnd();
        glBegin(GL_LINE_LOOP); glVertex3f(-s, -s,  s); glVertex3f( s, -s,  s); glVertex3f( s,  s,  s); glVertex3f(-s,  s,  s); glEnd();
        glBegin(GL_LINES);
        glVertex3f(-s, -s, -s); glVertex3f(-s, -s,  s);
        glVertex3f( s, -s, -s); glVertex3f( s, -s,  s);
        glVertex3f( s,  s, -s); glVertex3f( s,  s,  s);
        glVertex3f(-s,  s, -s); glVertex3f(-s,  s,  s);
        glEnd();


        glPopMatrix();
    }

    /* Draw Placement Cursor */
    glPushMatrix();
    glTranslatef(snapped_pos[0], snapped_pos[1], snapped_pos[2]);
    glColor3f(1.0f, 0.0f, 1.0f); /* Magenta Cursor */
    float s = 5.2f; /* Slightly larger than tiles */
    glBegin(GL_LINE_LOOP); glVertex3f(-s, -s, -s); glVertex3f( s, -s, -s); glVertex3f( s,  s, -s); glVertex3f(-s,  s, -s); glEnd();
    glBegin(GL_LINE_LOOP); glVertex3f(-s, -s,  s); glVertex3f( s, -s,  s); glVertex3f( s,  s,  s); glVertex3f(-s,  s,  s); glEnd();
    glBegin(GL_LINES);
    glVertex3f(-s, -s, -s); glVertex3f(-s, -s,  s);
    glVertex3f( s, -s, -s); glVertex3f( s, -s,  s);
    glVertex3f( s,  s, -s); glVertex3f( s,  s,  s);
    glVertex3f(-s,  s, -s); glVertex3f(-s,  s,  s);
    glEnd();
    glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    gui_paint(id);
}

static void edit_timer(int id, float dt)
{
    game_server_step(dt);
    game_client_sync(NULL);
    game_client_blend(game_server_blend());

    /* Update POS readout and calculate snapped position */
    float p[3];
    curr_ball_pos(0, p);

    snapped_pos[0] = roundf(p[0] / 10.0f) * 10.0f;
    snapped_pos[1] = roundf(p[1] / 5.0f) * 5.0f;
    snapped_pos[2] = roundf(p[2] / 10.0f) * 10.0f;


    snapped_pos[0] = roundf(p[0] / 10.0f) * 10.0f;
    snapped_pos[1] = roundf(p[1] / 5.0f) * 5.0f;
    snapped_pos[2] = roundf(p[2] / 10.0f) * 10.0f;

    char buf[64];
    sprintf(buf, "Pos: %.1f, %.1f, %.1f", snapped_pos[0], snapped_pos[1], snapped_pos[2]);
    gui_set_label(pos_id, buf);

    char tile_buf[64];
    sprintf(tile_buf, "Tile: %s", get_tile_name(current_tile_type));
    gui_set_label(tile_id, tile_buf);

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

        if (c == SDLK_q) {
            current_tile_type = (current_tile_type - 1 + TILE_MAX) % TILE_MAX;
        }
        if (c == SDLK_e) {
            current_tile_type = (current_tile_type + 1) % TILE_MAX;
        }

        if (c == SDLK_SPACE) {
            if (tile_count < MAX_EDITOR_TILES) {
                tiles[tile_count].type = current_tile_type;
                tiles[tile_count].x = snapped_pos[0];
                tiles[tile_count].y = snapped_pos[1];
                tiles[tile_count].z = snapped_pos[2];
                tile_count++;
            }
        }

        if (c == SDLK_BACKSPACE) {
            if (tile_count > 0) {
                tile_count--;
            }
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
    /* Action button ascends */
    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        game_set_action(d, device_id);

    /* Dash button places item */
    if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
        game_set_dash(d, device_id);

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
