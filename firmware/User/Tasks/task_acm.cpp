/*
 * task_acm.c
 *
 */

#include "task_acm.h"
#include "task_lvgl.h"
#include "task_encoder.h"
#include "main.h"
#include "task_midi.h"

// extern QueueHandle_t show_image_queue;
// uint8_t pic_buffer[25604];

ACM acm;

void ACM::createTask() {
    acm_event_queue = xQueueCreate(16, sizeof(acm_event_t));

	osThreadDef(ACMTask, task, osPriorityNormal, 0, 1024);
	ACMTaskHandle = osThreadCreate(osThread(ACMTask), this);
}

void ACM::task(void const *arg) {
	ACM *p_this = (ACM *)arg;

	acm_event_t acm_ev = {};
//	size_t packet_cnt = 0;
//	size_t img_size = 0;

	char command_string_buffer[512] = {0};
	size_t command_string_len = 0;
	while (1) {
        if(xQueueReceive(p_this->acm_event_queue, &acm_ev, portMAX_DELAY) == pdTRUE)
        {
			// Check if the buffer contains "\n" to indicate end of command
			// If it doen't - accumulate the buffer
			if(acm_ev.buffer[acm_ev.len - 1] != '\n')
			{
				// Accumulate the buffer
				if(command_string_len + acm_ev.len < sizeof(command_string_buffer) - 1)
				{
					memcpy(command_string_buffer + command_string_len, acm_ev.buffer, acm_ev.len);
					command_string_len += acm_ev.len;
					command_string_buffer[command_string_len] = '\0'; // Null-terminate the string
					continue; // Wait for more data
				}
			} else {
				// If the buffer ends with "\n", copy it to command_string_buffer
				if(command_string_len + acm_ev.len < sizeof(command_string_buffer) - 1)
				{
					memcpy(command_string_buffer + command_string_len, acm_ev.buffer, acm_ev.len);
					command_string_len += acm_ev.len;
					command_string_buffer[command_string_len] = '\0'; // Null-terminate the string
					p_this->parseInputBuffer(command_string_buffer);
				}
				command_string_len = 0; // Reset command string length
			}

            // memcpy(pic_buffer+img_size, acm_ev.buffer, acm_ev.len);
            // img_size += acm_ev.len;
        	// packet_cnt++;
            // if(img_size >= sizeof(pic_buffer))
            // {
            //     show_img_t show_img;
            //     show_img.display_id = 0;
            //     show_img.img_buf = pic_buffer;
            //     show_img.img_size = img_size;
            //     xQueueSend(show_image_queue, &show_img, portMAX_DELAY);
            //     img_size = 0;
            //     packet_cnt = 0;
            // }

        }
	}
}

