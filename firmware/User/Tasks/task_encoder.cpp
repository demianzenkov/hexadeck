/*
 * task_encoder.c
 *
 */

#include "task_encoder.h"
#include "task_lvgl.h"
#include "task_midi.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"

#define ENC_DEBOUNCE_HIGH_FILTER    50
#define ENC_DEBOUNCE_LOW_FILTER     3000
#define ENC_DEBOUNCE_CMD_FILTER		pdMS_TO_TICKS(250)
#define ENC_DEFAULT_VALUE			64
static void TaskEncoder_task(void const *arg);

osThreadId encoderTaskHandle;
QueueHandle_t encoder_queue = 0;
encoder_ev_t encoder_ev[16] = {{0, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {2, 0, 0, 0, 0}, {3, 0, 0, 0, 0}, {4, 0, 0, 0, 0}, {5, 0, 0, 0, 0}, {6, 0, 0, 0, 0}, {7, 0, 0, 0, 0}, {8, 0, 0, 0, 0}, {9, 0, 0, 0, 0}, {10, 0, 0, 0, 0}, {11, 0, 0, 0, 0}, {12, 0, 0, 0, 0}, {13, 0, 0, 0, 0}, {14, 0, 0, 0, 0}, {15, 0, 0, 0, 0}};
// encoder_ev_t encoder_ev[16] = {{0, 0, 0, 0, 0}, {4, 0, 0, 0, 0}, {8, 0, 0, 0, 0}, {12, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {5, 0, 0, 0, 0}, {9, 0, 0, 0, 0}, {13, 0, 0, 0, 0}, {2, 0, 0, 0, 0}, {6, 0, 0, 0, 0}, {10, 0, 0, 0, 0}, {14, 0, 0, 0, 0}, {3, 0, 0, 0, 0}, {7, 0, 0, 0, 0}, {11, 0, 0, 0, 0}, {15, 0, 0, 0 ,0}};

uint8_t encoder_values[16] = {};
int32_t encoder_delta_values[16] = {};
uint32_t ev_counter[16] = {};
uint32_t ev_last_time[16] = {};

midi_event_t enc_midi_ev = {};

void TaskEncoder_createTask() {
    encoder_queue = xQueueCreate(50, sizeof(encoder_ev_t));

	osThreadDef(encoderTask, TaskEncoder_task, osPriorityNormal, 0, 1024);
	encoderTaskHandle = osThreadCreate(osThread(encoderTask), NULL);
}

 void TaskEncoder_task(void const *arg) {
    // Set default levels to encoder values & send them to displays
   for(uint8_t i = 0; i < 16; i++) {
       encoder_values[i] = ENC_DEFAULT_VALUE;
       enc_midi_ev.message_type = MIDI_CC;
       enc_midi_ev.channel = i;
       enc_midi_ev.note = MIDI_CC_MODULATION;
       enc_midi_ev.value = encoder_values[i];
       TaskMIDI_sendEvent(&enc_midi_ev);
       ui.showBarLevel(i, encoder_values[i]);
   }
    
 	while (1) {
 		// Encoders to read without interrupts: ENC24, ENC41
		
         // Read event from interrupt: ENC11, ENC12, ENC13, ENC14, ENC21, ENC22, ENC23, ENC31, ENC32, ENC33, ENC34, ENC42, ENC43, ENC44
         encoder_ev_t ev;
         if(xQueueReceive(encoder_queue, &ev, portMAX_DELAY) == pdTRUE) {
                 if(ev.prev_state_a == 0 && ev.state_a == 1) {
                     if(ev.prev_state_b == 0 && ev.state_b == 1) {
                     	if((ev.encoder_id != 15) && (ev.encoder_id != 11)) {
                            if(encoder_values[ev.encoder_id] > ENCODER_MIN_VALUE) {
                                encoder_values[ev.encoder_id]--;
							}
                         } else {
							if(encoder_values[ev.encoder_id] < ENCODER_MAX_VALUE) {
								encoder_values[ev.encoder_id]++;
							}
                         }
                         
                     	 enc_midi_ev.message_type = MIDI_CC;
						 enc_midi_ev.channel = ev.encoder_id;
						 enc_midi_ev.note = MIDI_CC_MODULATION;
						 enc_midi_ev.value = encoder_values[ev.encoder_id];

                         TaskMIDI_sendEvent(&enc_midi_ev);
                         ui.showBarLevel(ev.encoder_id, encoder_values[ev.encoder_id]);
                     }
                 } else if(ev.prev_state_a == 1 && ev.state_a == 0) {
                     if(ev.prev_state_b == 0 && ev.state_b == 1) {
                         if((ev.encoder_id != 15) && (ev.encoder_id != 11)) {
							if(encoder_values[ev.encoder_id] < ENCODER_MAX_VALUE) {
								encoder_values[ev.encoder_id]++;
							}
                         } else {
							if(encoder_values[ev.encoder_id] > ENCODER_MIN_VALUE) {
								encoder_values[ev.encoder_id]--;
							}
                         }
                         
                         enc_midi_ev.message_type = MIDI_CC;
                         enc_midi_ev.channel = ev.encoder_id;
                         enc_midi_ev.note = MIDI_CC_MODULATION;
                         enc_midi_ev.value = encoder_values[ev.encoder_id];
                         
                         TaskMIDI_sendEvent(&enc_midi_ev);
                         ui.showBarLevel(ev.encoder_id, encoder_values[ev.encoder_id]);
                     }
                 }
         }
 	}
 }
