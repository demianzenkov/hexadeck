/*
 * task_lvgl.cpp
 *
 */

#include "task_lvgl.h"
#include "task_os.h"
#include "cmsis_os.h"
#include "main.h"
#include "ui.h"
#include "screens.h"
#include "string.h"
#include "stdio.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/display/st7735/lv_st7735.h"
#include "lvgl/src/display/lv_display_private.h"
#include "lvgl/src/display/lv_display.h"
#include "lv_lcd_custom_mipi.h"

#define TEST_UI 0

static uint8_t lvgl_draw_buffer[LCD_DRAW_BUFF_HEIGHT * LCD_V_PHYSICAL_RES * 2] = {};

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;


// static void TaskLVGL_task(void const *arg);
// static void TaskLVGL_ui_task(void const *arg);

#if TEST_UI
osThreadId testUITaskHandle;
static void TaskLVGL_test_ui_task(void const *arg);
#endif

static const uint8_t init_cmd_list[] = {
	0xB1, 3, 0x05, 0x3C, 0x3C,
	0xB2, 3, 0x05, 0x3C, 0x3C,
	0xB3, 6, 0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C,
	0xB4, 1, 0x03,
	0xC0, 3, 0x28, 0x08, 0x04,
	0xC1, 1, 0XC0,
	0xC2, 2, 0x0D, 0x00,
	0xC3, 2, 0x8D, 0x2A,
	0xC4, 2, 0x8D, 0xEE,
	0xC5, 1, 0x10,
	0xE0, 16, 0x04, 0x22, 0x07, 0x0A, 0x2E, 0x30, 0x25, 0x2A, 0x28, 0x26, 0x2E, 0x3A, 0x00, 0x01, 0x03, 0x13,
	0xE1, 16, 0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13,
	LV_LCD_CMD_DELAY_MS, LV_LCD_CMD_EOF};


UI * UI::getInstance()
{
	static UI instance;
	return &instance;
}


void UI::createTask()
{
	lvgl_ready_sem = xSemaphoreCreateBinary();
	ui_busy_mutex = xSemaphoreCreateMutex();
	xSemaphoreGive(ui_busy_mutex);
	xSemaphoreTake(lvgl_ready_sem, 0);

	ui_update_queue = xQueueCreate(64, sizeof(module_state_t *));
	ui_value_queue = xQueueCreate(64, sizeof(value_update_t));

	osThreadDef(lvglTask, taskLVGL, osPriorityNormal, 0, 1024);
	lvglTaskHandle = osThreadCreate(osThread(lvglTask), this);

	osThreadDef(uiTask, taskUI, osPriorityNormal, 0, 1024);
	uiTaskHandle = osThreadCreate(osThread(uiTask), this);

#if TEST_UI
	osThreadDef(testUITask, TaskLVGL_test_ui_task, osPriorityNormal, 0, 512);
	testUITaskHandle = osThreadCreate(osThread(testUITask), NULL);
#endif
}

	
void UI::taskUI(void const *arg)
{
	UI *p_this = (UI *)arg;
	module_state_t * ui_state;
	value_update_t value_update;
	xSemaphoreTake(p_this->lvgl_ready_sem, portMAX_DELAY);
	
	xSemaphoreTake(p_this->ui_busy_mutex, portMAX_DELAY);
	ui_init();
	loadScreen(p_this->isSimpleMode(0) ? SCREEN_ID_MAIN_SIMPLE : SCREEN_ID_MAIN);
	xSemaphoreGive(p_this->ui_busy_mutex);

	
	for (;;)
	{
		if(xQueueReceive(p_this->ui_update_queue, &ui_state, 0) == pdTRUE) {
			p_this->lvgl_setUiState(ui_state);
			vTaskDelay(30);
		}
		if(xQueueReceive(p_this->ui_value_queue, &value_update, 0) == pdTRUE) {
			p_this->lvgl_setValue(value_update);
			vTaskDelay(5);
		}
		vTaskDelay(1);
		// implement queue for value changes
	}
}


