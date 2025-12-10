#include "task_os.h"

#include "task_buttons.h"
#include "task_encoder.h"
#include "task_lvgl.h"
#include "task_midi.h"
#include "task_acm.h"
#include "bootloader.h"

TaskOS task_os;


TaskOS::TaskOS()
{
	for (uint8_t i = 0; i < 16; i++)
	{
		module_states[i].channel = MIDI_DEFAULT_CHANNEL;	// channel
		module_states[i].cc = i;							// cc number
		module_states[i].min_value = ENCODER_DEFAULT_MIN_VALUE;
		module_states[i].max_value = ENCODER_DEFAULT_MAX_VALUE;
		module_states[i].current_value = ENCODER_DEFAULT_VALUE;
		module_states[i].step = ENCODER_DEFAULT_STEP;
	}
}

void TaskOS::createTask()
{
	encoder_event_queue = xQueueCreate(32, sizeof(encoder_event_t));
	acm_event_queue = xQueueCreate(16, sizeof(acm_event_t));
	midi_input_event_queue = xQueueCreate(32, sizeof(midi_event_t));
	midi_sysex_input_event_queue = xQueueCreate(8, sizeof(midi_sysex_event_t));
	button_event_queue = xQueueCreate(8, sizeof(button_event_t));
	
	ui.createTask();
	task_midi.createTask();
	buttons.createTask();
	task_encoder.createTask();
	acm.createTask();

	osThreadDef(OSTask, task, osPriorityNormal, 0, 512);
	task_handle = osThreadCreate(osThread(OSTask), this);
}


