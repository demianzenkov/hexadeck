/*
 * task_lvgl.c
 *
 *  Created on: Apr 13, 2024
 *      Author: demian
 */

#include "task_lvgl.h"
#include "task_os.h"
#include "cmsis_os.h"
#include "main.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/display/st7735/lv_st7735.h"
#include "lvgl/src/display/lv_display_private.h"
#include "lvgl/src/display/lv_display.h"
#include "lv_lcd_custom_mipi.h"
#include "ui.h"
#include "screens.h"
#include "string.h"
#include "stdio.h"


#define TEST_UI 0

UI ui;
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


UI::UI()
{
	// 	uint8_t display_id;
	// uint8_t value;
	// uint8_t max_value;
	// uint8_t step;
	// uint8_t channel;
	// uint8_t cc;
	// char name[MAX_NAME_LENGTH];
	// lv_color_t background_color;
	// lv_color_t bar_color;
	// lv_color_t text_color;
	// lv_color_t border_color;
	// Initialize the ui_states array
	const ui_state_t init_states[16] = {
		{0,  64, 127, 1, 0, 0, "Bank", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{1,  64, 127, 1, 0, 1, "Wheel", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{2,  64, 127, 1, 0, 2, "Breath", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{3,  64, 127, 1, 0, 3, "CC-3", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{4,  64, 127, 1, 0, 4, "Foot", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{5,  64, 127, 1, 0, 5, "Portamento", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{6,  64, 127, 1, 0, 6, "Data Entry", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{7,  64, 127, 1, 0, 7, "Volume", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{8,	 64, 127, 1, 0, 8, "Balance", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{9,  64, 127, 1, 0, 9, "CC-9", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{10, 64, 127, 1, 0, 10, "Pan", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{11, 64, 127, 1, 0, 11, "Expression", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{12, 64, 127, 1, 0, 12, "Effect-1", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{13, 64, 127, 1, 0, 13, "Effect-2", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{14, 64, 127, 1, 0, 14, "CC-14", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{15, 64, 127, 1, 0, 15, "CC-15", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)}
	};
	// Copy the initialization data to the member array
	for (int i = 0; i < 16; i++) {
		memcpy(&ui_states[i], &init_states[i], sizeof(ui_state_t));
	}
}


void UI::createTask()
{
	lvgl_ready_sem = xSemaphoreCreateBinary();
	ui_busy_mutex = xSemaphoreCreateMutex();
	xSemaphoreGive(ui_busy_mutex);
	xSemaphoreTake(lvgl_ready_sem, 0);

	ui_update_queue = xQueueCreate(64, sizeof(ui_state_t *));

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
	xSemaphoreTake(p_this->lvgl_ready_sem, portMAX_DELAY);

	xSemaphoreTake(p_this->ui_busy_mutex, portMAX_DELAY);
	ui_init();
	loadScreen(SCREEN_ID_MAIN);
	xSemaphoreGive(p_this->ui_busy_mutex);

	p_this->initUiState();
	ui_state_t * ui_state;
	
	for (;;)
	{
		if(xQueueReceive(p_this->ui_update_queue, &ui_state, portMAX_DELAY) == pdTRUE) {
			while((p_this->lcd_disp->flushing) || (p_this->lcd_disp->rendering_in_progress)) {
			}
			p_this->lvgl_setUiState(ui_state);
			// vTaskDelay(5);
		}
	}
}


void UI::taskLVGL(void const *arg)
{
	UI *p_this = (UI *)arg;
	/* Initialize LVGL */
	lv_init();

	for (int i = 0; i < 16; i++)
	{
		set_active_display(i);
		lcd_io_init();
	}

	p_this->lcd_disp = lv_lcd_custom_mipi_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_BGR,
												 lcd_send_cmd, lcd_send_color);

	/* Don't explicitly set color format, let LVGL use its default */
	/* lv_display_set_color_format(lcd_disp, LV_COLOR_FORMAT_RGB565); */

	for (int i = 0; i < 16; i++)
	{
		set_active_display(i);
		lv_lcd_custom_init_controller(p_this->lcd_disp, LV_LCD_FLAG_BGR);
	}

	lv_lcd_custom_mipi_set_callbacks(p_this->lcd_disp);

	for (int i = 0; i < 16; i++)
	{
		set_active_display(i);
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


void UI::initUiState()
{
	for(int i = 0; i < 16; i++) {
		lvgl_setUiState(&ui_states[i]);
		vTaskDelay(50);
	}
}


void UI::lvgl_setUiState(ui_state_t * state)
{
	if (state->display_id > 15)
	{
		return;
	}
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	
	bool force_update = (current_ui_state.display_id != state->display_id);
	current_ui_state.display_id = state->display_id;

	set_active_display(state->display_id);

	// Always update if display_id changed, otherwise only if value changed
	if(force_update || (current_ui_state.value != state->value)) {
		current_ui_state.value = state->value;
		lv_bar_set_value(objects.level_bar, state->value, LV_ANIM_OFF);
		char value_str[6] = {};
		snprintf(value_str, sizeof(value_str), "%d", state->value);
		lv_label_set_text(objects.level_label, value_str);
	}
	if(force_update || (current_ui_state.max_value != state->max_value)) {
		current_ui_state.max_value = state->max_value;
		lv_bar_set_range(objects.level_bar, 0, current_ui_state.max_value);
	}
	if(force_update || (current_ui_state.channel != state->channel) || (state->channel == 0)) {
		current_ui_state.channel = state->channel;
		char channel_str[MAX_CH_LABEL_LENGTH] = {};
		snprintf(channel_str, sizeof(channel_str), "CH-%d", state->channel+1);
		lv_label_set_text(objects.channel_label, channel_str);
	}
	if(force_update || (current_ui_state.cc != state->cc) || (state->cc == 0)) {
		current_ui_state.cc = state->cc;
		char cc_str[MAX_CC_LABEL_LENGTH] = {};
		snprintf(cc_str, sizeof(cc_str), "CC-%d", state->cc);
		lv_label_set_text(objects.cc_label, cc_str);
	}
	if(force_update || (strcmp(current_ui_state.name, state->name) != 0)) {
		strncpy(current_ui_state.name, state->name, sizeof(current_ui_state.name));
		lv_label_set_text(objects.name_label, state->name);
	}
	if(force_update || (lv_color_eq(current_ui_state.bar_color, state->bar_color) == false)) {
		current_ui_state.bar_color = state->bar_color;
		lv_obj_set_style_bg_color(objects.level_bar, state->bar_color, LV_PART_INDICATOR | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(objects.level_bar, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	}
	if(force_update || (lv_color_eq(current_ui_state.border_color, state->border_color) == false)) {
		current_ui_state.border_color = state->border_color;
		lv_obj_set_style_border_color(objects.general_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_color(objects.channel_pannel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_color(objects.name_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_color(objects.cc_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_border_color(objects.range_panel, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	}
	if(force_update || (lv_color_eq(current_ui_state.text_color, state->text_color) == false)) {
		current_ui_state.text_color = state->text_color;
		lv_obj_set_style_text_color(objects.name_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(objects.channel_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(objects.level_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(objects.cc_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(objects.range_label, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	}
	if(force_update || (lv_color_eq(current_ui_state.background_color, state->background_color) == false)) {
		current_ui_state.background_color = state->background_color;
		lv_obj_set_style_bg_color(objects.general_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_color(objects.channel_pannel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_color(objects.name_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_color(objects.cc_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_color(objects.range_panel, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
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
	set_active_display(display_id);
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


void UI::lvgl_selectMidiBank(uint8_t bank_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	lv_obj_t * roller = objects.midi_banks_roller;
	lv_roller_set_selected(roller, bank_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_selectUiPreset(uint8_t preset_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	lv_obj_t * roller = objects.ui_presets_roller;
	lv_roller_set_selected(roller, preset_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_selectMidiUnit(uint8_t unit_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	lv_obj_t * roller = objects.config_midi_roller;
	lv_roller_set_selected(roller, unit_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_loadMidiUnitParameters(void * module_state)
{
	module_state_t * state = (module_state_t *)module_state;
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	lv_obj_t * roller = objects.config_midi_unit_roller;
	uint8_t options_buffer[64] = {};
	snprintf((char *)options_buffer, sizeof(options_buffer), "Channel: %d\nCC: %d\nReturn", state->channel + 1, state->cc);
	lv_roller_set_options(roller, (char *)options_buffer, LV_ROLLER_MODE_INFINITE);
	xSemaphoreGive(ui_busy_mutex);
}


void UI::lvgl_selectMidiParameter(uint8_t parameter_menu_index)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	lv_obj_t * roller = objects.config_midi_unit_roller;
	lv_roller_set_selected(roller, parameter_menu_index, LV_ANIM_OFF);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_activateChannelSelector(bool active)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	// if active - set roller to checked state
	lv_obj_t * roller = objects.config_midi_unit_roller;
	lv_obj_set_state(roller, LV_STATE_CHECKED, active);
	xSemaphoreGive(ui_busy_mutex);
}

void UI::lvgl_activateCCSelector(bool active)
{
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	set_active_display(0);
	lv_obj_t * roller = objects.config_midi_unit_roller;
	lv_obj_set_state(roller, LV_STATE_CHECKED, active);
	xSemaphoreGive(ui_busy_mutex);
}


void UI::setRange(uint8_t disp, uint8_t max_level)
{
	if(max_level > 127) {
		return;
	}
	ui_states[disp].max_value = max_level;
	// current_ui_state.max_value = max_level;
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}


void UI::setValue(uint8_t disp, uint8_t value)
{
	if (value > 127)
	{
		return;
	}
	ui_states[disp].value = value;
	// current_ui_state.value = value;
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}


void UI::setName(uint8_t disp, const char *str)
{
	if (strlen(str) > MAX_NAME_LENGTH - 1)
	{
		return;
	}
	
	strncpy(ui_states[disp].name, str, sizeof(ui_states[disp].name));
	// strncpy(current_ui_state.name, str, sizeof(current_ui_state.name));

	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}


void UI::setChannel(uint8_t disp, uint8_t channel)
{
	if(channel > 16) {
		return;
	}
	// current_ui_state.channel = channel;
	ui_states[disp].channel = channel;
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}


void UI::setCC(uint8_t disp, const uint8_t cc)
{
	if(cc > 127) {
		return;
	}
	// current_ui_state.cc = cc;
	ui_states[disp].cc = cc;
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}


void UI::setColor(uint8_t disp, color_element_e element, lv_color_t color)
{
	switch(element) {
		case COLOR_ELEMENT_BACKGROUND:
			ui_states[disp].background_color = color;
			// current_ui_state.background_color = color;
			break;
		case COLOR_ELEMENT_BORDER:
			ui_states[disp].border_color = color;
			// current_ui_state.border_color = color;
			break;
		case COLOR_ELEMENT_TEXT:
			ui_states[disp].text_color = color;
			// current_ui_state.text_color = color;
			break;
		case COLOR_ELEMENT_BAR:
			ui_states[disp].bar_color = color;
			// current_ui_state.bar_color = color;
			break;
		default:
			return;
	}
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}

void UI::updateState(uint8_t disp)
{
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
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
