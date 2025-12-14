#include "task_os.h"
#include "bootloader.h"

TaskOS task_os;


TaskOS::TaskOS()
{
	ui_p = &ui;
	task_midi_p = &task_midi;
	buttons_p = &buttons;
	task_encoder_p = &task_encoder;
	acm_p = &acm;

	for (uint8_t i = 0; i < 16; i++)
	{
		module_states[i].channel = MIDI_DEFAULT_CHANNEL;	// channel
		module_states[i].cc = i;							// cc number
		module_states[i].min_value = ENCODER_DEFAULT_MIN_VALUE;
		module_states[i].max_value = ENCODER_DEFAULT_MAX_VALUE;
		module_states[i].current_value = ENCODER_DEFAULT_VALUE;
		module_states[i].step = ENCODER_DEFAULT_STEP;
	}

	current_screen = SCREEN_ID_MAIN;
	current_menu_selection = MENU_SELECT_LOAD_MIDI;
	midi_parameter_channel_selector_active = false;
	midi_parameter_cc_selector_active = false;
}


void TaskOS::createTask()
{
	encoder_event_queue = xQueueCreate(32, sizeof(encoder_event_t));
	acm_event_queue = xQueueCreate(16, sizeof(acm_event_t));
	midi_input_event_queue = xQueueCreate(32, sizeof(midi_event_t));
	midi_sysex_input_event_queue = xQueueCreate(8, sizeof(midi_sysex_event_t));
	button_event_queue = xQueueCreate(8, sizeof(button_event_t));

	ui_p->createTask();
	task_midi_p->createTask();
	buttons_p->createTask();
	task_encoder_p->createTask();
	acm_p->createTask();

	osThreadDef(OSTask, task, osPriorityNormal, 0, 512);
	task_handle = osThreadCreate(osThread(OSTask), this);
}


void TaskOS::processEncoderEvent(encoder_event_t * encoder_event)
{
	uint8_t enc_id = encoder_event->encoder_id;
	if(enc_id < 16) {
		module_state_t * module_state = &module_states[enc_id];
		if(encoder_event->increase) {
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
		task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
		ui_p->setValue(enc_id, module_state->current_value);
	}
}


void TaskOS::processMenuButton()
{
	switch(current_screen) {
		case SCREEN_ID_MENU: {
			switch(current_menu_selection) {
				case MENU_SELECT_LOAD_MIDI: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MIDI_BANKS);
					current_screen = SCREEN_ID_MIDI_BANKS;
					selected_action = ACTION_LOAD;
					break;
				}
				case MENU_SELECT_SAVE_MIDI: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MIDI_BANKS);
					selected_action = ACTION_SAVE;
					current_screen = SCREEN_ID_MIDI_BANKS;
					break;
				}
				case MENU_SELECT_LOAD_UI: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_UI_PRESETS);
					current_screen = SCREEN_ID_UI_PRESETS;
					selected_action = ACTION_LOAD;
					break;
				}
				case MENU_SELECT_SAVE_UI: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_UI_PRESETS);
					current_screen = SCREEN_ID_UI_PRESETS;
					selected_action = ACTION_SAVE;
					break;
				}
				case MENU_SELECT_CONFIG_MIDI: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_CONFIG_MIDI);
					current_screen = SCREEN_ID_CONFIG_MIDI;
					break;
				}
				case MENU_SELECT_AUTOMAPPING: {
					break;
				}
				case MENU_SELECT_FIRMWARE_UPDATE: {
					JumpToBootloader();
					break;
				}
				case MENU_SELECT_EXIT: {
					// TODO: update display states
					ui_p->updateState(0);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MAIN);
					current_screen = SCREEN_ID_MAIN;
					break;
				}
				default:
					break;
			}
			break;
		}
		case SCREEN_ID_MIDI_BANKS: {
			if(midi_bank_selection > 3) {
				ui_p->lvgl_selectMidiBank(0);
				midi_bank_selection = 0;
				ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
				current_screen = SCREEN_ID_MENU;
			}
			break;
		}
		case SCREEN_ID_UI_PRESETS: {
			if(ui_preset_selection > 3) {
				ui_p->lvgl_selectUiPreset(0);
				ui_preset_selection = 0;
				ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
				current_screen = SCREEN_ID_MENU;
			}
			break;
		}
		case SCREEN_ID_CONFIG_MIDI: {
			if(midi_unit_selection > 15) {
				ui_p->lvgl_selectMidiUnit(0);
				midi_unit_selection = 0;
				ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
				current_screen = SCREEN_ID_MENU;
			}
			else {
				ui_p->lvgl_loadMidiUnitParameters(&module_states[midi_unit_selection]);
				ui_p->lvgl_loadScreen(0, SCREEN_ID_CONFIG_MIDI_UNIT);
				current_screen = SCREEN_ID_CONFIG_MIDI_UNIT;
			}
			break;
		}
		case SCREEN_ID_CONFIG_MIDI_UNIT: {
			if(midi_parameter_selection > 1) {
				ui_p->lvgl_selectMidiParameter(0);
				midi_parameter_selection = 0;
				ui_p->lvgl_loadScreen(0, SCREEN_ID_CONFIG_MIDI);
				current_screen = SCREEN_ID_CONFIG_MIDI;
			}
			else if (midi_parameter_selection == 0) {
				if(midi_parameter_channel_selector_active) {
					midi_parameter_channel_selector_active = false;
					// TODO: set unit channel state
					ui_p->setChannel(midi_unit_selection, module_states[midi_unit_selection].channel);
					vTaskDelay(30);
					ui_p->lvgl_loadScreen(midi_unit_selection, SCREEN_ID_MAIN);
					vTaskDelay(30);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_CONFIG_MIDI_UNIT);
					vTaskDelay(30);
				} else {
					midi_parameter_channel_selector_active = true;
				}
				ui_p->lvgl_activateChannelSelector(midi_parameter_channel_selector_active);
			}
			else if(midi_parameter_selection == 1) {
				if(midi_parameter_cc_selector_active) {
					midi_parameter_cc_selector_active = false;
					// TODO: set unit CC state
					ui_p->setCC(midi_unit_selection, module_states[midi_unit_selection].cc);
					vTaskDelay(30);
					ui_p->lvgl_loadScreen(midi_unit_selection, SCREEN_ID_MAIN);
					vTaskDelay(30);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_CONFIG_MIDI_UNIT);
					vTaskDelay(30);
				} else {
					midi_parameter_cc_selector_active = true;
				}
				ui_p->lvgl_activateCCSelector(midi_parameter_cc_selector_active);
			}
			break;
		}
		default:
			break;
	}
}