void TaskOS::task(void const *arg)
{
	TaskOS *p_this = (TaskOS *)arg;

	encoder_event_t encoder_event;
	acm_event_t acm_event;
	midi_event_t midi_input_ev;
	midi_sysex_event_t sysex_input_ev;
	button_event_t button_ev;
	while (1)
	{
		if(xQueueReceive(p_this->encoder_event_queue, &encoder_event, 0) == pdTRUE) {
			uint8_t enc_id = encoder_event.encoder_id;
			if(enc_id < 16) {
				module_state_t * module_state = &p_this->module_states[enc_id];
				if(encoder_event.increase) {
					if(module_state->current_value + module_state->step <= module_state->max_value) {
						module_state->current_value += module_state->step;
					} else {
						module_state->current_value = module_state->max_value;
					}
				} else {
					if(module_state->current_value >= (module_state->min_value + module_state->step)) {
						module_state->current_value -= module_state->step;
					} else {
						module_state->current_value = module_state->min_value;
					}
				}
				task_midi.sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
				// Update UI
				ui.setValue(enc_id, module_state->current_value);
			}
		}
		
		if(xQueueReceive(p_this->acm_event_queue, &acm_event, 0) == pdTRUE) {
			switch(acm_event.type) {
				case ACM_EVENT_TYPE_SET_NAME: {
					if(acm_event.id < 16) {
						memset(p_this->module_states[acm_event.id].name, 0, sizeof(p_this->module_states[acm_event.id].name));
						strncpy((char *)p_this->module_states[acm_event.id].name, (char *)acm_event.data, 16);
						ui.setName(acm_event.id, (const char *)p_this->module_states[acm_event.id].name);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_VALUE: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if(	(acm_event.data[0] >= module_state->min_value) && 
						(acm_event.data[0] <= module_state->max_value)) {
							// TODO: check if value correlates with step
							module_state->current_value = acm_event.data[0];
							// Send MIDI CC event
							task_midi.sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
							// Update UI
							ui.setValue(acm_event.id, module_state->current_value);
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_CHANNEL: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if((acm_event.data[0] >= 1) && (acm_event.data[0] <= 16)) {
							module_state->channel = acm_event.data[0];
							ui.setChannel(acm_event.id, module_state->channel);
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_CC: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if(acm_event.data[0] <= 127) {
							module_state->cc = acm_event.data[0];
							ui.setCC(acm_event.id, module_state->cc);
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_RANGE: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if((acm_event.data[0] >= module_state->min_value) && (acm_event.data[0] <= 127)) {
							module_state->max_value = acm_event.data[0];
							ui.setRange(acm_event.id, module_state->max_value);
							// Adjust current value if needed
							if(module_state->current_value > module_state->max_value) {
								module_state->current_value = module_state->max_value;
								// Send MIDI CC event
								task_midi.sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
								// Update UI
								ui.setValue(acm_event.id, module_state->current_value);
							}
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BG: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						ui.setColor(acm_event.id, COLOR_ELEMENT_BACKGROUND, color);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BORDER: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						ui.setColor(acm_event.id, COLOR_ELEMENT_BORDER, color);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_TEXT: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						ui.setColor(acm_event.id, COLOR_ELEMENT_TEXT, color);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BAR: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						ui.setColor(acm_event.id, COLOR_ELEMENT_BAR, color);
					}
					break;
				}
				case ACM_EVENT_FIRMWARE_UPDATE: {
					JumpToBootloader();
					break;
				}
				default:
					break;
			}
		}
		
		if(xQueueReceive(p_this->button_event_queue, &button_ev, 0) == pdTRUE) {
			// Process button events, CC-102..CC-119, values 0/127
			if(button_ev.button_id < 16) {
				uint8_t cc_number = 102 + button_ev.button_id;
				uint8_t cc_value = button_ev.state ? 127 : 0;
				// Send MIDI CC event
				task_midi.sendMidiCC(0, cc_number, cc_value);
			}
		}

		if(xQueueReceive(p_this->midi_input_event_queue, &midi_input_ev, 0) == pdTRUE) {
			vTaskDelay(0);
			// if(midi_input_ev.note == MIDI_MSG_CC_MODULATION) {
				// TODO: process input MIDI CC event
				// if(midi_ev.channel <= 16) {
				// 	if(midi_ev.value > encoder_max_values[midi_ev.channel]) {
				// 		midi_ev.value = encoder_max_values[midi_ev.channel];
				// 	}
				// 	ui.showBarLevel(midi_ev.channel, midi_ev.value);
				// 	task_encoder.encoder_values[midi_ev.channel] = midi_ev.value;
				// }
			// }
		}
		
		if(xQueueReceive(p_this->midi_sysex_input_event_queue, &sysex_input_ev, 0) == pdTRUE) {
			if(sysex_input_ev.len == 0) {
				continue;
			}
			switch(sysex_input_ev.buffer[0]) {
				case MIDI_SYS_SET_NAME: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						memset(p_this->module_states[id].name, 0, sizeof(p_this->module_states[id].name));
						memcpy((char *)p_this->module_states[id].name, &sysex_input_ev.buffer[2], sysex_input_ev.len - 2);
						ui.setName(id, (const char *)p_this->module_states[id].name);
					}
					break;
				}
				case MIDI_SYS_SET_VALUE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t value = sysex_input_ev.buffer[2];
						if(	(value >= module_state->min_value) && 
						(value <= module_state->max_value)) {
							module_state->current_value = acm_event.data[0];
							// Send MIDI CC event
							task_midi.sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
							// Update UI
							ui.setValue(acm_event.id, module_state->current_value);
						}
					}
					break;
				}
				case MIDI_SYS_SET_CHANNEL: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t channel = sysex_input_ev.buffer[2];
						if((channel >= 1) && (channel <= 16)) {
							module_state->channel = channel;
							ui.setChannel(id, module_state->channel);
						}
					}
					break;
				}
				case MIDI_SYS_SET_CC: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t cc = sysex_input_ev.buffer[2];
						if(cc <= 127) {
							module_state->cc = cc;
							ui.setCC(id, module_state->cc);
						}
					}
					break;
				}
				case MIDI_SYS_SET_RANGE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t max_level = sysex_input_ev.buffer[2];
						if((max_level >= module_state->min_value) && (max_level <= 127)) {
							module_state->max_value = max_level;
							ui.setRange(id, module_state->max_value);
							// Adjust current value if needed
							if(module_state->current_value > module_state->max_value) {
								module_state->current_value = module_state->max_value;
								// Send MIDI CC event
								task_midi.sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
								// Update UI
								ui.setValue(id, module_state->current_value);
							}
						}
					}
					break;
				}
				case MIDI_SYS_SET_COLOR_BG: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						// Reconstruct 8-bit RGB from two MIDI bytes per color
						uint8_t r = sysex_input_ev.buffer[2] | (sysex_input_ev.buffer[3] << 7);
						uint8_t g = sysex_input_ev.buffer[4] | (sysex_input_ev.buffer[5] << 7);
						uint8_t b = sysex_input_ev.buffer[6] | (sysex_input_ev.buffer[7] << 7);
						lv_color_t color = lv_color_make(r, g, b);
						ui.setColor(id, COLOR_ELEMENT_BACKGROUND, color);
					}
					break;
				}
				case MIDI_SYS_SET_COLOR_BORDER: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t r = sysex_input_ev.buffer[2] | (sysex_input_ev.buffer[3] << 7);
						uint8_t g = sysex_input_ev.buffer[4] | (sysex_input_ev.buffer[5] << 7);
						uint8_t b = sysex_input_ev.buffer[6] | (sysex_input_ev.buffer[7] << 7);
						lv_color_t color = lv_color_make(r, g, b);
						ui.setColor(id, COLOR_ELEMENT_BORDER, color);
					}
					break;
				}
				case MIDI_SYS_SET_COLOR_TEXT: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t r = sysex_input_ev.buffer[2] | (sysex_input_ev.buffer[3] << 7);
						uint8_t g = sysex_input_ev.buffer[4] | (sysex_input_ev.buffer[5] << 7);
						uint8_t b = sysex_input_ev.buffer[6] | (sysex_input_ev.buffer[7] << 7);
						lv_color_t color = lv_color_make(r, g, b);
						ui.setColor(id, COLOR_ELEMENT_TEXT, color);
					}
					break;
				}
				case MIDI_SYS_SET_COLOR_BAR: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {	
						uint8_t r = sysex_input_ev.buffer[2] | (sysex_input_ev.buffer[3] << 7);
						uint8_t g = sysex_input_ev.buffer[4] | (sysex_input_ev.buffer[5] << 7);
						uint8_t b = sysex_input_ev.buffer[6] | (sysex_input_ev.buffer[7] << 7);
						lv_color_t color = lv_color_make(r, g, b);
						ui.setColor(id, COLOR_ELEMENT_BAR, color);
					}
					break;
				}
				case MIDI_SYS_FIRMWARE_UPDATE: {
					JumpToBootloader();
					break;
				}
				default:
					break;
			}
		}
		vTaskDelay(0);
	}
}