void UI::taskLVGL(void const *arg)
{
	UI *p_this = (UI *)arg;
	/* Initialize LVGL */
	lv_init();

	for (int i = 0; i < 16; i++)
	{
		p_this->display.set_active_display(i);
		p_this->display.lcd_io_init();
	}

	p_this->lcd_disp = p_this->display.createDisplay();

	/* Don't explicitly set color format, let LVGL use its default */
	/* lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565); */

	for (int i = 0; i < 16; i++)
	{
		p_this->display.set_active_display(i);
		lv_lcd_custom_init_controller(p_this->lcd_disp, LV_LCD_FLAG_BGR);
	}

	lv_lcd_custom_mipi_set_callbacks(p_this->lcd_disp);

	for (int i = 0; i < 16; i++)
	{
		p_this->display.set_active_display(i);
		lv_lcd_generic_mipi_send_cmd_list(p_this->lcd_disp, init_cmd_list);
		lv_display_set_rotation(p_this->lcd_disp, LV_DISPLAY_ROTATION_90);
		lv_display_set_resolution(p_this->lcd_disp, LCD_H_PHYSICAL_RES, LCD_V_PHYSICAL_RES);
		// lv_st7735_set_invert(p_this->lcd_disp, 1);
		lv_display_set_offset(p_this->lcd_disp, (LCD_V_RES - LCD_V_PHYSICAL_RES) / 2 - 1, (LCD_H_RES - LCD_H_PHYSICAL_RES) / 2 - 2);
		// lv_display_set_offset(p_this->lcd_disp, (LCD_V_RES - LCD_V_PHYSICAL_RES) / 2 - 2, (LCD_H_RES - LCD_H_PHYSICAL_RES) / 2 - 2);
		
	}

	/* Allocate draw buffers on the heap. */
	lv_color_t *buf1 = nullptr;
	lv_color_t *buf2 = nullptr;

	uint32_t buf_size = LCD_DRAW_BUFF_HEIGHT * LCD_V_PHYSICAL_RES * 2;
	lv_display_set_buffers(p_this->lcd_disp, lvgl_draw_buffer, buf2, buf_size,
							   LV_DISPLAY_RENDER_MODE_PARTIAL);

	xSemaphoreGive(p_this->lvgl_ready_sem);

	for (;;)
	{
		xSemaphoreTake(p_this->ui_busy_mutex, portMAX_DELAY);
		lv_timer_handler();
		xSemaphoreGive(p_this->ui_busy_mutex);
		osDelay(1);
	}
}


void UI::refreshDisplayState(uint8_t disp, module_state_t * state)
{
	xQueueSend(ui_update_queue, &state, portMAX_DELAY);
}


void UI::refreshDisplayValue(uint8_t disp, uint8_t value, uint8_t range_max)
{
	value_update_t state = {
		.id = disp,
		.value = value,
		.range_max = range_max
	};
	xQueueSend(ui_value_queue, &state, portMAX_DELAY);
}


