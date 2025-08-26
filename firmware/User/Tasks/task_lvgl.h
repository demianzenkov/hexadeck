/*
 * task_lvgl.h
 *
 *  Created on: Apr 13, 2024
 *      Author: demian
 */

#ifndef TASK_LVGL_H_
#define TASK_LVGL_H_

#include "lvgl.h"
#include "display.h"


#ifdef __cplusplus
extern "C" {
#endif


#define LCD_H_RES       		(132)
#define LCD_V_RES       		(162)

#define LCD_H_PHYSICAL_RES      (80)
#define LCD_V_PHYSICAL_RES      (160)

#define LCD_DRAW_BUFF_HEIGHT  	(40)

#define BUS_SPI1_POLL_TIMEOUT 	(0x1000U)
#define MAX_NAME_LENGTH 		(16)
#define MAX_CH_LENGTH 			(4)


typedef struct {
    uint8_t display_id;
    char str[MAX_NAME_LENGTH];
} show_string_t;

typedef struct {
	uint8_t display_id;
	uint8_t level;
} show_level_t;

typedef struct {
    uint8_t display_id;
    uint8_t * img_buf;
    size_t img_size;
} show_img_t;

typedef enum {
	COLOR_ELEMENT_BACKGROUND = 0,
	COLOR_ELEMENT_BORDER,
	COLOR_ELEMENT_TEXT,
	COLOR_ELEMENT_BAR,
} color_element_e;


typedef struct {
    uint8_t display_id;
	color_element_e element;
    lv_color_t color;
} show_color_t;


typedef struct {
	uint8_t display_id;
	uint8_t bar_level;
	uint8_t max_bar_level;
	char name[MAX_NAME_LENGTH];
	char channel[MAX_CH_LENGTH];
	char value[MAX_NAME_LENGTH];
	lv_color_t background_color;
	lv_color_t bar_color;
	lv_color_t text_color;
	lv_color_t border_color;
} ui_state_t;

class UI {
public:
	UI();

	void createTask();
	void showBarLevel(uint8_t disp, uint8_t value);
	void showValue(uint8_t disp, const char * str);
	void showName(uint8_t disp, const char * str);
	void showChannel(uint8_t disp, const char * str);
	void showColor(uint8_t disp, color_element_e element, lv_color_t color);
	void changeBarRange(uint8_t disp, uint8_t max_level);

private:
	static void taskUI(void const *arg);
	static void taskLVGL(void const *arg);
	void lvgl_initUiState();
	void lvgl_setUiState(ui_state_t * state);

public:
	lv_display_t * lcd_disp;

private:
	osThreadId lvglTaskHandle;
	osThreadId uiTaskHandle;
	ui_state_t ui_states[16];
	ui_state_t current_ui_state;
	SemaphoreHandle_t lvgl_ready_sem;
	SemaphoreHandle_t ui_busy_mutex;
	QueueHandle_t show_name_queue;
	QueueHandle_t show_value_queue;
	QueueHandle_t show_channel_queue;
	QueueHandle_t show_level_queue;
	QueueHandle_t show_color_queue;

	QueueHandle_t ui_update_queue;
	// QueueHandle_t show_image_queue;
	
	SemaphoreHandle_t ui_ready_sem;

};

extern UI ui;

#ifdef __cplusplus
}
#endif


#endif /* TASK_LVGL_H_ */
