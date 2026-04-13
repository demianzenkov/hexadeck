#ifndef MODULE_STATE_H_
#define MODULE_STATE_H_

#include <stdint.h>
#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t display_id;
	uint8_t value;
	uint8_t channel;
	uint8_t cc;
	uint8_t min_value;
	uint8_t max_value;
	uint8_t step;
	uint8_t simple_screen_enabled;
	uint8_t button_midi_enabled;
	uint8_t button_onclick_mode;
	uint8_t button_onclick_active;
	uint8_t button_onclick_step;
	uint8_t button_midi_channel;
	uint8_t button_midi_cc;
	uint8_t button_midi_released_value;
	uint8_t button_midi_pressed_value;
	char name[16 + 1];
	lv_color_t background_color;
	lv_color_t bar_color;
	lv_color_t text_color;
	lv_color_t border_color;
} module_state_t;

typedef enum {
	BUTTON_MIDI_ENABLED = 0,
	BUTTON_MIDI_DISABLED,
} button_midi_state_e;

typedef enum {
	BUTTON_ONCLICK_STEP = 0,
	BUTTON_ONCLICK_DISABLED,
} button_onclick_mode_e;

typedef struct {
	uint8_t id;
	uint8_t value;
	uint8_t range_max;
} value_update_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* MODULE_STATE_H_ */