void UI::lvgl_setUiState(module_state_t * state)
{
	if (state->display_id > 15)
	{
		return;
	}
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	
	bool use_simple = isSimpleMode(state->display_id);
	bool force_update = (current_ui_state.display_id != state->display_id);
	bool mode_changed = (last_simple_mode[state->display_id] != use_simple);
	if(mode_changed) {
		last_simple_mode[state->display_id] = use_simple;
		force_update = true;
	}
	current_ui_state.display_id = state->display_id;
	display.set_active_display(state->display_id);

	// Always update if display_id changed, otherwise only if value changed
	// if(force_update || memcmp(&current_ui_state, state, sizeof(module_state_t)) != 0) {
	// 	// Update needed
	
	// 	lv_bar_set_value(objects.level_bar, state->value, LV_ANIM_OFF);
	// 	char value_str[6] = {};
	// 	snprintf(value_str, sizeof(value_str), "%d", state->value);
	// 	lv_label_set_text(objects.level_label, value_str);
	
	// 	lv_bar_set_range(objects.level_bar, 0, current_ui_state.max_value);
	
	// 	current_ui_state.channel = state->channel;
	// 	char channel_str[MAX_CH_LABEL_LENGTH] = {};
	// 	snprintf(channel_str, sizeof(channel_str), "CH-%d", state->channel+1);
	// 	lv_label_set_text(objects.channel_label, channel_str);
	
	// 	current_ui_state.cc = state->cc;
	// 	char cc_str[MAX_CC_LABEL_LENGTH] = {};
	// 	snprintf(cc_str, sizeof(cc_str), "CC-%d", state->cc);
	// 	lv_label_set_text(objects.cc_label, cc_str);
	
	// 	strncpy(current_ui_state.name, state->name, sizeof(current_ui_state.name));
	// 	lv_label_set_text(objects.name_label, state->name);
	
	// 	lv_obj_set_style_bg_color(objects.level_bar, state->bar_color, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_bg_opa(objects.level_bar, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	
	// 	lv_obj_set_style_border_color(objects.general_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_border_color(objects.channel_pannel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_border_color(objects.name_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_border_color(objects.cc_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_border_color(objects.range_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	// 	lv_obj_set_style_text_color(objects.name_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(objects.channel_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(objects.level_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(objects.cc_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(objects.range_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	
	// 	lv_obj_set_style_bg_color(objects.general_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_bg_color(objects.channel_pannel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_bg_color(objects.name_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_bg_color(objects.cc_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	// 	lv_obj_set_style_bg_color(objects.range_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);

	// 	memcpy(&current_ui_state, state, sizeof(module_state_t));
	// }
	if(force_update || (current_ui_state.value != state->value)) {
		current_ui_state.value = state->value;
		char value_str[6] = {};
		snprintf(value_str, sizeof(value_str), "%d", state->value);
		if(use_simple) {
			lv_bar_set_value(objects.level_bar_simple, state->value, LV_ANIM_OFF);
			lv_label_set_text(objects.level_label_simple, value_str);
		} else {
			lv_bar_set_value(objects.level_bar, state->value, LV_ANIM_OFF);
			lv_label_set_text(objects.level_label, value_str);
		}
	}
	if(force_update || (current_ui_state.max_value != state->max_value) || (current_ui_state.min_value != state->min_value) || (current_ui_state.step != state->step)) {
		current_ui_state.max_value = state->max_value;
		current_ui_state.min_value = state->min_value;
		current_ui_state.step = state->step;
		if(use_simple) {
			lv_bar_set_range(objects.level_bar_simple, 0, current_ui_state.max_value);
			lv_bar_set_value(objects.level_bar_simple, state->value, LV_ANIM_OFF);
		} else {
			lv_bar_set_range(objects.level_bar, 0, current_ui_state.max_value);
			lv_bar_set_value(objects.level_bar, state->value, LV_ANIM_OFF);
			char step_str[8] = {};
			snprintf(step_str, sizeof(step_str), "S: %d", state->step);
			lv_label_set_text(objects.range_step_label, step_str);
			char min_str[6] = {};
			snprintf(min_str, sizeof(min_str), "%d", state->min_value);
			lv_label_set_text(objects.range_min_label, min_str);
			char max_str[6] = {};
			snprintf(max_str, sizeof(max_str), "%d", state->max_value);
			lv_label_set_text(objects.range_max_label, max_str);
		}
	}
	if(force_update || (current_ui_state.channel != state->channel) || (state->channel == 0)) {
		if(!use_simple) {
			current_ui_state.channel = state->channel;
			char channel_str[MAX_CH_LABEL_LENGTH] = {};
			snprintf(channel_str, sizeof(channel_str), "CH-%d", state->channel+1);
			lv_label_set_text(objects.channel_label, channel_str);
		}
	}
	if(force_update || (current_ui_state.cc != state->cc) || (state->cc == 0)) {
		if(!use_simple) {
			current_ui_state.cc = state->cc;
			char cc_str[MAX_CC_LABEL_LENGTH] = {};
			snprintf(cc_str, sizeof(cc_str), "CC-%d", state->cc);
			lv_label_set_text(objects.cc_label, cc_str);
		}
	}
	if(force_update || (strcmp(current_ui_state.name, state->name) != 0)) {
		strncpy(current_ui_state.name, state->name, sizeof(current_ui_state.name));
		if(use_simple) {
			lv_label_set_text(objects.name_label_simple, state->name);
		} else {
			lv_label_set_text(objects.name_label, state->name);
		}
	}
	if(force_update || (lv_color_eq(current_ui_state.bar_color, state->bar_color) == false)) {
		current_ui_state.bar_color = state->bar_color;
		if(use_simple) {
			lv_obj_set_style_bg_color(objects.level_bar_simple, state->bar_color, LV_PART_INDICATOR | LV_STATE_DEFAULT);
			lv_obj_set_style_bg_opa(objects.level_bar_simple, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);
		} else {
			lv_obj_set_style_bg_color(objects.level_bar, state->bar_color, LV_PART_INDICATOR | LV_STATE_DEFAULT);
			lv_obj_set_style_bg_opa(objects.level_bar, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);
		}
	}
	if(force_update || (lv_color_eq(current_ui_state.border_color, state->border_color) == false)) {
		current_ui_state.border_color = state->border_color;
		if(!use_simple) {
			lv_obj_set_style_border_color(objects.general_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	}
	if(force_update || (lv_color_eq(current_ui_state.text_color, state->text_color) == false)) {
		current_ui_state.text_color = state->text_color;
		if(use_simple) {
			lv_obj_set_style_text_color(objects.name_label_simple, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.level_label_simple, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		} else {
			lv_obj_set_style_text_color(objects.name_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.channel_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.level_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.cc_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.range_step_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.range_min_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
			lv_obj_set_style_text_color(objects.range_max_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	}
	if(force_update || (lv_color_eq(current_ui_state.background_color, state->background_color) == false)) {
		current_ui_state.background_color = state->background_color;
		if(use_simple) {
			lv_obj_set_style_bg_color(objects.general_panel_simple, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		} else {
			lv_obj_set_style_bg_color(objects.general_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		}
	}

	xSemaphoreGive(ui_busy_mutex);
}


void UI::lvgl_setValue(value_update_t value)
{
	if (value.id > 15)
	{
		return;
	}
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	
	display.set_active_display(value.id);
	bool use_simple = isSimpleMode(value.id);

	char value_str[6] = {};
	snprintf(value_str, sizeof(value_str), "%d", value.value);
	if(use_simple) {
		lv_bar_set_range(objects.level_bar_simple, 0, value.range_max);
		lv_bar_set_value(objects.level_bar_simple, value.value, LV_ANIM_OFF);
		lv_label_set_text(objects.level_label_simple, value_str);
	} else {
		lv_bar_set_range(objects.level_bar, 0, value.range_max);
		lv_bar_set_value(objects.level_bar, value.value, LV_ANIM_OFF);
		lv_label_set_text(objects.level_label, value_str);
	}

	xSemaphoreGive(ui_busy_mutex);
}


void UI::lvgl_loadScreen(uint8_t display_id, enum ScreensEnum screen_id)
{
	if (display_id > 15)
	{
		return;
	}
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(display_id);
	loadScreen(screen_id);
	xSemaphoreGive(ui_busy_mutex);
}


void UI::lvgl_selectMenu(uint16_t selected_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	lv_obj_t * roller = objects.menu_roller;
	lv_roller_set_selected(roller, selected_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}


void UI::lvgl_selectPreset(uint8_t menu_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.presets_roller;
	lv_roller_set_selected(roller, menu_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_loadPresetOptions(uint8_t preset_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.presets_roller;
	char options_buffer[64] = {};
	snprintf(options_buffer, sizeof(options_buffer), "Preset: %d\nLoad\nSave\nReturn", preset_index + 1);
	lv_roller_set_options(roller, options_buffer, LV_ROLLER_MODE_INFINITE);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_activatePresetSelector(bool active)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.presets_roller;
	lv_obj_set_style_text_color(roller, lv_color_hex(0xff000000), LV_PART_SELECTED | LV_STATE_CHECKED);
	lv_obj_set_style_border_width(roller, 2, LV_PART_SELECTED | LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(roller, lv_color_hex(0xffdedede), LV_PART_SELECTED | LV_STATE_CHECKED);
	lv_obj_set_state(roller, LV_STATE_CHECKED, active);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_selectKnobSetup(uint8_t selected_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.knob_setup_roller;
	lv_roller_set_selected(roller, selected_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_loadKnobSetupParameters(uint8_t knob_index, const module_state_t *state)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.knob_setup_roller;
	char options_buffer[160] = {};
	snprintf(options_buffer, sizeof(options_buffer),
		"Selected Knob: %d\nChannel: %d\nCC: %d\nMin. range: %d\nMax. range: %d\nStep: %d\nReturn",
		knob_index + 1,
		state->channel + 1,
		state->cc,
		state->min_value,
		state->max_value,
		state->step
	);
	lv_roller_set_options(roller, options_buffer, LV_ROLLER_MODE_INFINITE);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_activateKnobSetupSelector(bool active)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.knob_setup_roller;
	lv_obj_set_state(roller, LV_STATE_CHECKED, active);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_selectButtonSetup(uint8_t selected_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.button_setup_roller;
	lv_roller_set_selected(roller, selected_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_loadButtonSetupParameters(uint8_t button_index, const module_state_t *state)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.button_setup_roller;
	const char *midi_str = (state->button_midi_enabled == BUTTON_MIDI_ENABLED) ? "enabled" : "disabled";
	const char *onclick_str = (state->button_onclick_mode == BUTTON_ONCLICK_STEP) ? "step" : "disabled";
	char options_buffer[200] = {};
	snprintf(options_buffer, sizeof(options_buffer),
		"Selected Button: %d\nMIDI: %s\nChannel: %d\nCC: %d\nDefault value: %d\nPressed value: %d\nOnclick: %s\nOnclick step: %d\nReturn",
		button_index + 1,
		midi_str,
		state->button_midi_channel + 1,
		state->button_cc,
		state->button_default_value,
		state->button_pressed_value,
		onclick_str,
		state->button_onclick_step
	);
	lv_roller_set_options(roller, options_buffer, LV_ROLLER_MODE_INFINITE);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_activateButtonSetupSelector(bool active)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.button_setup_roller;
	lv_obj_set_state(roller, LV_STATE_CHECKED, active);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_selectSettings(uint8_t selected_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.knob_setup_roller;
	lv_roller_set_selected(roller, selected_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_loadSettingsOptions(uint8_t screen_index, bool simple_screen_enabled)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.knob_setup_roller;
	char screen_str[8] = {};
	if(screen_index == 0xFF) {
		strncpy(screen_str, "all", sizeof(screen_str));
	} else {
		snprintf(screen_str, sizeof(screen_str), "%u", (unsigned)(screen_index + 1));
	}
	const char *simple_str = simple_screen_enabled ? "On" : "Off";
	char options_buffer[64] = {};
	snprintf(options_buffer, sizeof(options_buffer),
		"Screen: %s\nSimple screen: %s\nReturn",
		screen_str,
		simple_str
	);
	lv_roller_set_options(roller, options_buffer, LV_ROLLER_MODE_INFINITE);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_activateSettingsSelector(bool active)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	display.set_active_display(0);
	lv_obj_t * roller = objects.knob_setup_roller;
	lv_obj_set_style_text_color(roller, lv_color_hex(0xff000000), LV_PART_SELECTED | LV_STATE_CHECKED);
	lv_obj_set_style_border_width(roller, 2, LV_PART_SELECTED | LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(roller, lv_color_hex(0xffdedede), LV_PART_SELECTED | LV_STATE_CHECKED);
	lv_obj_set_state(roller, LV_STATE_CHECKED, active);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::setSimpleMode(uint8_t display_id, bool enabled)
{
	if(display_id > 15) {
		return;
	}
	simple_mode[display_id] = enabled;
}

bool UI::isSimpleMode(uint8_t display_id) const
{
	if(display_id > 15) {
		return false;
	}
	return simple_mode[display_id];
}


#if TEST_UI
void TaskLVGL_test_ui_task(void const *arg)
{
	xSemaphoreTake(ui_ready_sem, portMAX_DELAY);

	for (int i = 0; i < 16; i++)
	{
		show_string_t show_name;
		show_name.display_id = i;
		sprintf(show_name.str, "Display %d", i);
		xQueueSend(show_name_queue, &show_name, portMAX_DELAY);
	}

	while (1)
	{
		osDelay(1000);
	}
}
#endif
