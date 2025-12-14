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
#include "task_encoder.h"
#include "task_lvgl.h"
#include "task_midi.h"
#include "task_acm.h"

#define ENCODER_DEFAULT_VALUE 		64
#define ENCODER_DEFAULT_MIN_VALUE   0
#define ENCODER_DEFAULT_MAX_VALUE   127
#define ENCODER_DEFAULT_STEP   		1

#define MIDI_DEFAULT_CHANNEL		0	// Channel - 1


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	MENU_SELECT_LOAD_MIDI = 0,
	MENU_SELECT_SAVE_MIDI,
	MENU_SELECT_LOAD_UI,
	MENU_SELECT_SAVE_UI,
	MENU_SELECT_CONFIG_MIDI,
	MENU_SELECT_AUTOMAPPING,
	MENU_SELECT_FIRMWARE_UPDATE,
	MENU_SELECT_EXIT,
	MENU_SELECT_COUNT,
} menu_select_e;

typedef struct {
	uint8_t channel;
	uint8_t cc;
	uint8_t min_value;
	uint8_t max_value;
	uint8_t current_value;
	uint8_t step;
	uint8_t name[16 + 1];
} module_state_t;

typedef enum {
	ACTION_LOAD = 0,
	ACTION_SAVE,
} action_load_save_e;

class TaskOS : public TaskPrototype {
public:
	TaskOS();
	void createTask();
private:
	static void task(void const *arg);
	void processEncoderEvent(encoder_event_t * encoder_event);
	void processMenuSelector(bool increase);
	void processMenuButton();
	void processMidiBankSelector(bool increase);
	void processUiPresetSelector(bool increase);
	void processMidiUnitSelector(bool increase);
	void processMidiParameterSelector(bool increase);

	void setStateCC(uint8_t module_id, uint8_t cc);
	void setStateChannel(uint8_t module_id, uint8_t channel);
	
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
	TaskEncoder * task_encoder_p;
	ScreensEnum current_screen;
	menu_select_e current_menu_selection;
	action_load_save_e selected_action;
	uint8_t midi_bank_selection;	// 4 banks
	uint8_t ui_preset_selection;	// 4 presets
	uint8_t midi_unit_selection;	// 16 units
	uint8_t midi_parameter_selection; // 2 parameters
	bool midi_parameter_channel_selector_active;
	bool midi_parameter_cc_selector_active;
};

extern TaskOS task_os;

#ifdef __cplusplus
}
#endif


#endif /* TASK_OS_H_ */