void ACM::parseInputBuffer(char * buffer) {
	// Check string for commands
	// "/set/name/x/y" - where x - display id, y - string value
	if(buffer[0] == '/') {
		char * cmd = (char *)buffer;
		if(strncmp(cmd, "/set/name/", 10) == 0) {
			cmd += 10; // skip "/set/name/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				ui.showName(disp_id, cmd);
				CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
			}
			else {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
			}
		}
		else if(strncmp(cmd, "/set/level/", 11) == 0) {
			cmd += 11; // skip "/set/value/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				int32_t level = atoi(cmd);
				if((level < 0) || (level > 127)) {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
					return;
				}
				ui.showBarLevel(disp_id, level);
				encoder_values[disp_id] = level;
				CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
				return;
			}
			else {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
			}
		}
		else if(strncmp(cmd, "/set/value/", 11) == 0) {
			cmd += 11; // skip "/set/channel/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				int str_len = strlen(cmd);
				if(str_len > 18) {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
					return;
				}
				ui.showValue(disp_id, cmd);
				CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
			}
			else {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
			}
		}
		else if(strncmp(cmd, "/set/channel/", 13) == 0) {
			cmd += 13; // skip "/set/channel/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				int str_len = strlen(cmd);
				if(str_len > 4) {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
					return;
				}
				ui.showChannel(disp_id, cmd);
				CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
			}
			else {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
			}
		}
		else if(strncmp(cmd, "/set/range/", 11) == 0) {
			cmd += 11; // skip "/set/range/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				int32_t max_level = atoi(cmd);
				if((max_level < 1) || (max_level > 127)) {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
					return;
				}
				encoder_max_values[disp_id] = max_level;
				ui.changeBarRange(disp_id, max_level);
				
				if(encoder_values[disp_id] > max_level) {
					encoder_values[disp_id] = max_level;

					midi_event_t enc_midi_ev = {};
					enc_midi_ev.message_type = MIDI_CC;
					enc_midi_ev.channel = disp_id;
					enc_midi_ev.note = MIDI_CC_MODULATION;
					enc_midi_ev.value = encoder_values[disp_id];
					
					ui.showBarLevel(disp_id, encoder_values[disp_id]);
					TaskMIDI_sendEvent(&enc_midi_ev);
				}
				CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
				return;
			}
			else {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
			}
		}
		// /set/color/bg/x/ff0012 - where x - display id, 0xff - r, 0x00 - g, 0x12 - b
		else if(strncmp(cmd, "/set/color/bg/", 14) == 0) {
			cmd += 14; // skip "/set/color/bg/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				uint32_t color_value = 0;
				if(sscanf(cmd, "%6x", &color_value) == 1) {
					uint8_t r = (color_value >> 16) & 0xFF;
					uint8_t g = (color_value >> 8) & 0xFF;
					uint8_t b = color_value & 0xFF;
					lv_color_t color = lv_color_make(r, g, b);
					ui.showColor(disp_id, COLOR_ELEMENT_BACKGROUND, color);
					CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
				}
				else {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				}
			}
		}
		else if(strncmp(cmd, "/set/color/border/", 18) == 0) {
			cmd += 18; // skip "/set/color/border/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				uint32_t color_value = 0;
				if(sscanf(cmd, "%6x", &color_value) == 1) {
					uint8_t r = (color_value >> 16) & 0xFF;
					uint8_t g = (color_value >> 8) & 0xFF;
					uint8_t b = color_value & 0xFF;
					lv_color_t color = lv_color_make(r, g, b);
					ui.showColor(disp_id, COLOR_ELEMENT_BORDER, color);
					CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
				}
				else {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				}
			}
		}
		else if(strncmp(cmd, "/set/color/text/", 16) == 0) {
			cmd += 16; // skip "/set/color/text/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				uint32_t color_value = 0;
				if(sscanf(cmd, "%6x", &color_value) == 1) {
					uint8_t r = (color_value >> 16) & 0xFF;
					uint8_t g = (color_value >> 8) & 0xFF;
					uint8_t b = color_value & 0xFF;
					lv_color_t color = lv_color_make(r, g, b);
					ui.showColor(disp_id, COLOR_ELEMENT_TEXT, color);
					CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
				}
			}
			else {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
			}
		}
		else if(strncmp(cmd, "/set/color/bar/", 15) == 0) {
			cmd += 15; // skip "/set/color/bar/"
			uint8_t disp_id = atoi(cmd);
			if(disp_id > 15) {
				CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				return;
			}
			cmd = strchr(cmd, '/');
			if(cmd != NULL) {
				cmd++; // skip '/'
				uint32_t color_value = 0;
				if(sscanf(cmd, "%6x", &color_value) == 1) {
					uint8_t r = (color_value >> 16) & 0xFF;
					uint8_t g = (color_value >> 8) & 0xFF;
					uint8_t b = color_value & 0xFF;
					lv_color_t color = lv_color_make(r, g, b);
					ui.showColor(disp_id, COLOR_ELEMENT_BAR, color);
					CDC_Transmit(0, (uint8_t *)"OK\n\r", 4);
				}
				else {
					CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
				}
			}
		}
		
		else {
			CDC_Transmit(0, (uint8_t *)"FAIL\n\r", 6);
		}
	}
}
