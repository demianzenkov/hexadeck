#include "task_os.h"
#include "bootloader.h"



TaskOS::TaskOS()
{
	ui_p = UI::getInstance();
	task_midi_p = TaskMIDI::getInstance();
	buttons_p = Buttons::getInstance();
	acm_p = ACM::getInstance();

	const module_state_t init_states[16] = {
		{0,  64,  0, 0,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 16, 0, 127, "Bank", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{1,  64,  0, 1,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 17, 0, 127, "Wheel", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{2,  64,  0, 2,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 18, 0, 127, "Breath", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{3,  64,  0, 3,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 19, 0, 127, "CC-3", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{4,  64,  0, 4,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 20, 0, 127, "Foot", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{5,  64,  0, 5,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 21, 0, 127, "Portamento", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{6,  64,  0, 6,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 22, 0, 127, "Data Entry", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{7,  64,  0, 7,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 23, 0, 127, "Volume", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{8,	 64,  0, 8,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 24, 0, 127, "Balance", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{9,  64,  0, 9,  0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 25, 0, 127, "CC-9", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{10, 64,  0, 10, 0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 26, 0, 127,  "Pan", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{11, 64,  0, 11, 0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 27, 0, 127,  "Expression", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{12, 64,  0, 12, 0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 28, 0, 127,  "Effect-1", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{13, 64,  0, 13, 0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 29, 0, 127,  "Effect-2", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{14, 64,  0, 14, 0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 31, 0, 127,  "CC-14", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)},
		{15, 64,  0, 15, 0, 127, 1, 1, BUTTON_MIDI_ENABLED, BUTTON_ONCLICK_STEP, 0, 10, 0, 32, 0, 127,  "CC-15", lv_color_make(0x1e, 0x1e, 0x1e), lv_color_make(0, 0xff, 0x88), lv_color_make(255, 255, 255), lv_color_make(0, 0xff, 0x88)}
	};
	for (int i = 0; i < 16; i++) {
		memcpy(&module_states[i], &init_states[i], sizeof(module_state_t));
	}


	current_screen = SCREEN_ID_MAIN;
	current_menu_selection = MENU_SELECT_PRESETS;
	preset_menu_selection = PRESET_MENU_PRESET;
	preset_index = 0;
	preset_edit_active = false;
	knob_setup_selection = KNOB_SETUP_SELECT_KNOB;
	knob_selection = 0;
	knob_setup_edit_active = false;
	button_setup_selection = BUTTON_SETUP_SELECT_BUTTON;
	button_selection = 0;
	button_setup_edit_active = false;
	settings_menu_selection = SETTINGS_MENU_SCREEN;
	settings_screen_edit_active = false;
	settings_screen_index = 0xFF;
	for(int i = 0; i < 16; i++) {
		module_states[i].simple_screen_enabled = 1;
		ui_p->setSimpleMode(i, true);
	}
	current_screen = getMainScreenForDisplay(0);
}

ScreensEnum TaskOS::getMainScreenForDisplay(uint8_t display_id) const
{
	if(display_id > 15) {
		return SCREEN_ID_MAIN;
	}
	return module_states[display_id].simple_screen_enabled ? SCREEN_ID_MAIN_SIMPLE : SCREEN_ID_MAIN;
}

bool TaskOS::areAllSimpleScreensEnabled() const
{
	for(int i = 0; i < 16; i++) {
		if(!module_states[i].simple_screen_enabled) {
			return false;
		}
	}
	return true;
}

void TaskOS::applySimpleScreenMode(uint8_t display_id, bool enabled)
{
	if(display_id > 15) {
		return;
	}
	module_states[display_id].simple_screen_enabled = enabled ? 1 : 0;
	ui_p->setSimpleMode(display_id, enabled);
	if(display_id == 0) {
		if(current_screen == SCREEN_ID_SCREEN_SETUP) {
			return;
		}
		if(current_screen == SCREEN_ID_MAIN || current_screen == SCREEN_ID_MAIN_SIMPLE) {
			current_screen = getMainScreenForDisplay(0);
			ui_p->lvgl_loadScreen(0, current_screen);
		}
		ui_p->refreshDisplayState(0, &module_states[0]);
		return;
	}

	ui_p->lvgl_loadScreen(display_id, getMainScreenForDisplay(display_id));
	ui_p->refreshDisplayState(display_id, &module_states[display_id]);
}

void TaskOS::refreshScreenSetupUi()
{
	bool current_simple = (settings_screen_index == 0xFF)
		? areAllSimpleScreensEnabled()
		: (module_states[settings_screen_index].simple_screen_enabled != 0);
	ui_p->lvgl_loadSettingsOptions(settings_screen_index, current_simple);
	ui_p->lvgl_selectSettings(settings_menu_selection);
	ui_p->lvgl_activateSettingsSelector(settings_screen_edit_active);
}

TaskOS * TaskOS::getInstance()
{
	static TaskOS instance;
	return &instance;
}

void TaskOS::captureKnobSetupSnapshot()
{
	if(knob_selection > 15) {
		knob_setup_snapshot_valid = false;
		return;
	}
	memcpy(&knob_setup_snapshot, &module_states[knob_selection], sizeof(module_state_t));
	knob_setup_snapshot_id = knob_selection;
	knob_setup_snapshot_valid = true;
}

bool TaskOS::knobSetupParamsChanged() const
{
	if(!knob_setup_snapshot_valid || knob_setup_snapshot_id > 15) {
		return false;
	}
	const module_state_t *state = &module_states[knob_setup_snapshot_id];
	return (state->channel != knob_setup_snapshot.channel) ||
		(state->cc != knob_setup_snapshot.cc) ||
		(state->min_value != knob_setup_snapshot.min_value) ||
		(state->max_value != knob_setup_snapshot.max_value) ||
		(state->step != knob_setup_snapshot.step);
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
		ui_p->refreshDisplayValue(enc_id, module_state->value, module_state->max_value);
	}
}

void TaskOS::processMenuButton()
{
	switch(current_screen) {
		case SCREEN_ID_MENU: {
			switch(current_menu_selection) {
				case MENU_SELECT_PRESETS: {
					preset_edit_active = false;
					ui_p->lvgl_activatePresetSelector(false);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_PRESETS);
					ui_p->lvgl_loadPresetOptions(preset_index);
					ui_p->lvgl_selectPreset(preset_menu_selection);
					current_screen = SCREEN_ID_PRESETS;
					break;
				}
				case MENU_SELECT_KNOB_SETUP: {
					knob_setup_edit_active = false;
					ui_p->lvgl_activateKnobSetupSelector(false);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_KNOB_SETUP);
					ui_p->lvgl_loadKnobSetupParameters(knob_selection, &module_states[knob_selection]);
					ui_p->lvgl_selectKnobSetup(knob_setup_selection);
					current_screen = SCREEN_ID_KNOB_SETUP;
					break;
				}
				case MENU_SELECT_BUTTON_SETUP: {
					button_setup_edit_active = false;
					ui_p->lvgl_activateButtonSetupSelector(false);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_BUTTON_SETUP);
					ui_p->lvgl_loadButtonSetupParameters(button_selection, &module_states[button_selection]);
					ui_p->lvgl_selectButtonSetup(button_setup_selection);
					current_screen = SCREEN_ID_BUTTON_SETUP;
					break;
				}
				case MENU_SELECT_SCREEN_SETUP: {
					settings_screen_edit_active = false;
					ui_p->lvgl_loadScreen(0, SCREEN_ID_SCREEN_SETUP);
					refreshScreenSetupUi();
					current_screen = SCREEN_ID_SCREEN_SETUP;
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
					current_screen = getMainScreenForDisplay(0);
					ui_p->lvgl_loadScreen(0, current_screen);
					ui_p->refreshDisplayState(0, &module_states[0]);
					break;
				}
				default:
					break;
			}
			break;
		}
		case SCREEN_ID_PRESETS: {
			switch(preset_menu_selection) {
				case PRESET_MENU_PRESET:
					preset_edit_active = !preset_edit_active;
					ui_p->lvgl_activatePresetSelector(preset_edit_active);
					break;
				case PRESET_MENU_LOAD:
					if(nvs.loadModulePreset(preset_index, module_states) != 0) {
						break;
					}
					preset_edit_active = false;
					ui_p->lvgl_activatePresetSelector(false);
					ui_p->lvgl_loadScreen(0, getMainScreenForDisplay(0));
					for (uint8_t i = 0; i < 16; i++) {
						ui_p->refreshDisplayState(i, &module_states[i]);
						vTaskDelay(50);
					}
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					current_screen = SCREEN_ID_MENU;
					vTaskDelay(50);
					break;
				case PRESET_MENU_SAVE:
					nvs.saveModulePreset(preset_index, module_states);
					preset_edit_active = false;
					ui_p->lvgl_activatePresetSelector(false);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					current_screen = SCREEN_ID_MENU;
					break;
				case PRESET_MENU_RETURN:
					preset_edit_active = false;
					ui_p->lvgl_activatePresetSelector(false);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					current_screen = SCREEN_ID_MENU;
					break;
				default:
					break;
			}
			break;
		}
		case SCREEN_ID_KNOB_SETUP: {
			if(knob_setup_selection == KNOB_SETUP_RETURN) {
				knob_setup_edit_active = false;
				ui_p->lvgl_activateKnobSetupSelector(false);
				ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
				current_screen = SCREEN_ID_MENU;
				break;
			}
			knob_setup_edit_active = !knob_setup_edit_active;
			if(knob_setup_edit_active) {
				captureKnobSetupSnapshot();
				ui_p->lvgl_activateKnobSetupSelector(true);
				break;
			}
			ui_p->lvgl_activateKnobSetupSelector(false);
			if(knobSetupParamsChanged() && knob_setup_snapshot_id != 0) {
				ui_p->lvgl_loadScreen(knob_setup_snapshot_id, getMainScreenForDisplay(knob_setup_snapshot_id));
				ui_p->refreshDisplayState(knob_setup_snapshot_id, &module_states[knob_setup_snapshot_id]);
				vTaskDelay(30);
			}
			ui_p->lvgl_loadScreen(0, SCREEN_ID_KNOB_SETUP);
			ui_p->lvgl_loadKnobSetupParameters(knob_selection, &module_states[knob_selection]);
			ui_p->lvgl_selectKnobSetup(knob_setup_selection);
			break;
		}
		case SCREEN_ID_BUTTON_SETUP: {
			if(button_setup_selection == BUTTON_SETUP_RETURN) {
				button_setup_edit_active = false;
				ui_p->lvgl_activateButtonSetupSelector(false);
				ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
				current_screen = SCREEN_ID_MENU;
				break;
			}
			button_setup_edit_active = !button_setup_edit_active;
			ui_p->lvgl_activateButtonSetupSelector(button_setup_edit_active);
			break;
		}
		case SCREEN_ID_SCREEN_SETUP: {
			switch(settings_menu_selection) {
				case SETTINGS_MENU_SCREEN:
					settings_screen_edit_active = !settings_screen_edit_active;
					refreshScreenSetupUi();
					break;
				case SETTINGS_MENU_SIMPLE_SCREEN:
					settings_screen_edit_active = false;
					if(settings_screen_index == 0xFF) {
						bool enable_all = !areAllSimpleScreensEnabled();
						for(uint8_t i = 0; i < 16; i++) {
							applySimpleScreenMode(i, enable_all);
							vTaskDelay(40);
						}
						ui_p->lvgl_loadScreen(0, SCREEN_ID_SCREEN_SETUP);
						current_screen = SCREEN_ID_SCREEN_SETUP;
					} else if(settings_screen_index < 16) {
						applySimpleScreenMode(settings_screen_index, !module_states[settings_screen_index].simple_screen_enabled);
						vTaskDelay(30);
					}
					refreshScreenSetupUi();
					break;
				case SETTINGS_MENU_RETURN:
					settings_screen_edit_active = false;
					ui_p->lvgl_activateSettingsSelector(false);
					ui_p->lvgl_loadScreen(0, SCREEN_ID_MENU);
					current_screen = SCREEN_ID_MENU;
					break;
				default:
					break;
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
	if(preset_edit_active) {
		if(increase) {
			if(preset_index < 3) {
				preset_index++;
			} else {
				preset_index = 0;
			}
		} else {
			if(preset_index > 0) {
				preset_index--;
			} else {
				preset_index = 3;
			}
		}
		ui_p->lvgl_loadPresetOptions(preset_index);
		ui_p->lvgl_selectPreset(preset_menu_selection);
		return;
	}

	if(increase) {
		if(preset_menu_selection < PRESET_MENU_COUNT - 1) {
			preset_menu_selection = (preset_menu_select_e)((int)preset_menu_selection + 1);
		} else {
			preset_menu_selection = (preset_menu_select_e)0;
		}
	} else {
		if(preset_menu_selection > 0) {
			preset_menu_selection = (preset_menu_select_e)((int)preset_menu_selection - 1);
		} else {
			preset_menu_selection = (preset_menu_select_e)((int)PRESET_MENU_COUNT - 1);
		}
	}
	ui_p->lvgl_selectPreset(preset_menu_selection);
}


void TaskOS::processKnobSetupSelector(bool increase)
{
	if(knob_setup_edit_active) {
		switch(knob_setup_selection) {
			case KNOB_SETUP_SELECT_KNOB: {
				if(increase) {
					if(knob_selection < 15) {
						knob_selection++;
					} else {
						knob_selection = 0;
					}
				} else {
					if(knob_selection > 0) {
						knob_selection--;
					} else {
						knob_selection = 15;
					}
				}
				captureKnobSetupSnapshot();
				break;
			}
			case KNOB_SETUP_CHANNEL: {
				if(increase) {
					if(module_states[knob_selection].channel < 15) {
						module_states[knob_selection].channel++;
					} else {
						module_states[knob_selection].channel = 0;
					}
				} else {
					if(module_states[knob_selection].channel > 0) {
						module_states[knob_selection].channel--;
					} else {
						module_states[knob_selection].channel = 15;
					}
				}
				break;
			}
			case KNOB_SETUP_CC: {
				if(increase) {
					if(module_states[knob_selection].cc < 127) {
						module_states[knob_selection].cc++;
					} else {
						module_states[knob_selection].cc = 0;
					}
				} else {
					if(module_states[knob_selection].cc > 0) {
						module_states[knob_selection].cc--;
					} else {
						module_states[knob_selection].cc = 127;
					}
				}
				break;
			}
			case KNOB_SETUP_MIN_RANGE: {
				if(increase) {
					if(module_states[knob_selection].min_value < module_states[knob_selection].max_value) {
						module_states[knob_selection].min_value++;
					}
				} else {
					if(module_states[knob_selection].min_value > 0) {
						module_states[knob_selection].min_value--;
					}
				}
				if(module_states[knob_selection].value < module_states[knob_selection].min_value) {
					module_states[knob_selection].value = module_states[knob_selection].min_value;
				}
				break;
			}
			case KNOB_SETUP_MAX_RANGE: {
				if(increase) {
					if(module_states[knob_selection].max_value < 127) {
						module_states[knob_selection].max_value++;
					}
				} else {
					if(module_states[knob_selection].max_value > module_states[knob_selection].min_value) {
						module_states[knob_selection].max_value--;
					}
				}
				if(module_states[knob_selection].value > module_states[knob_selection].max_value) {
					module_states[knob_selection].value = module_states[knob_selection].max_value;
				}
				break;
			}
			case KNOB_SETUP_STEP: {
				if(increase) {
					if(module_states[knob_selection].step < 127) {
						module_states[knob_selection].step++;
					}
				} else {
					if(module_states[knob_selection].step > 1) {
						module_states[knob_selection].step--;
					}
				}
				break;
			}
			case KNOB_SETUP_RETURN:
			default:
				break;
		}
		ui_p->lvgl_loadKnobSetupParameters(knob_selection, &module_states[knob_selection]);
		ui_p->lvgl_selectKnobSetup(knob_setup_selection);
		return;
	}

	if(increase) {
		if(knob_setup_selection < KNOB_SETUP_COUNT - 1) {
			knob_setup_selection = (knob_setup_select_e)((int)knob_setup_selection + 1);
		} else {
			knob_setup_selection = (knob_setup_select_e)0;
		}
	} else {
		if(knob_setup_selection > 0) {
			knob_setup_selection = (knob_setup_select_e)((int)knob_setup_selection - 1);
		} else {
			knob_setup_selection = (knob_setup_select_e)((int)KNOB_SETUP_COUNT - 1);
		}
	}
	ui_p->lvgl_selectKnobSetup(knob_setup_selection);
}


void TaskOS::processButtonSetupSelector(bool increase)
{
	if(button_setup_edit_active) {
		switch(button_setup_selection) {
			case BUTTON_SETUP_SELECT_BUTTON: {
				if(increase) {
					if(button_selection < 15) {
						button_selection++;
					} else {
						button_selection = 0;
					}
				} else {
					if(button_selection > 0) {
						button_selection--;
					} else {
						button_selection = 15;
					}
				}
				ui_p->lvgl_loadButtonSetupParameters(button_selection, &module_states[button_selection]);
				ui_p->lvgl_selectButtonSetup(button_setup_selection);
				break;
			}
			case BUTTON_SETUP_MIDI:
				module_states[button_selection].button_midi_enabled =
					(module_states[button_selection].button_midi_enabled == BUTTON_MIDI_ENABLED)
					? BUTTON_MIDI_DISABLED
					: BUTTON_MIDI_ENABLED;
				break;
			case BUTTON_SETUP_CHANNEL:
				if(increase) {
					if(module_states[button_selection].button_midi_channel < 15) {
						module_states[button_selection].button_midi_channel++;
					} else {
						module_states[button_selection].button_midi_channel = 0;
					}
				} else {
					if(module_states[button_selection].button_midi_channel > 0) {
						module_states[button_selection].button_midi_channel--;
					} else {
						module_states[button_selection].button_midi_channel = 15;
					}
				}
				break;
			case BUTTON_SETUP_CC:
				if(increase) {
					if(module_states[button_selection].button_midi_cc < 127) {
						module_states[button_selection].button_midi_cc++;
					} else {
						module_states[button_selection].button_midi_cc = 0;
					}
				} else {
					if(module_states[button_selection].button_midi_cc > 0) {
						module_states[button_selection].button_midi_cc--;
					} else {
						module_states[button_selection].button_midi_cc = 127;
					}
				}
				break;
			case BUTTON_SETUP_DEFAULT_VALUE:
				if(increase) {
					if(module_states[button_selection].button_midi_released_value < 127) {
						module_states[button_selection].button_midi_released_value++;
					}
				} else {
					if(module_states[button_selection].button_midi_released_value > 0) {
						module_states[button_selection].button_midi_released_value--;
					}
				}
				break;
			case BUTTON_SETUP_PRESSED_VALUE:
				if(increase) {
					if(module_states[button_selection].button_midi_pressed_value < 127) {
						module_states[button_selection].button_midi_pressed_value++;
					}
				} else {
					if(module_states[button_selection].button_midi_pressed_value > 0) {
						module_states[button_selection].button_midi_pressed_value--;
					}
				}
				break;
			case BUTTON_SETUP_ONCLICK:
				module_states[button_selection].button_onclick_mode =
					(module_states[button_selection].button_onclick_mode == BUTTON_ONCLICK_STEP)
					? BUTTON_ONCLICK_DISABLED
					: BUTTON_ONCLICK_STEP;
					if(module_states[button_selection].button_onclick_mode == BUTTON_ONCLICK_DISABLED &&
						module_states[button_selection].button_onclick_active) {
						uint8_t temp_step = module_states[button_selection].step;
						module_states[button_selection].step = module_states[button_selection].button_onclick_step;
						module_states[button_selection].button_onclick_step = temp_step;
						module_states[button_selection].button_onclick_active = 0;
					}
				break;
			case BUTTON_SETUP_ONCLICK_STEP:
				if(increase) {
					if(module_states[button_selection].button_onclick_step < 127) {
						module_states[button_selection].button_onclick_step++;
					}
				} else {
					if(module_states[button_selection].button_onclick_step > 1) {
						module_states[button_selection].button_onclick_step--;
					}
				}
				break;
			case BUTTON_SETUP_RETURN:
			default:
				break;
		}
		ui_p->lvgl_loadButtonSetupParameters(button_selection, &module_states[button_selection]);
		ui_p->lvgl_selectButtonSetup(button_setup_selection);
		return;
	}

	if(increase) {
		if(button_setup_selection < BUTTON_SETUP_COUNT - 1) {
			button_setup_selection = (button_setup_select_e)((int)button_setup_selection + 1);
		} else {
			button_setup_selection = (button_setup_select_e)0;
		}
	} else {
		if(button_setup_selection > 0) {
			button_setup_selection = (button_setup_select_e)((int)button_setup_selection - 1);
		} else {
			button_setup_selection = (button_setup_select_e)((int)BUTTON_SETUP_COUNT - 1);
		}
	}
	ui_p->lvgl_selectButtonSetup(button_setup_selection);
}


void TaskOS::processSettingsSelector(bool increase)
{
	if(settings_screen_edit_active && settings_menu_selection == SETTINGS_MENU_SCREEN) {
		if(increase) {
			if(settings_screen_index == 0xFF) {
				settings_screen_index = 0;
			} else if(settings_screen_index < 15) {
				settings_screen_index++;
			} else {
				settings_screen_index = 0xFF;
			}
		} else {
			if(settings_screen_index == 0xFF) {
				settings_screen_index = 15;
			} else if(settings_screen_index > 0) {
				settings_screen_index--;
			} else {
				settings_screen_index = 0xFF;
			}
		}
		refreshScreenSetupUi();
		return;
	}

	if(increase) {
		if(settings_menu_selection < SETTINGS_MENU_COUNT - 1) {
			settings_menu_selection = (settings_menu_select_e)((int)settings_menu_selection + 1);
		} else {
			settings_menu_selection = (settings_menu_select_e)0;
		}
	} else {
		if(settings_menu_selection > 0) {
			settings_menu_selection = (settings_menu_select_e)((int)settings_menu_selection - 1);
		} else {
			settings_menu_selection = (settings_menu_select_e)((int)SETTINGS_MENU_COUNT - 1);
		}
	}
	ui_p->lvgl_selectSettings(settings_menu_selection);
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
}


void TaskOS::setStateCC(uint8_t module_id, uint8_t cc)
{
	if(cc > 127) {
		return;
	}
	module_states[module_id].cc = cc;
}


void TaskOS::setStateRangeMin(uint8_t id, uint8_t min_level)
{
	if(min_level > 127) {
		return;
	}
	module_states[id].min_value = min_level;
	if(module_states[id].min_value > module_states[id].max_value) {
		module_states[id].max_value = module_states[id].min_value;
	}
	if(module_states[id].value < module_states[id].min_value) {
		module_states[id].value = module_states[id].min_value;
	}
}

void TaskOS::setStateRangeMax(uint8_t id, uint8_t max_level)
{
	if(max_level > 127) {
		return;
	}
	module_states[id].max_value = max_level;
	if(module_states[id].max_value < module_states[id].min_value) {
		module_states[id].min_value = module_states[id].max_value;
	}
	if(module_states[id].value > module_states[id].max_value) {
		module_states[id].value = module_states[id].max_value;
	}
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
	// if(p_this->nvs.loadModulePreset(0, p_this->module_states) != 0) {
	// 	p_this->nvs.saveModulePreset(0, p_this->module_states);
	// 	if(p_this->nvs.loadModulePreset(0, p_this->module_states) != 0) {
	// 		while(1) {
	// 			vTaskDelay(50);
	// 		}
	// 	}
	// }

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
			if((current_time - p_this->last_encoder_event_time[encoder_event.encoder_id]) < 20) {
				continue;
			}
			p_this->last_encoder_event_time[encoder_event.encoder_id] = current_time;

			if(p_this->current_screen == SCREEN_ID_MAIN || p_this->current_screen == SCREEN_ID_MAIN_SIMPLE) {
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
					case SCREEN_ID_KNOB_SETUP: {
						p_this->processKnobSetupSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_BUTTON_SETUP: {
						p_this->processButtonSetupSelector(encoder_event.increase);
						break;
					}
					case SCREEN_ID_SCREEN_SETUP: {
						p_this->processSettingsSelector(encoder_event.increase);
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
					if(p_this->current_screen == SCREEN_ID_MAIN || p_this->current_screen == SCREEN_ID_MAIN_SIMPLE) {
						if(button_ev.button_id < 16) {
							module_state_t *button_state = &p_this->module_states[button_ev.button_id];
							if(button_state->button_midi_enabled == BUTTON_MIDI_ENABLED) {
								uint8_t cc_value = button_ev.state
									? button_state->button_midi_pressed_value
									: button_state->button_midi_released_value;
								p_this->task_midi_p->sendMidiCC(
									button_state->button_midi_channel,
									button_state->button_midi_cc,
									cc_value
								);
							}
							if(button_state->button_onclick_mode == BUTTON_ONCLICK_STEP && !button_ev.state) {
								uint8_t temp_step = button_state->step;
								button_state->step = button_state->button_onclick_step;
								button_state->button_onclick_step = temp_step;
								button_state->button_onclick_active = button_state->button_onclick_active ? 0 : 1;
								p_this->ui_p->refreshDisplayState(button_ev.button_id, button_state);
							}
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
							p_this->ui_p->refreshDisplayValue(acm_event.id, module_state->value, module_state->max_value);
							// p_this->ui_p->refreshDisplayState(acm_event.id, module_state);
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
							p_this->setStateRangeMax(acm_event.id, acm_event.data[0]);
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
			// TODO: automapping
			
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
			midi_sys_event_type_e event_type = (midi_sys_event_type_e)sysex_input_ev.buffer[0];
			switch(event_type) {
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
				case MIDI_SYS_SET_RANGE_MIN: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t min_level = sysex_input_ev.buffer[2];
						if(min_level > module_state->max_value) {
							min_level = module_state->max_value;
						}
						p_this->setStateRangeMin(id, min_level);
						if(module_state->value < module_state->min_value) {
							p_this->setStateValue(id, module_state->min_value);
							p_this->task_midi_p->sendMidiCC(module_state->channel, module_state->cc, module_state->value);
						}
						p_this->ui_p->refreshDisplayState(id, module_state);
					}
					break;
				}
				case MIDI_SYS_SET_RANGE_MAX: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						module_state_t * module_state = &p_this->module_states[id];
						uint8_t max_level = sysex_input_ev.buffer[2];
						if(max_level < module_state->min_value) {
							max_level = module_state->min_value;
						}
						if(max_level <= 127) {
							p_this->setStateRangeMax(id, max_level);
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
				case MIDI_SYS_SET_STEP: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t step = sysex_input_ev.buffer[2];
						if(step < 1) {
							step = 1;
						}
						p_this->module_states[id].step = step;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_ONCLICK_MODE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t mode = sysex_input_ev.buffer[2];
						p_this->module_states[id].button_onclick_mode = (mode == BUTTON_ONCLICK_STEP)
							? BUTTON_ONCLICK_STEP
							: BUTTON_ONCLICK_DISABLED;
						if(p_this->module_states[id].button_onclick_mode == BUTTON_ONCLICK_DISABLED &&
							p_this->module_states[id].button_onclick_active) {
							uint8_t temp_step = p_this->module_states[id].step;
							p_this->module_states[id].step = p_this->module_states[id].button_onclick_step;
							p_this->module_states[id].button_onclick_step = temp_step;
							p_this->module_states[id].button_onclick_active = 0;
						}
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_ONCLICK_STEP: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t step = sysex_input_ev.buffer[2];
						if(step < 1) {
							step = 1;
						}
						p_this->module_states[id].button_onclick_step = step;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_ONCLICK_ACTIVE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t active = sysex_input_ev.buffer[2];
						p_this->module_states[id].button_onclick_active = active ? 1 : 0;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_MIDI_CHANNEL: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t channel = sysex_input_ev.buffer[2];
						p_this->module_states[id].button_midi_channel = channel & 0x0F;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_MIDI_CC: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t cc = sysex_input_ev.buffer[2];
						p_this->module_states[id].button_midi_cc = cc;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_MIDI_RELEASED_VALUE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t value = sysex_input_ev.buffer[2];
						p_this->module_states[id].button_midi_released_value = value;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
					}
					break;
				}
				case MIDI_SYS_SET_BUTTON_MIDI_PRESSED_VALUE: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t value = sysex_input_ev.buffer[2];
						p_this->module_states[id].button_midi_pressed_value = value;
						p_this->ui_p->refreshDisplayState(id, &p_this->module_states[id]);
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
				case MIDI_SYS_SET_SIMPLE_SCREEN: {
					uint8_t id = sysex_input_ev.buffer[1];
					if(id < 16) {
						uint8_t enabled = sysex_input_ev.buffer[2];
						p_this->applySimpleScreenMode(id, enabled != 0);
					}
					break;
				}
				case MIDI_SYS_FIRMWARE_UPDATE: {
					JumpToBootloader();
					break;
				}
				case MIDI_SYS_GET_NAME:
				case MIDI_SYS_GET_VALUE:
				case MIDI_SYS_GET_CHANNEL:
				case MIDI_SYS_GET_CC:
				case MIDI_SYS_GET_RANGE_MIN:
				case MIDI_SYS_GET_RANGE_MAX:
				case MIDI_SYS_GET_STEP:
				case MIDI_SYS_GET_BUTTON_ONCLICK_MODE:
				case MIDI_SYS_GET_BUTTON_ONCLICK_STEP:
				case MIDI_SYS_GET_BUTTON_MIDI_CHANNEL:
				case MIDI_SYS_GET_BUTTON_MIDI_CC:
				case MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE:
				case MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE:
				case MIDI_SYS_GET_COLOR_BG:
				case MIDI_SYS_GET_COLOR_BORDER:
				case MIDI_SYS_GET_COLOR_TEXT:
				case MIDI_SYS_GET_COLOR_BAR:
				case MIDI_SYS_GET_SIMPLE_SCREEN:
				case MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE: {
					uint8_t id = sysex_input_ev.buffer[1];
					auto send_reply = [&](uint8_t module_id) {
						module_state_t * module_state = &p_this->module_states[module_id];
						switch(event_type) {
							case MIDI_SYS_GET_NAME: {
								uint8_t payload[2 + MAX_NAME_LENGTH] = {};
								payload[0] = MIDI_SYS_GET_NAME;
								payload[1] = module_id;
								strncpy((char *)&payload[2], module_state->name, MAX_NAME_LENGTH);
								p_this->task_midi_p->sendMidiSysex(payload, 2 + strnlen(module_state->name, MAX_NAME_LENGTH));
								break;
							}
							case MIDI_SYS_GET_VALUE: {
								uint8_t payload[3] = {MIDI_SYS_GET_VALUE, module_id, module_state->value};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_CHANNEL: {
								uint8_t payload[3] = {MIDI_SYS_GET_CHANNEL, module_id, module_state->channel};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_CC: {
								uint8_t payload[3] = {MIDI_SYS_GET_CC, module_id, module_state->cc};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_RANGE_MIN: {
								uint8_t payload[3] = {MIDI_SYS_GET_RANGE_MIN, module_id, module_state->min_value};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_RANGE_MAX: {
								uint8_t payload[3] = {MIDI_SYS_GET_RANGE_MAX, module_id, module_state->max_value};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_STEP: {
								uint8_t payload[3] = {MIDI_SYS_GET_STEP, module_id, module_state->step};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_ONCLICK_MODE: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_ONCLICK_MODE, module_id, module_state->button_onclick_mode};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_ONCLICK_STEP: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_ONCLICK_STEP, module_id, module_state->button_onclick_step};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_MIDI_CHANNEL: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_MIDI_CHANNEL, module_id, module_state->button_midi_channel};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_MIDI_CC: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_MIDI_CC, module_id, module_state->button_midi_cc};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_MIDI_RELEASED_VALUE, module_id, module_state->button_midi_released_value};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_MIDI_PRESSED_VALUE, module_id, module_state->button_midi_pressed_value};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_COLOR_BG:
							case MIDI_SYS_GET_COLOR_BORDER:
							case MIDI_SYS_GET_COLOR_TEXT:
							case MIDI_SYS_GET_COLOR_BAR: {
								lv_color_t color = module_state->background_color;
								if(event_type == MIDI_SYS_GET_COLOR_BORDER) {
									color = module_state->border_color;
								} else if(event_type == MIDI_SYS_GET_COLOR_TEXT) {
									color = module_state->text_color;
								} else if(event_type == MIDI_SYS_GET_COLOR_BAR) {
									color = module_state->bar_color;
								}
								lv_color32_t rgb = lv_color_to_32(color, LV_OPA_COVER);
								uint8_t r = rgb.red;
								uint8_t g = rgb.green;
								uint8_t b = rgb.blue;
								uint8_t payload[8] = {
									(uint8_t)event_type,
									module_id,
									(uint8_t)(r & 0x7F), (uint8_t)((r >> 7) & 0x7F),
									(uint8_t)(g & 0x7F), (uint8_t)((g >> 7) & 0x7F),
									(uint8_t)(b & 0x7F), (uint8_t)((b >> 7) & 0x7F)
								};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_SIMPLE_SCREEN: {
								uint8_t payload[3] = {MIDI_SYS_GET_SIMPLE_SCREEN, module_id, module_state->simple_screen_enabled};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							case MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE: {
								uint8_t payload[3] = {MIDI_SYS_GET_BUTTON_ONCLICK_ACTIVE, module_id, module_state->button_onclick_active};
								p_this->task_midi_p->sendMidiSysex(payload, sizeof(payload));
								break;
							}
							default:
								break;
						}
					};
					if(id == 0xFF) {
						for(uint8_t module_id = 0; module_id < 16; module_id++) {
							send_reply(module_id);
							vTaskDelay(5);
						}
					} else if(id < 16) {
						send_reply(id);
					}
					break;
				}
				default:
					break;
			}
		}
		vTaskDelay(0);
	}
}
