/*
 * task_lvgl.c
 *
 *  Created on: Apr 13, 2024
 *      Author: demian
 */

#include "task_lvgl.h"
#include "cmsis_os.h"
#include "main.h"
#include "lvgl.h"
#include "src/drivers/display/st7735/lv_st7735.h"
#include "lv_lcd_custom_mipi.h"
#include "src/display/lv_display.h"
#include "src/display/lv_display_private.h"
#include "ui.h"
#include "screens.h"
#include "task_lvgl.h"
#include "string.h"
#include "stdio.h"

#define TEST_UI 0

UI ui;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;

osThreadId lvglTaskHandle;
osThreadId uiTaskHandle;

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
	// Initialize the ui_states array
	ui_state_t init_states[16] = {
		{0,  0, 127, "Name0", "0", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{1,  0, 127, "Name1", "1", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{2,  0, 127, "Name2", "2", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{3,  0, 127, "Name3", "3", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{4,  0, 127, "Name4", "4", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{5,  0, 127, "Name5", "5", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{6,  0, 127, "Name6", "6", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{7,  0, 127, "Name7", "7", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{8,  0, 127, "Name8", "8", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{9,	 0, 127, "Name9", "9", "0.00", 	lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{10, 0, 127, "Name10", "10", "0.00", lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{11, 0, 127, "Name11", "11", "0.00", lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{12, 0, 127, "Name12", "12", "0.00", lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{13, 0, 127, "Name13", "13", "0.00", lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{14, 0, 127, "Name14", "14", "0.00", lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{15, 0, 127, "Name15", "15", "0.00", lv_color_make(0x1e, 0x1e, 0x1e),	lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)}
	};
	// Copy the initialization data to the member array
	for (int i = 0; i < 16; i++) {
		memcpy(&ui_states[i], &init_states[i], sizeof(ui_state_t));
	}

	current_ui_state = {};
}

void UI::createTask()
{
	lvgl_ready_sem = xSemaphoreCreateBinary();
	ui_busy_mutex = xSemaphoreCreateMutex();
	xSemaphoreGive(ui_busy_mutex);

	// show_level_queue = xQueueCreate(16, sizeof(show_level_t));
	// show_name_queue = xQueueCreate(16, sizeof(show_string_t));
	// show_value_queue = xQueueCreate(16, sizeof(show_string_t));
	// show_channel_queue = xQueueCreate(16, sizeof(show_string_t));
	// show_color_queue = xQueueCreate(16, sizeof(show_color_t));
	// show_image_queue = xQueueCreate(16, sizeof(show_img_t));

	ui_update_queue = xQueueCreate(32, sizeof(ui_state_t *));

	osThreadDef(lvglTask, taskLVGL, osPriorityAboveNormal, 0, 1024);
	lvglTaskHandle = osThreadCreate(osThread(lvglTask), this);

	osThreadDef(uiTask, taskUI, osPriorityNormal, 0, 2048);
	uiTaskHandle = osThreadCreate(osThread(uiTask), this);

#if TEST_UI
	osThreadDef(testUITask, TaskLVGL_test_ui_task, osPriorityNormal, 0, 1024);
	testUITaskHandle = osThreadCreate(osThread(testUITask), NULL);
#endif
}

	
void UI::taskUI(void const *arg)
{
	UI *p_this = (UI *)arg;
	xSemaphoreTake(p_this->lvgl_ready_sem, portMAX_DELAY);

	for (int i = 0; i < 16; i++)
	{
		xSemaphoreTake(p_this->ui_busy_mutex, portMAX_DELAY);
		set_active_display(i);
		ui_init();
		loadScreen(SCREEN_ID_MAIN);
		xSemaphoreGive(p_this->ui_busy_mutex);
		osDelay(30);
	}
	p_this->lvgl_initUiState();
	ui_state_t * ui_state;
	
	for (;;)
	{
		if(xQueueReceive(p_this->ui_update_queue, &ui_state, 1) == pdTRUE) {
			while((p_this->lcd_disp->flushing) || (p_this->lcd_disp->rendering_in_progress))
			;
			p_this->lvgl_setUiState(ui_state);
			osDelay(5);
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
		lv_display_set_rotation(p_this->lcd_disp, LV_DISPLAY_ROTATION_270);
		lv_display_set_resolution(p_this->lcd_disp, LCD_H_PHYSICAL_RES, LCD_V_PHYSICAL_RES);
		lv_st7735_set_invert(p_this->lcd_disp, 1);
		lv_display_set_offset(p_this->lcd_disp, (LCD_V_RES - LCD_V_PHYSICAL_RES) / 2, (LCD_H_RES - LCD_H_PHYSICAL_RES) / 2);
	}

	/* Allocate draw buffers on the heap. In this example we use two partial buffers of 1/10th size of the screen */
	lv_color_t *buf1 = NULL;
	lv_color_t *buf2 = NULL;

	uint32_t buf_size = LCD_V_PHYSICAL_RES * LCD_DRAW_BUFF_HEIGHT * lv_color_format_get_size(lv_display_get_color_format(p_this->lcd_disp));

	buf1 = (lv_color_t *)lv_malloc(buf_size);
	if (buf1 == NULL)
	{
		LV_LOG_ERROR("display draw buffer malloc failed");
		return;
	}

	buf2 = NULL;
	// buf2 = (lv_color_t *)lv_malloc(buf_size);
	// if (buf2 == NULL) {
	// 	LV_LOG_ERROR("display buffer malloc failed");
	// 	lv_free(buf1);
	// 	return;
	// }
	lv_display_set_buffers(p_this->lcd_disp, buf1, buf2, buf_size,
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

void UI::lvgl_initUiState()
{
	for(int i = 0; i < 16; i++) {
		memset(&current_ui_state, 0, sizeof(current_ui_state));
		lvgl_setUiState(&ui_states[i]);
		osDelay(50);
	}
}
void UI::lvgl_setUiState(ui_state_t * state)
{
	uint8_t display_id = state->display_id;
	if (display_id > 15)
	{
		return;
	}
	xSemaphoreTake(ui_busy_mutex, portMAX_DELAY);
	current_ui_state.display_id = state->display_id;
	set_active_display(display_id);
	if(current_ui_state.max_bar_level != state->max_bar_level) {
		current_ui_state.max_bar_level = state->max_bar_level;
		lv_bar_set_range(objects.bar_level, 0, current_ui_state.max_bar_level);
	}
	if(current_ui_state.bar_level != state->bar_level) {
		current_ui_state.bar_level = state->bar_level;
		lv_bar_set_value(objects.bar_level, state->bar_level, LV_ANIM_OFF);
	}
	if(strcmp(current_ui_state.name, state->name) != 0) {
		strncpy(current_ui_state.name, state->name, sizeof(current_ui_state.name));
		lv_label_set_text(objects.label_name, state->name);
	}
	if(strcmp(current_ui_state.channel, state->channel) != 0) {
		strncpy(current_ui_state.channel, state->channel, sizeof(current_ui_state.channel));
		lv_label_set_text(objects.label_channel, state->channel);
	}
	if(strcmp(current_ui_state.value, state->value) != 0) {
		strncpy(current_ui_state.value, state->value, sizeof(current_ui_state.value));
		lv_label_set_text(objects.label_level, state->value);
	}
	if(lv_color_eq(current_ui_state.bar_color, state->bar_color) == false) {
		current_ui_state.bar_color = state->bar_color;
		lv_obj_t *obj = objects.bar_level;
		lv_obj_set_style_bg_color(obj, state->bar_color, LV_PART_INDICATOR | LV_STATE_DEFAULT);
		lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_INDICATOR | LV_STATE_DEFAULT);
	}
	if(lv_color_eq(current_ui_state.border_color, state->border_color) == false) {
		current_ui_state.border_color = state->border_color;
		lv_obj_t *obj = NULL;
		obj = objects.panel_general;
		lv_obj_set_style_border_color(obj, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		obj = objects.panel_channel;
		lv_obj_set_style_border_color(obj, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		obj = objects.panel_text;
		lv_obj_set_style_border_color(obj, state->border_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	}
	if(lv_color_eq(current_ui_state.text_color, state->text_color) == false) {
		current_ui_state.text_color = state->text_color;
		lv_obj_set_style_text_color(objects.label_name, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(objects.label_channel, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		lv_obj_set_style_text_color(objects.label_level, state->text_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	}
	if(lv_color_eq(current_ui_state.background_color, state->background_color) == false) {
		current_ui_state.background_color = state->background_color;
		lv_obj_t *obj = NULL;
		obj = objects.panel_general;
		lv_obj_set_style_bg_color(obj, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		obj = objects.panel_channel;
		lv_obj_set_style_bg_color(obj, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
		obj = objects.panel_text;
		lv_obj_set_style_bg_color(obj, state->background_color, LV_PART_MAIN | LV_STATE_DEFAULT);
	}


	xSemaphoreGive(ui_busy_mutex);
}


void UI::showBarLevel(uint8_t disp, uint8_t level)
{
	if(level > 127) {
		return;
	}
	ui_states[disp].bar_level = level;
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
	// show_level_t show_level = {};
	// show_level.display_id = disp;
	// show_level.level = level;
	// xQueueSend(show_level_queue, &show_level, portMAX_DELAY);
}

void UI::changeBarRange(uint8_t disp, uint8_t max_level)
{
	if(max_level > 127) {
		return;
	}
	ui_states[disp].max_bar_level = max_level;
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
}

void UI::showValue(uint8_t disp, const char *str)
{
	if (strlen(str) > MAX_NAME_LENGTH - 1)
	{
		return;
	}

	memset(ui_states[disp].value, 0, sizeof(ui_states[disp].value));
	strncpy(ui_states[disp].value, str, sizeof(ui_states[disp].value));

	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
	
	// show_string_t show_value = {};
	// show_value.display_id = disp;
	// strncpy(show_value.str, str, sizeof(show_value.str));
	// xQueueSend(show_value_queue, &show_value, portMAX_DELAY);
}

void UI::showName(uint8_t disp, const char *str)
{
	if (strlen(str) > MAX_NAME_LENGTH - 1)
	{
		return;
	}
	memset(ui_states[disp].name, 0, sizeof(ui_states[disp].name));
	strncpy(ui_states[disp].name, str, sizeof(ui_states[disp].name));
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
	// show_string_t show_name = {};
	// show_name.display_id = disp;
	// strncpy(show_name.str, str, sizeof(show_name.str));
	// xQueueSend(show_name_queue, &show_name, portMAX_DELAY);
}

void UI::showChannel(uint8_t disp, const char *str)
{
	if (strlen(str) > MAX_NAME_LENGTH - 1)
	{
		return;
	}
	memset(ui_states[disp].channel, 0, sizeof(ui_states[disp].channel));
	strncpy(ui_states[disp].channel, str, sizeof(ui_states[disp].channel));
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
	// show_string_t show_name = {};
	// show_name.display_id = disp;
	// strncpy(show_name.str, str, sizeof(show_name.str));
	// xQueueSend(show_channel_queue, &show_name, portMAX_DELAY);
}

void UI::showColor(uint8_t disp, color_element_e element, lv_color_t color)
{
	switch(element) {
		case COLOR_ELEMENT_BACKGROUND:
			ui_states[disp].background_color = color;
			break;
		case COLOR_ELEMENT_BORDER:
			ui_states[disp].border_color = color;
			break;
		case COLOR_ELEMENT_TEXT:
			ui_states[disp].text_color = color;
			break;
		case COLOR_ELEMENT_BAR:
			ui_states[disp].bar_color = color;
			break;
		default:
			return;
	}
	ui_state_t * ui_state_pointer = &ui_states[disp];
	xQueueSend(ui_update_queue, &ui_state_pointer, portMAX_DELAY);
	// show_color_t show_color = {};
	// show_color.display_id = disp;
	// show_color.element = element;
	// show_color.color = color;
	// xQueueSend(show_color_queue, &show_color, portMAX_DELAY);
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
