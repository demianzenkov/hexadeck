#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu;
    lv_obj_t *presets;
    lv_obj_t *config_midi;
    lv_obj_t *config_midi_unit;
    lv_obj_t *general_panel;
    lv_obj_t *channel_pannel;
    lv_obj_t *channel_label;
    lv_obj_t *name_panel;
    lv_obj_t *name_label;
    lv_obj_t *level_bar;
    lv_obj_t *level_label;
    lv_obj_t *cc_panel;
    lv_obj_t *cc_label;
    lv_obj_t *range_panel;
    lv_obj_t *range_label;
    lv_obj_t *menu_roller;
    lv_obj_t *midi_banks_roller;
    lv_obj_t *config_midi_roller;
    lv_obj_t *config_midi_unit_roller;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU = 2,
    SCREEN_ID_PRESETS = 3,
    SCREEN_ID_CONFIG_MIDI = 4,
    SCREEN_ID_CONFIG_MIDI_UNIT = 5,
};

void create_screen_main();
void tick_screen_main();

void create_screen_menu();
void tick_screen_menu();

void create_screen_presets();
void tick_screen_presets();

void create_screen_config_midi();
void tick_screen_config_midi();

void create_screen_config_midi_unit();
void tick_screen_config_midi_unit();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/