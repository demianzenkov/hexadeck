#include "task_os.h"
#include "bootloader.h"



TaskOS::TaskOS()
{
	ui_p = UI::getInstance();
	task_midi_p = TaskMIDI::getInstance();
	buttons_p = Buttons::getInstance();
	acm_p = ACM::getInstance();

	const module_state_t init_states[16] = {
		{0,  64,  0, 0,  0, 127, 1, "Bank", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{1,  64,  0, 1,  0, 127, 1, "Wheel", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{2,  64,  0, 2,  0, 127, 1, "Breath", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{3,  64,  0, 3,  0, 127, 1, "CC-3", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{4,  64,  0, 4,  0, 127, 1, "Foot", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{5,  64,  0, 5,  0, 127, 1, "Portamento", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{6,  64,  0, 6,  0, 127, 1, "Data Entry", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{7,  64,  0, 7,  0, 127, 1, "Volume", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{8,	 64,  0, 8,  0, 127, 1, "Balance", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{9,  64,  0, 9,  0, 127, 1, "CC-9", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{10, 64,  0, 10, 0, 127, 1,  "Pan", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{11, 64,  0, 11, 0, 127, 1,  "Expression", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{12, 64,  0, 12, 0, 127, 1,  "Effect-1", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{13, 64,  0, 13, 0, 127, 1,  "Effect-2", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{14, 64,  0, 14, 0, 127, 1,  "CC-14", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)},
		{15, 64,  0, 15, 0, 127, 1,  "CC-15", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(255, 255, 255)}
	};
	for (int i = 0; i < 16; i++) {
		memcpy(&module_states[i], &init_states[i], sizeof(module_state_t));
	}


	current_screen = SCREEN_ID_MAIN;
	current_menu_selection = MENU_SELECT_LOAD_PRESET;
	midi_parameter_channel_selector_active = false;
	midi_parameter_cc_selector_active = false;
}

TaskOS * TaskOS::getInstance()
{
	static TaskOS instance;
	return &instance;
}


void TaskOS::createTask()
{
	encoder_event_queue = xQueueCreate(64, sizeof(encoder_event_t));
	acm_event_queue = xQueueCreate(16, sizeof(acm_event_t));
	midi_input_event_queue = xQueueCreate(32, sizeof(midi_event_t));
	midi_sysex_input_event_queue = xQueueCreate(8, sizeof(midi_sysex_event_t));
	button_event_queue = xQueueCreate(8, sizeof(button_event_t));

	ui_p->createTask();
	task_midi_p->createTask();
	buttons_p->createTask();
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
			if(module_state->value + module_state->step <= module_state->max_value) {
				module_state->value += module_state->step;
			} else {
				module_state->value = module_state->max_value;
			}
		} else {
			if(module_state->value >= (module_state->min_value + module_state->step)) {
				module_state->value -= module_state->step;
			} else {
				module_state->value = module_state->min_value;
			}
		}
		task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->value);
		module_states[enc_id].value = module_state->value;

		// ui_p->refreshDisplayState(enc_id, module_state);
		ui_p->refreshDisplayValue(enc_id, module_state->value);
		// ui_p->setValue(enc_id, module_state->value);
	}
}


void TaskOS::processMenuButton()
{
	switch(current_screen) {
		case SCREEN_ID_MENU: {
			switch(current_menu_selection) {
				case MENU_SELECT_LOAD_PRESET: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_PRESETS);
					current_screen = SCREEN_ID_PRESETS;
					selected_action = ACTION_LOAD;
					break;
				}
				case MENU_SELECT_SAVE_PRESET: {
					ui_p->lvgl_loadScreen(0, SCREEN_ID_PRESETS);
					selected_action = ACTION_SAVE;
					current_screen = SCREEN_ID_PRESETS;
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
					current_screen = SCREEN_ID_MAIN;
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MAIN);
					ui_p->refreshDisplayState(0, &module_states[0]);
					break;
				}
				default:
					break;
			}
			break;
		}
		case SCREEN_ID_PRESETS: {
			if(preset_selection > 3) {
				ui_p->lvgl_selectPreset(0);
				preset_selection = 0;
				ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
				current_screen = SCREEN_ID_MENU;
			}
			else {
				if(selected_action == ACTION_LOAD) {
					if(nvs.loadModulePreset(preset_selection, module_states) != 0) {
						break;
					}
					// Refresh UI states
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MAIN);
					for (uint8_t i = 0; i < 16; i++) {
						ui_p->refreshDisplayState(i, &module_states[i]);
						vTaskDelay(50);
					}
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					current_screen = SCREEN_ID_MENU;
					vTaskDelay(50);
				} else if (selected_action == ACTION_SAVE) {
					// Save preset to NVS
					nvs.saveModulePreset(preset_selection, module_states);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					current_screen = SCREEN_ID_MENU;
				}
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
					// ui_p->setChannel(midi_unit_selection, module_states[midi_unit_selection].channel);
					module_states[midi_unit_selection].channel = module_states[midi_unit_selection].channel;
					ui_p->refreshDisplayState(midi_unit_selection, &module_states[midi_unit_selection]);
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
					// ui_p->setCC(midi_unit_selection, module_states[midi_unit_selection].cc);
					module_states[midi_unit_selection].cc = module_states[midi_unit_selection].cc;
					ui_p->refreshDisplayState(midi_unit_selection, &module_states[midi_unit_selection]);
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


void TaskOS::processPresetSelector(bool increase)
{
	if(increase) {
		if(preset_selection < 4) {
			preset_selection++;
		} else {
			preset_selection = 0;
		}
	} else {
		if(preset_selection > 0) {
			preset_selection--;
		} else {
			preset_selection = 4;
		}
	}
	ui_p->lvgl_selectPreset(preset_selection);
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


void TaskOS::setStateValue(uint8_t disp, uint8_t value)
{
	if (value > 127)
	{
		return;
	}
	module_states[disp].value = value;
}


void TaskOS::setStateChannel(uint8_t module_id, uint8_t channel)
{
	if(channel > 16) {
		return;
	}
	module_states[module_id].channel = channel;
	// ui_p->setChannel(module_id, channel);
}


void TaskOS::setStateCC(uint8_t module_id, uint8_t cc)
{
	if(cc > 127) {
		return;
	}
	module_states[module_id].cc = cc;
	// ui_p->setCC(module_id, cc);
}


void TaskOS::setStateRange(uint8_t id, uint8_t max_level)
{
	if(max_level > 127) {
		return;
	}
	module_states[id].max_value = max_level;
}


void TaskOS::setStateColor(uint8_t disp, color_element_e element, lv_color_t color)
{
	switch(element) {
		case COLOR_ELEMENT_BACKGROUND:
			module_states[disp].background_color = color;
			break;
		case COLOR_ELEMENT_BORDER:
			module_states[disp].border_color = color;
			break;
		case COLOR_ELEMENT_TEXT:
			module_states[disp].text_color = color;
			break;
		case COLOR_ELEMENT_BAR:
			module_states[disp].bar_color = color;
			break;
		default:
			return;
	}
}


void TaskOS::setStateName(uint8_t disp, const char *str)
{
	if (strlen(str) > MAX_NAME_LENGTH - 1) {
		return;
	}
	strncpy(module_states[disp].name, str, sizeof(module_states[disp].name));
}
	

void TaskOS::task(void const *arg)
{
	TaskOS *p_this = (TaskOS *)arg;

	encoder_event_t encoder_event;
	acm_event_t acm_event;
	midi_event_t midi_input_ev;
	midi_sysex_event_t sysex_input_ev;
	button_event_t button_ev;

	/* Load UI states from flash memory */
	if(p_this->nvs.loadModulePreset(0, p_this->module_states) != 0) {
		// Failed to load, use default states
		p_this->nvs.saveModulePreset(0, p_this->module_states);
		if(p_this->nvs.loadModulePreset(0, p_this->module_states) != 0) {
			while(1) {
				vTaskDelay(50);
			}
		}
	}

	/* Set UI for modules states */
	for(int i = 0; i < 16; i++) {
		p_this->ui_p->refreshDisplayState(i, &p_this->module_states[i]);
		vTaskDelay(30);
	}

	while (1)
	{
		if(xQueueReceive(p_this->encoder_event_queue, &encoder_event, 0) == pdTRUE) {
			// debouncer, if passed time less for channel less than 10 ms, ignore event
			uint32_t current_time = xTaskGetTickCount();
			if((current_time - p_this->last_encoder_event_time[encoder_event.encoder_id]) < 10) {
				continue;
			}
			p_this->last_encoder_event_time[encoder_event.encoder_id] = current_time;

			if(p_this->current_screen == SCREEN_ID_MAIN) {
				p_this->processEncoderEvent(&encoder_event);
			}
			else if(encoder_event.encoder_id == 0) {
				switch(p_this->current_screen) {
					case SCREEN_ID_MENU: {
						p_this->processMenuSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_PRESETS: {
						p_this->processPresetSelector(encoder_event.increase);
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
							p_this->task_midi_p->sendMidiCC(0, cc_number, cc_value);
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
						p_this->setStateName(acm_event.id, (const char *)acm_event.data);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_VALUE: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if(	(acm_event.data[0] >= module_state->min_value) && 
						(acm_event.data[0] <= module_state->max_value)) {
							// TODO: check if value correlates with step
							p_this->setStateValue(acm_event.id, acm_event.data[0]);
							p_this->ui_p->refreshDisplayState(acm_event.id, module_state);
							p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->value);
						}
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_CHANNEL: {
					if(acm_event.id < 16) {
						uint8_t channel = acm_event.data[0];
						p_this->setStateChannel(acm_event.id, channel);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_CC: {
					if(acm_event.id < 16) {
						uint8_t cc = acm_event.data[0];
						p_this->setStateCC(acm_event.id, cc);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_RANGE: {
					if(acm_event.id < 16) {
						module_state_t * module_state = &p_this->module_states[acm_event.id];
						if((acm_event.data[0] >= module_state->min_value) && (acm_event.data[0] <= 127)) {
							p_this->setStateRange(acm_event.id, acm_event.data[0]);
							// Adjust current value if needed
							if(module_state->value > module_state->max_value) {
								module_state->value = module_state->max_value;
								p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->value);
								p_this->setStateValue(acm_event.id, module_state->value);
							}
						}
						p_this->ui_p->refreshDisplayState(acm_event.id, module_state);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BG: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->setStateColor(acm_event.id, COLOR_ELEMENT_BACKGROUND, color);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BORDER: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->setStateColor(acm_event.id, COLOR_ELEMENT_BORDER, color);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_TEXT: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->setStateColor(acm_event.id, COLOR_ELEMENT_TEXT, color);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
					}
					break;
				}
				case ACM_EVENT_TYPE_SET_COLOR_BAR: {
					if(acm_event.id < 16) {
						lv_color_t color = lv_color_make(acm_event.data[0], acm_event.data[1], acm_event.data[2]);
						p_this->setStateColor(acm_event.id, COLOR_ELEMENT_BAR, color);
						p_this->ui_p->refreshDisplayState(acm_event.id, &p_this->module_states[acm_event.id]);
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
						// memset(p_this->module_states[id].name, 0, sizeof(p_this->module_states[id].name));
						p_this->setStateName(id, (const char *)&sysex_input_ev.buffer[2]);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
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
							p_this->setStateValue(acm_event.id, value);
							p_this->ui_p->refreshDisplayState(id, module_state);
							p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->value);
						}
					}
					break;
				}
				case MIDI_SYS_SET_CHANNEL: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t channel = sysex_input_ev.buffer[2];
						p_this->setStateChannel(id, channel);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_CC: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t cc = sysex_input_ev.buffer[2];
						p_this->setStateCC(id, cc);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_RANGE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t max_level = sysex_input_ev.buffer[2];
						if((max_level >= module_state->min_value) && (max_level <= 127)) {
							p_this->setStateRange(id, max_level);
							// Adjust current value if needed
							if(module_state->value > module_state->max_value) {
								p_this->setStateValue(id, module_state->max_value);
								p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->value);
							}
							p_this->ui_p->refreshDisplayState(id, module_state);
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
						p_this->setStateColor(id, COLOR_ELEMENT_BACKGROUND, color);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
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
						p_this->setStateColor(id, COLOR_ELEMENT_BORDER, color);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
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
						p_this->setStateColor(id, COLOR_ELEMENT_TEXT, color);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
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
						p_this->setStateColor(id, COLOR_ELEMENT_BAR, color);
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
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
