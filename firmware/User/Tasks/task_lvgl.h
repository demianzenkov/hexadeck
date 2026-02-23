/*
 * task_lvgl.h
 *
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
	void 		refreshDisplayValue(uint8_t disp, uint8_t value, uint8_t range_max);
	
	void		lvgl_loadScreen(uint8_t display_id, enum ScreensEnum screen_id);
	void		lvgl_selectMenu(uint16_t selected_index);
	void		lvgl_selectPreset(uint8_t menu_index);
	void		lvgl_loadPresetOptions(uint8_t preset_index);
	void		lvgl_activatePresetSelector(bool active);
	void		lvgl_selectKnobSetup(uint8_t selected_index);
	void		lvgl_loadKnobSetupParameters(uint8_t knob_index, const module_state_t *state);
	void		lvgl_activateKnobSetupSelector(bool active);
	void		lvgl_selectButtonSetup(uint8_t selected_index);
	void		lvgl_loadButtonSetupParameters(uint8_t button_index, const module_state_t *state);
	void		lvgl_activateButtonSetupSelector(bool active);
	void		lvgl_selectSettings(uint8_t selected_index);
	void		lvgl_loadSettingsOptions(uint8_t screen_index, bool simple_screen_enabled);
	void		lvgl_activateSettingsSelector(bool active);
	void		setSimpleMode(uint8_t display_id, bool enabled);
	bool		isSimpleMode(uint8_t display_id) const;
	
private:
	static void taskUI(void const *arg);
	static void taskLVGL(void const *arg);
	void 		lvgl_setUiState(module_state_t * state);
	void 		lvgl_setValue(value_update_t value);

public:
	lv_display_t * lcd_disp;
	
private:
	Display display;
	osThreadId lvglTaskHandle;
	osThreadId uiTaskHandle;
	SemaphoreHandle_t lvgl_ready_sem;
	SemaphoreHandle_t ui_busy_mutex;
	QueueHandle_t ui_update_queue;
	QueueHandle_t ui_value_queue;
	module_state_t current_ui_state = {};
	bool simple_mode[16] = {};
	bool last_simple_mode[16] = {};
};


#ifdef __cplusplus
}
#endif


#endif /* TASK_LVGL_H_ */