void TaskOS::processMenuSelector(bool increase)
{
	if(increase) {
		if(current_menu_selection < (MENU_SELECT_COUNT - 1)) {
			current_menu_selection = (menu_select_e)((int)current_menu_selection + 1);
		} else {
			current_menu_selection = (menu_select_e)0;
		}
	} else {
		if(current_menu_selection > 0) {
			current_menu_selection = (menu_select_e)((int)current_menu_selection - 1);
		} else {
			current_menu_selection = (menu_select_e)((int)MENU_SELECT_COUNT - 1);
		}
	}
	ui_p->lvgl_selectMenu(current_menu_selection);
}


void TaskOS::processMidiBankSelector(bool increase)
{
	if(increase) {
		if(midi_bank_selection < 4) {
			midi_bank_selection++;
		} else {
			midi_bank_selection = 0;
		}
	} else {
		if(midi_bank_selection > 0) {
			midi_bank_selection--;
		} else {
			midi_bank_selection = 4;
		}
	}
	ui_p->lvgl_selectMidiBank(midi_bank_selection);
}


void TaskOS::processUiPresetSelector(bool increase)
{
	if(increase) {
		if(ui_preset_selection < 4) {
			ui_preset_selection++;
		} else {
			ui_preset_selection = 0;
		}
	} else {
		if(ui_preset_selection > 0) {
			ui_preset_selection--;
		} else {
			ui_preset_selection = 4;
		}
	}
	ui_p->lvgl_selectUiPreset(ui_preset_selection);
}


void TaskOS::processMidiUnitSelector(bool increase)
{
	if(increase) {
		if(midi_unit_selection < 16) {
			midi_unit_selection++;
		} else {
			midi_unit_selection = 0;
		}
	} else {
		if(midi_unit_selection > 0) {
			midi_unit_selection--;
		} else {
			midi_unit_selection = 16;
		}
	}
	ui_p->lvgl_selectMidiUnit(midi_unit_selection);
}


void TaskOS::processMidiParameterSelector(bool increase)
{
	if(midi_parameter_channel_selector_active) {
		// Channel selector active, update channel state & lvgl_loadMidiUnitParameters
		uint8_t channel = module_states[midi_unit_selection].channel;
		if(increase) {
			if(channel < 15) {
				channel++;
			} else {
				channel = 0;
			}
		} else {
			if(channel > 0) {
				channel--;
			} else {
				channel = 15;
			}
		}
		module_states[midi_unit_selection].channel = channel;
		ui_p->lvgl_loadMidiUnitParameters(&module_states[midi_unit_selection]);
		ui_p->lvgl_selectMidiParameter(midi_parameter_selection);	// to keep roller not updated
	} else if (midi_parameter_cc_selector_active) {
		// CC selector active, update CC state & lvgl_loadMidiUnitParameters
		uint8_t cc = module_states[midi_unit_selection].cc;
		if(increase) {
			if(cc < 127) {
				cc++;
			} else {
				cc = 0;
			}
		} else {
			if(cc > 0) {
				cc--;
			} else {
				cc = 127;
			}
		}
		// setStateCC(midi_unit_selection, cc);
		module_states[midi_unit_selection].cc = cc;
		ui_p->lvgl_loadMidiUnitParameters(&module_states[midi_unit_selection]);
		ui_p->lvgl_selectMidiParameter(midi_parameter_selection);	// to keep roller not updated
	}
	else {
		if(increase) {
			if(midi_parameter_selection < 2) {
				midi_parameter_selection++;
			} else {
				midi_parameter_selection = 0;
			}
		} else {
			if(midi_parameter_selection > 0) {
				midi_parameter_selection--;
			} else {
				midi_parameter_selection = 2;
			}
		}
		ui_p->lvgl_selectMidiParameter(midi_parameter_selection); // to keep roller not updated
	}
}


