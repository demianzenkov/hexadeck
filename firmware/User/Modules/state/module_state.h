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
	char name[16 + 1];
	lv_color_t background_color;
	lv_color_t bar_color;
	lv_color_t text_color;
	lv_color_t border_color;
} module_state_t;

typedef struct {
	uint8_t id;
	uint8_t value;
} value_update_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* MODULE_STATE_H_ */