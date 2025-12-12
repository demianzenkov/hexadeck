#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *menu;
    lv_obj_t *midi_banks;
    lv_obj_t *ui_presets;
    lv_obj_t *config_midi;
    lv_obj_t *config_midi_unit;
    lv_obj_t *config_midi_channel;
    lv_obj_t *config_midi_cc;
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
    lv_obj_t *ui_banks_roller;
    lv_obj_t *config_midi_roller;
    lv_obj_t *config_midi_unit_roller;
    lv_obj_t *panel_config_midi_general;
    lv_obj_t *panel_config_midi_general_1;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MENU = 2,
    SCREEN_ID_MIDI_BANKS = 3,
    SCREEN_ID_UI_PRESETS = 4,
    SCREEN_ID_CONFIG_MIDI = 5,
    SCREEN_ID_CONFIG_MIDI_UNIT = 6,
    SCREEN_ID_CONFIG_MIDI_CHANNEL = 7,
    SCREEN_ID_CONFIG_MIDI_CC = 8,
};

void create_screen_main();
void tick_screen_main();

void create_screen_menu();
void tick_screen_menu();

void create_screen_midi_banks();
void tick_screen_midi_banks();

void create_screen_ui_presets();
void tick_screen_ui_presets();

void create_screen_config_midi();
void tick_screen_config_midi();

void create_screen_config_midi_unit();
void tick_screen_config_midi_unit();

void create_screen_config_midi_channel();
void tick_screen_config_midi_channel();

void create_screen_config_midi_cc();
void tick_screen_config_midi_cc();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/