void TaskOS::setStateChannel(uint8_t module_id, uint8_t channel)
{
	if(channel > 16) {
		return;
	}
	module_states[module_id].channel = channel;
	ui_p->setChannel(module_id, channel);
}


void TaskOS::setStateCC(uint8_t module_id, uint8_t cc)
{
	if(cc > 127) {
		return;
	}
	module_states[module_id].cc = cc;
	ui_p->setCC(module_id, cc);
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
			if(p_this->current_screen == SCREEN_ID_MAIN) {
				p_this->processEncoderEvent(&encoder_event);
			}
			else if(encoder_event.encoder_id == 0) {
				switch(p_this->current_screen) {
					case SCREEN_ID_MENU: {
						p_this->processMenuSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_MIDI_BANKS: {
						p_this->processMidiBankSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_UI_PRESETS: {
						p_this->processUiPresetSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_CONFIG_MIDI: {
						p_this->processMidiUnitSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_CONFIG_MIDI_UNIT: {
						p_this->processMidiParameterSelector(encoder_event.increase);
						break;
					}
					default:
						break;
				}
			}
		}
		
		if(xQueueReceive(p_this->button_event_queue, &button_ev, 0) == pdTRUE) {
			switch (button_ev.type) {
				case BUTTON_EVENT_SINGLE_PRESS: {
					if(p_this->current_screen == SCREEN_ID_MAIN) {
						if(button_ev.button_id < 16) {
							// Process single press button events, CC-100..CC-115, values 0/127
							uint8_t cc_number = 102 + button_ev.button_id;
							uint8_t cc_value = button_ev.state ? 127 : 0;
							task_midi.sendMidiCC(0, cc_number, cc_value);
						}
					} else if((button_ev.button_id == 0) && (button_ev.state == 1)) {
						p_this->processMenuButton();
					}
					break;
				}
				case BUTTON_EVENT_CENTRAL_QUAD_PRESS: {
					p_this->current_menu_selection = (menu_select_e)0;
					p_this->ui_p->lvgl_selectMenu(p_this->current_menu_selection);
					p_this->ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					p_this->current_screen = SCREEN_ID_MENU;
					break;
				}
				default:
					break;
			}
		}

		if(xQueueReceive(p_this->acm_event_queue, &acm_event, 0) == pdTRUE) {
			switch(acm_event.type) {
				case ACM_EVENT_TYPE_SET_NAME: {
					if(acm_event.id < 16) {
						memset(p_this->module_states[acm_event.id].name, 0, sizeof(p_this->module_states[acm_event.id].name));
						strncpy((char *)p_this->module_states[acm_event.id].name, (char *)acm_event.data, 16);
						p_this->ui_p->setName(acm_event.id, (const char *)p_this->module_states[acm_event.id].name);
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
							p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
							// Update UI
							p_this->ui_p->setValue(acm_event.id, module_state->current_value);
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_CHANNEL: {
					if(acm_event.id < 16) {
						uint8_t channel = acm_event.data[0];
						p_this->setStateChannel(acm_event.id, channel);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_CC: {
					if(acm_event.id < 16) {
						uint8_t cc = acm_event.data[0];
						p_this->setStateCC(acm_event.id, cc);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_RANGE: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if((acm_event.data[0] >= module_state->min_value) && (acm_event.data[0] <= 127)) {
							module_state->max_value = acm_event.data[0];
							p_this->ui_p->setRange(acm_event.id, module_state->max_value);
							// Adjust current value if needed
							if(module_state->current_value > module_state->max_value) {
								module_state->current_value = module_state->max_value;
								// Send MIDI CC event
								p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
								// Update UI
								p_this->ui_p->setValue(acm_event.id, module_state->current_value);
							}
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BG: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->ui_p->setColor(acm_event.id, COLOR_ELEMENT_BACKGROUND, color);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BORDER: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->ui_p->setColor(acm_event.id, COLOR_ELEMENT_BORDER, color);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_TEXT: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->ui_p->setColor(acm_event.id, COLOR_ELEMENT_TEXT, color);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BAR: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->ui_p->setColor(acm_event.id, COLOR_ELEMENT_BAR, color);
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
							p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->current_value);
							p_this->ui_p->setValue(acm_event.id, module_state->current_value);
						}
					}
					break;
				}
				case MIDI_SYS_SET_CHANNEL: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t channel = sysex_input_ev.buffer[2];
						p_this->setStateChannel(id, channel);
					}
					break;
				}
				case MIDI_SYS_SET_CC: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t cc = sysex_input_ev.buffer[2];
						p_this->setStateCC(id, cc);
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
