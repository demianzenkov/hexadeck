#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *main_simple;
    lv_obj_t *menu;
    lv_obj_t *presets;
    lv_obj_t *knob_setup;
    lv_obj_t *button_setup;
    lv_obj_t *screen_setup;
    lv_obj_t *general_panel;
    lv_obj_t *name_label;
    lv_obj_t *channel_label;
    lv_obj_t *cc_label;
    lv_obj_t *level_bar;
    lv_obj_t *level_label;
    lv_obj_t *range_step_label;
    lv_obj_t *range_min_label;
    lv_obj_t *range_max_label;
    lv_obj_t *general_panel_simple;
    lv_obj_t *name_label_simple;
    lv_obj_t *level_bar_simple;
    lv_obj_t *level_label_simple;
    lv_obj_t *menu_roller;
    lv_obj_t *presets_roller;
    lv_obj_t *knob_setup_roller;
    lv_obj_t *button_setup_roller;
    lv_obj_t *screen_setup_roller;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MAIN_SIMPLE = 2,
    SCREEN_ID_MENU = 3,
    SCREEN_ID_PRESETS = 4,
    SCREEN_ID_KNOB_SETUP = 5,
    SCREEN_ID_BUTTON_SETUP = 6,
    SCREEN_ID_SCREEN_SETUP = 7,
};

void create_screen_main();
void tick_screen_main();

void create_screen_main_simple();
void tick_screen_main_simple();

void create_screen_menu();
void tick_screen_menu();

void create_screen_presets();
void tick_screen_presets();

void create_screen_knob_setup();
void tick_screen_knob_setup();

void create_screen_button_setup();
void tick_screen_button_setup();

void create_screen_screen_setup();
void tick_screen_screen_setup();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/