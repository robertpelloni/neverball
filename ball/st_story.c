/*
 * Story / Cutscene State
 */

#include <stdio.h>
#include <string.h>

#include "gui.h"
#include "transition.h"
#include "image.h"
#include "config.h"
#include "common.h"
#include "video.h"
#include "st_story.h"
#include "st_title.h"

#define MAX_SLIDES 16

struct story_slide {
    char image[256];
    char text[1024];
};

static struct story_slide slides[MAX_SLIDES];
static int slide_count = 0;
static int current_slide = 0;

static struct state *next_state = &st_title;
static int hub_return_id = -1;

static int image_id;
static int text_id;
static int gui_root_id;

void story_set(const char *img, const char *txt, struct state *nxt)
{
    slide_count = 1;
    current_slide = 0;
    SAFECPY(slides[0].image, img);
    SAFECPY(slides[0].text, txt);
    next_state = nxt;
    hub_return_id = -1;
}

void story_set_hub(const char *img, const char *txt, int warp_id)
{
    slide_count = 1;
    current_slide = 0;
    SAFECPY(slides[0].image, img);
    SAFECPY(slides[0].text, txt);
    next_state = NULL; /* Special Hub handling */
    hub_return_id = warp_id;
}

int story_load_script(const char *filename, struct state *nxt)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) return 0;

    slide_count = 0;
    current_slide = 0;
    next_state = nxt;
    hub_return_id = -1;

    char line[1024];
    while (fgets(line, sizeof(line), fp) && slide_count < MAX_SLIDES)
    {
        /* Simple format: ImagePath|Text String */
        char *pipe = strchr(line, '|');
        if (pipe) {
            *pipe = '\0';
            char *img = line;
            char *txt = pipe + 1;

            /* Remove newline from text */
            char *nl = strchr(txt, '\n');
            if (nl) *nl = '\0';

            SAFECPY(slides[slide_count].image, img);
            SAFECPY(slides[slide_count].text, txt);
            slide_count++;
        }
    }
    fclose(fp);
    return slide_count > 0;
}

static void update_slide_ui(void)
{
    if (current_slide < slide_count) {
        gui_set_image(image_id, slides[current_slide].image);
        gui_set_multi(text_id, slides[current_slide].text);
    }
}

static int story_action(int tok, int val)
{
    if (tok == GUI_BACK)
    {
        current_slide++;
        if (current_slide < slide_count)
        {
            update_slide_ui();
            /* Pulse the layout to indicate change */
            gui_slide(gui_root_id, GUI_N | GUI_EASE_BACK, 0, 0.2f, 0);
            return 1;
        }

        /* Reached the end of the script */
        if (hub_return_id >= 0)
        {
            return goto_state(&st_title);
        }
        return goto_state(next_state);
    }
    return 1;
}

static int story_gui(void)
{
    int root;

    if ((root = gui_vstack(0)))
    {
        gui_root_id = root;

        int w = video.device_w * 0.8f;
        int h = video.device_h * 0.5f;

        /* Initialize with empty/first slide to allocate geometry */
        const char *img = (slide_count > 0) ? slides[0].image : " ";
        const char *txt = (slide_count > 0) ? slides[0].text : " ";

        image_id = gui_image(root, img, w, h);
        gui_space(root);
        text_id = gui_multi(root, txt, GUI_MED, gui_wht, gui_blk);
        gui_space(root);
        gui_state(root, "Continue", GUI_MED, GUI_BACK, 0);

        gui_layout(root, 0, 0);
    }
    return root;
}

static int story_enter(struct state *st, struct state *prev, int intent)
{
    return transition_slide(story_gui(), 1, intent);
}

static int story_leave(struct state *st, struct state *next, int id, int intent)
{
    return transition_slide(id, 0, intent);
}

static void story_paint(int id, float t)
{
    gui_paint(id);
}

static int story_click(int b, int d)
{
    if (gui_click(b, d))
        return story_action(gui_token(gui_active()), gui_value(gui_active()));
    return 1;
}

static int story_buttn(int b, int d, int device_id)
{
    if (d)
    {
        int active = gui_active();
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return story_action(gui_token(active), gui_value(active));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return story_action(GUI_BACK, 0);
    }
    return 1;
}

static int story_keybd(int c, int d)
{
    if (d)
    {
        if (c == 27 || c == 13 || c == 32) /* ESC, ENTER, SPACE */
            return story_action(GUI_BACK, 0);
    }
    return 1;
}

struct state st_story = {
    story_enter,
    story_leave,
    story_paint, /* paint */
    NULL, /* timer */
    NULL, /* point */
    NULL, /* stick */
    NULL, /* angle */
    story_click,
    story_keybd,
    story_buttn
};
