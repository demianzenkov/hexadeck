/*
 * task_buttons.c
 *
 */

#include "task_buttons.h"
#include "task_midi.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "usbd_cdc_acm_if.h"
#include "bootloader.h"


// #define CONFIG_BUTTONS_CALIBRATION_ENABLED 1


typedef struct {
	uint16_t median;
} ButtonAdcMedian;


// serial 005 has these median values

const ButtonAdcMedian button_adc_medians[16] = {
	// i*4+0, i*4+1, i*4+2, i*4+3 for i=0..3
	{728}, // 0
	{770}, // 1
	{820}, // 2
	{900}, // 3
	{728}, // 4
	{770}, // 5
	{820}, // 6
	{900}, // 7
	{728}, // 8
	{770}, // 9
	{820}, // 10
	{900}, // 11
	{728}, // 12
	{770}, // 13
	{820}, // 14
	{900}, // 15
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

static void TaskButtons_task(void const *arg);

osThreadId buttonsTaskHandle;
osSemaphoreId buttons_ready_sem;
extern ADC_HandleTypeDef hadc1;

volatile uint32_t adc_counter = 0;
uint8_t adc_i = 0;
volatile uint32_t adc_values[4] = {};
volatile uint8_t data_sampled = 1;
uint8_t button_state[16] = {};
uint8_t prev_button_state[16] = {};
uint32_t button_changed_time[16] = {};
uint8_t button_changed[16] = {};
uint32_t bootloader_entry_start = 0;

void TaskButtons_createTask() {
    buttons_ready_sem = xSemaphoreCreateBinary();

    osThreadDef(buttonsTask, TaskButtons_task, osPriorityNormal, 0, 256);
	buttonsTaskHandle = osThreadCreate(osThread(buttonsTask), NULL);
}


void TaskButtons_task(void const *arg) {

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, 4);
	uint32_t last_calib_send = 0;
	while (1) {
		if(xSemaphoreTake(buttons_ready_sem, portMAX_DELAY) == pdTRUE) {
			uint32_t now = HAL_GetTick();
			// Reset button states
			for(int i = 0; i < 16; i++) {
				button_state[i] = 0;
			}

			// Check if ADC0.ch0 is 568 +-20 & ADC0.ch3 is 570 +-20 for 3 seconds to enter bootloader
			if( (adc_values[0] >= (568 - 20)) && (adc_values[0] <= (568 + 20)) &&
				(adc_values[3] >= (570 - 20)) && (adc_values[3] <= (570 + 20)) ) {

				if(bootloader_entry_start == 0) {
					bootloader_entry_start = now;
				} else {
					if((now - bootloader_entry_start) >= 3000) {
						// Enter bootloader
						CDC_Transmit(0, (uint8_t *)"Entering bootloader...\r\n", 25);
						HAL_Delay(100);
						JumpToBootloader();
					}
				}
			} else {
				// Reset timer
				bootloader_entry_start = 0;
			}

			// Check each ADC channel for its 4 buttons using median ±20
			for(int adc_idx = 0; adc_idx < 4; adc_idx++) {
				uint32_t adc_val = adc_values[adc_idx];
				for(int btn = 0; btn < 4; btn++) {
					int btn_idx = adc_idx * 4 + btn;
					uint16_t median = button_adc_medians[btn_idx].median;
					if(adc_val >= (median - 20) && adc_val <= (median + 20)) {
						button_state[btn_idx] = 1;
					}
				}
			}

#ifdef CONFIG_BUTTONS_CALIBRATION_ENABLED
			// Send ADC values to ACM once per second
			if ((now - last_calib_send) > 3000) {
				char adc_msg[64];
				int len = snprintf(adc_msg, sizeof(adc_msg), "ADC_BUTTONS: %lu,%lu,%lu,%lu\r\n", adc_values[0], adc_values[1], adc_values[2], adc_values[3]);
				CDC_Transmit(0, (uint8_t *)adc_msg, len);
				last_calib_send = now;
			}
#endif

			// ...existing code...
			for(int i=0; i < 16; i++) {
				if((button_state[i] != prev_button_state[i]) && (!button_changed[i])) {
					button_changed[i] = 1;
					button_changed_time[i] = now;
				} else if (button_state[i] == prev_button_state[i]){
					button_changed[i] = 0;
				}
			}
			for (int i = 0; i < 16; i++) {
				if (button_changed[i] && ((now - button_changed_time[i]) > 50)) {

					midi_event_t midi_ev;
					if (button_state[i] == 0) {
						midi_ev.message_type = MIDI_NOTE_OFF;
						midi_ev.value = 0;
					} else {
						midi_ev.message_type = MIDI_NOTE_ON;
						midi_ev.value = 1;
					}
					midi_ev.channel = i;
					midi_ev.note = 0;
					prev_button_state[i] = button_state[i];
					TaskMIDI_sendEvent(&midi_ev);
				}
			}
			HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, 4);
		}
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    xSemaphoreGiveFromISR(buttons_ready_sem, NULL);
}
