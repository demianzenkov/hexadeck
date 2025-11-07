/*
 * task_buttons.c
 *
 */

#include "task_buttons.h"
#include "task_midi.h"
#include "main.h"
#include "cmsis_os.h"

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
uint32_t prev_buttons_pressed_time[16] = {};
uint32_t button_changed_time[16] = {};
uint8_t button_changed[16] = {};

void TaskButtons_createTask() {
    buttons_ready_sem = xSemaphoreCreateBinary();

    osThreadDef(buttonsTask, TaskButtons_task, osPriorityNormal, 0, 256);
	buttonsTaskHandle = osThreadCreate(osThread(buttonsTask), NULL);
}

void TaskButtons_task(void const *arg) {

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, 4);
 	while (1) {
        if(xSemaphoreTake(buttons_ready_sem, portMAX_DELAY) == pdTRUE) {
            uint32_t now = HAL_GetTick();
            for(int i=0; i<4; i++) {
            				
                button_state[i*4] = 0;
                button_state[i*4+1] = 0;
                button_state[i*4+2] = 0;
                button_state[i*4+3] = 0;
            
                if((adc_values[i] > 685) && (adc_values[i] < 710)) {
					// Button i.0
					button_state[i*4] = 1;
				}
                else if((adc_values[i] > 720) && (adc_values[i] < 750)) {
					// Button i.1
					button_state[i*4+1] = 1;
				}
                else if((adc_values[i] > 770) && (adc_values[i] < 800)) {
					// Button i.2
					button_state[i*4+2] = 1;
				}
                else if((adc_values[i] > 830) && (adc_values[i] < 870)) {
                    // Button i.3
                    button_state[i*4+3] = 1;
                }

                // TODO: Handle several buttons together pressed
            }

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
