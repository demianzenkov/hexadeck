/*
 * task_buttons.c
 *
 */

#include "task_buttons.h"
#include "task_os.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "usbd_cdc_acm_if.h"
#include "bootloader.h"


// #define CONFIG_BUTTONS_CALIBRATION_ENABLED 1

// Serial 002
// #define CORNER_BUTTONS_ADC_CH0  568
// #define CORNER_BUTTONS_ADC_CH3  570

// Serial 005
#define CORNER_BUTTONS_ADC_CH0  600
#define CORNER_BUTTONS_ADC_CH3  560

#define CENTRAL_BUTTONS_ADC_CH1_CH2  560

typedef struct {
	uint16_t median;
} ButtonAdcMedian;



// serial 005 has these median values
const ButtonAdcMedian button_adc_medians[16] = {
	{737}, // 0
	{778}, // 1
	{838}, // 2
	{906}, // 3
	{695}, // 4
	{733}, // 5
	{812}, // 6
	{848}, // 7
	{696}, // 8
	{734}, // 9
	{786}, // 10
	{849}, // 11
	{698}, // 12
	{734}, // 13
	{790}, // 14
	{852}, // 15
};

// 729,697,694,693 - 
// 769,733,732,730 - 
// 828,787,787,786 - 
// 894,848,848,846 - 
// serial 002 has these median values
/*
const ButtonAdcMedian button_adc_medians[16] = {
	// i*4+0, i*4+1, i*4+2, i*4+3 for i=0..3
	{729}, // 0
	{770}, // 1
	{828}, // 2
	{894}, // 3
	{696}, // 4
	{733}, // 5
	{789}, // 6
	{848}, // 7
	{694}, // 8
	{733}, // 9
	{787}, // 10
	{847}, // 11
	{692}, // 12
	{730}, // 13
	{786}, // 14
	{848}, // 15
};
*/

extern ADC_HandleTypeDef hadc1;


Buttons * Buttons::getInstance() {
	static Buttons instance;
	return &instance;
}

void Buttons::createTask() {
    buttons_ready_sem = xSemaphoreCreateBinary();

    osThreadDef(buttonsTask, task, osPriorityNormal, 0, 256);
	task_handle = osThreadCreate(osThread(buttonsTask), this);
}


void Buttons::task(void const *arg)
{
	Buttons *p_this = (Buttons *)arg;

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)p_this->adc_values, 4);
	uint32_t last_calib_send = 0;
	while (1) {
		if(xSemaphoreTake(p_this->buttons_ready_sem, portMAX_DELAY) == pdTRUE) {
			uint32_t now = HAL_GetTick();
			// Reset button states
			for(int i = 0; i < 16; i++) {
				p_this->button_state[i] = 0;
			}
			
			// Check if ADC0.ch1 & ADC0.ch2 is CENTRAL_BUTTONS_ADC_CH1_CH2 +-20 500ms to fire hold event
			if( (p_this->adc_values[1] >= (CENTRAL_BUTTONS_ADC_CH1_CH2 - 30)) && (p_this->adc_values[1] <= (CENTRAL_BUTTONS_ADC_CH1_CH2 + 30)) &&
				(p_this->adc_values[2] >= (CENTRAL_BUTTONS_ADC_CH1_CH2 - 30)) && (p_this->adc_values[2] <= (CENTRAL_BUTTONS_ADC_CH1_CH2 + 30)) ) {

				if(p_this->menu_entry_start == 0) {
					p_this->menu_entry_start = now;
				} else {
					if((now - p_this->menu_entry_start) >= 100) {
						button_event_t button_ev;
						button_ev.type = BUTTON_EVENT_CENTRAL_QUAD_PRESS;
						xQueueSend(task_os.button_event_queue, &button_ev, 0);
					}
				}
			} else {
				// Reset timer
				p_this->menu_entry_start = 0;
			}

			// Check each ADC channel for its 4 buttons using median ±20
			for(int adc_idx = 0; adc_idx < 4; adc_idx++) {
				uint32_t adc_val = p_this->adc_values[adc_idx];
				for(int btn = 0; btn < 4; btn++) {
					int btn_idx = adc_idx * 4 + btn;
					uint16_t median = button_adc_medians[btn_idx].median;
					if((adc_val >= (median - 20)) && (adc_val <= (median + 20))) {
						p_this->button_state[btn_idx] = 1;
					}
				}
			}

#ifdef CONFIG_BUTTONS_CALIBRATION_ENABLED
			// Send ADC values to ACM once per second
			if ((now - last_calib_send) > 3000) {
				char adc_msg[64];
				int len = snprintf(adc_msg, sizeof(adc_msg), "ADC_BUTTONS: %lu,%lu,%lu,%lu\r\n", p_this->adc_values[0], p_this->adc_values[1], p_this->adc_values[2], p_this->adc_values[3]);
				CDC_Transmit(0, (uint8_t *)adc_msg, len);
				last_calib_send = now;
			}
#endif

			// ...existing code...
			for(int i=0; i < 16; i++) {
				if((p_this->button_state[i] != p_this->prev_button_state[i]) && (!p_this->button_changed[i])) {
					p_this->button_changed[i] = 1;
					p_this->button_changed_time[i] = now;
				} else if (p_this->button_state[i] == p_this->prev_button_state[i]){
					p_this->button_changed[i] = 0;
				}
			}
			for (int i = 0; i < 16; i++) {
				if (p_this->button_changed[i] && ((now - p_this->button_changed_time[i]) > 50)) {
					button_event_t button_ev;
					button_ev.type = BUTTON_EVENT_SINGLE_PRESS;
					button_ev.button_id = i;
					button_ev.state = p_this->button_state[i];
					xQueueSend(task_os.button_event_queue, &button_ev, 0);
					p_this->prev_button_state[i] = p_this->button_state[i];
				}
			}
			HAL_ADC_Start_DMA(&hadc1, (uint32_t *)p_this->adc_values, 4);
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    xSemaphoreGiveFromISR(Buttons::getInstance()->buttons_ready_sem, NULL);
}
