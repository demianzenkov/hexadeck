/*
 * task_lvgl.h
 *
 *  Created on: Apr 13, 2024
 *  Author: demian
 */

#ifndef TASK_LVGL_H_
#define TASK_LVGL_H_

#include "lvgl/lvgl.h"
#include "display.h"
#include "task_prototype.h"
#include "cmsis_os.h"
#include "screens.h"
#include "api_midi.h"
#include "module_state.h"

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
#define MAX_CH_LABEL_LENGTH 			(6)
#define MAX_CC_LABEL_LENGTH 			(7)


typedef enum {
	COLOR_ELEMENT_BACKGROUND = 0,
	COLOR_ELEMENT_BORDER,
	COLOR_ELEMENT_TEXT,
	COLOR_ELEMENT_BAR,
} color_element_e;

class UI : public TaskPrototype {
public:
	static UI * getInstance();
	void 		createTask() override;
	void		refreshDisplayState(uint8_t disp, module_state_t * state);
	void 		refreshDisplayValue(uint8_t disp, uint8_t value);
	
	void		lvgl_loadScreen(uint8_t display_id, enum ScreensEnum screen_id);
	void		lvgl_selectMenu(uint16_t selected_index);
	void		lvgl_selectPreset(uint8_t bank_index);
	void		lvgl_selectMidiUnit(uint8_t unit_index);
	void		lvgl_loadMidiUnitParameters(void * module_state); // module_state_t
	void		lvgl_selectMidiParameter(uint8_t parameter_menu_index);
	void		lvgl_activateChannelSelector(bool active);
	void		lvgl_activateCCSelector(bool active);
	
private:
	static void taskUI(void const *arg);
	static void taskLVGL(void const *arg);
	void 		lvgl_setUiState(module_state_t * state);
	void 		lvgl_setValue(value_update_t value);

public:
	lv_display_t * lcd_disp;
	
private:
	osThreadId lvglTaskHandle;
	osThreadId uiTaskHandle;
	SemaphoreHandle_t lvgl_ready_sem;
	SemaphoreHandle_t ui_busy_mutex;
	QueueHandle_t ui_update_queue;
	QueueHandle_t ui_value_queue;
	module_state_t current_ui_state = {};
};


#ifdef __cplusplus
}
#endif


#endif /* TASK_LVGL_H_ */
