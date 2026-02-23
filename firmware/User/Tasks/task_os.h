/*
 * task_os.h
 *
 */

#ifndef TASK_OS_H_
#define TASK_OS_H_

#include "main.h"
#include "cmsis_os.h"
#include "task_prototype.h"
#include "task_buttons.h"
#include "task_lvgl.h"
#include "task_midi.h"
#include "task_acm.h"
#include "api_midi.h"
#include "nvs.h"
#include "module_state.h"


#define ENCODER_DEFAULT_VALUE 		64
#define ENCODER_DEFAULT_MIN_VALUE   0
#define ENCODER_DEFAULT_MAX_VALUE   127
#define ENCODER_DEFAULT_STEP   		1

#define MIDI_DEFAULT_CHANNEL		0	// Channel - 1


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	MENU_SELECT_PRESETS = 0,
	MENU_SELECT_KNOB_SETUP,
	MENU_SELECT_BUTTON_SETUP,
	MENU_SELECT_SCREEN_SETUP,
	MENU_SELECT_AUTOMAPPING,
	MENU_SELECT_FIRMWARE_UPDATE,
	MENU_SELECT_EXIT,
	MENU_SELECT_COUNT,
} menu_select_e;

typedef enum {
	PRESET_MENU_PRESET = 0,
	PRESET_MENU_LOAD,
	PRESET_MENU_SAVE,
	PRESET_MENU_RETURN,
	PRESET_MENU_COUNT,
} preset_menu_select_e;

typedef enum {
	KNOB_SETUP_SELECT_KNOB = 0,
	KNOB_SETUP_CHANNEL,
	KNOB_SETUP_CC,
	KNOB_SETUP_MIN_RANGE,
	KNOB_SETUP_MAX_RANGE,
	KNOB_SETUP_STEP,
	KNOB_SETUP_RETURN,
	KNOB_SETUP_COUNT,
} knob_setup_select_e;

typedef enum {
	BUTTON_SETUP_SELECT_BUTTON = 0,
	BUTTON_SETUP_MIDI,
	BUTTON_SETUP_CHANNEL,
	BUTTON_SETUP_CC,
	BUTTON_SETUP_DEFAULT_VALUE,
	BUTTON_SETUP_PRESSED_VALUE,
	BUTTON_SETUP_ONCLICK,
	BUTTON_SETUP_ONCLICK_STEP,
	BUTTON_SETUP_RETURN,
	BUTTON_SETUP_COUNT,
} button_setup_select_e;

typedef enum {
	SETTINGS_MENU_SCREEN = 0,
	SETTINGS_MENU_SIMPLE_SCREEN,
	SETTINGS_MENU_RETURN,
	SETTINGS_MENU_COUNT,
} settings_menu_select_e;


typedef struct {
	bool increase;		// true - increase, false - decrease
	uint8_t encoder_id;
} encoder_event_t;


class TaskOS : public TaskPrototype {
public:
	TaskOS();
	static TaskOS * getInstance();
	void createTask();
private:
	static void task(void const *arg);
	void processEncoderEvent(encoder_event_t * encoder_event);
	void processMenuSelector(bool increase);
	void processMenuButton();
	void processPresetSelector(bool increase);
	void processKnobSetupSelector(bool increase);
	void processButtonSetupSelector(bool increase);
	void processSettingsSelector(bool increase);
	void refreshScreenSetupUi();
	void captureKnobSetupSnapshot();
	bool knobSetupParamsChanged() const;
	void applySimpleScreenMode(uint8_t display_id, bool enabled);
	ScreensEnum getMainScreenForDisplay(uint8_t display_id) const;
	bool areAllSimpleScreensEnabled() const;

	void setStateValue(uint8_t id, uint8_t value);
	void setStateChannel(uint8_t id, uint8_t channel);
	void setStateCC(uint8_t id, uint8_t cc);
	void setStateRange(uint8_t id, uint8_t max_level);
	void setStateColor(uint8_t id, color_element_e element, lv_color_t color);
	void setStateName(uint8_t id, const char *str);
	
public:
	QueueHandle_t encoder_event_queue;
	QueueHandle_t button_event_queue;
	QueueHandle_t acm_event_queue;
	QueueHandle_t midi_input_event_queue;
	QueueHandle_t midi_sysex_input_event_queue;
	
	module_state_t module_states[16];
private:
	// ui.createTask();
	// task_midi.createTask();
	// buttons.createTask();
	// task_encoder.createTask();
	// acm.createTask();
	ACM * acm_p;
	Buttons * buttons_p;
	UI * ui_p;
	TaskMIDI * task_midi_p;
	NVS nvs;
	ScreensEnum current_screen;
	menu_select_e current_menu_selection;
	preset_menu_select_e preset_menu_selection;
	uint8_t preset_index;	// 4 banks
	bool preset_edit_active;
	knob_setup_select_e knob_setup_selection;
	uint8_t knob_selection;	// 16 knobs
	bool knob_setup_edit_active;
	button_setup_select_e button_setup_selection;
	uint8_t button_selection;	// 16 buttons
	bool button_setup_edit_active;
	settings_menu_select_e settings_menu_selection;
	bool settings_screen_edit_active;
	uint8_t settings_screen_index; // 0xFF = all, 0..15 = screen
	module_state_t knob_setup_snapshot = {};
	uint8_t knob_setup_snapshot_id = 0;
	bool knob_setup_snapshot_valid = false;
	uint32_t last_encoder_event_time[16] = {0};
};

#ifdef __cplusplus
}
#endif


#endif /* TASK_OS_H_ */
