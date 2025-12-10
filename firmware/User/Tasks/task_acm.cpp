/*
 * task_acm.c
 *
 */

#include "task_acm.h"
#include "task_os.h"
#include "main.h"
#include "task_midi.h"
#include "bootloader.h"


ACM acm;

void ACM::createTask()
{
	recv_data_queue = xQueueCreate(32, sizeof(acm_data_event_t));
	send_data_queue = xQueueCreate(16, sizeof(acm_data_event_t));

	osThreadDef(ACMRecvTask, task_recv, osPriorityNormal, 0, 256);
	task_handle = osThreadCreate(osThread(ACMRecvTask), this);

	osThreadDef(ACMSendTask, task_send, osPriorityNormal, 0, 256);
	task_send_handle = osThreadCreate(osThread(ACMSendTask), this);
}

int ACM::sendData(uint8_t * data, size_t len)
{
	acm_data_event_t acm_ev = {};
	if(len > sizeof(acm_ev.buffer)) {
		return -1;
	}
	memcpy(acm_ev.buffer, data, len);
	acm_ev.len = len;
	if(xQueueSend(send_data_queue, &acm_ev, 0) != pdPASS) {
		return -1;
	}
	return 0;
}

void ACM::parseInputBuffer(char *buffer)
{
	// Fast command parsing using pointer arithmetic and memcmp
	if (buffer[0] != '/') {
		sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		return;
	}
	acm_event_t acm_event = {};
	char *cmd = buffer + 1; // skip initial '/'
	if (memcmp(cmd, "set/name/", 9) == 0) {
		cmd += 9;
		uint8_t disp_id = atoi(cmd);
		if ((disp_id == 0) || (disp_id > 16)) {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		cmd = strchr(cmd, '/');
		if (cmd != NULL) {
			cmd++;
			int str_len = strlen(cmd);
			if (str_len > 16) {
				sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
				return;
			}
			acm_event.type = ACM_EVENT_TYPE_SET_NAME;
			acm_event.id = disp_id - 1;
			memcpy(acm_event.data, cmd, str_len - 1);
			xQueueSend(task_os.acm_event_queue, &acm_event, 0);
			sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		}
	}
	
	else if (memcmp(cmd, "set/value/", 10) == 0) {
		cmd += 10;
		uint8_t disp_id = atoi(cmd);
		if ((disp_id == 0) || (disp_id > 16)) {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		cmd = strchr(cmd, '/');
		if (cmd != NULL) {
			cmd++;
			int32_t level = atoi(cmd);
			if ((level < 0) || (level > 127)) {
				sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
				return;
			}
			acm_event.type = ACM_EVENT_TYPE_SET_VALUE;
			acm_event.id = disp_id - 1;
			acm_event.data[0] = (uint8_t)level;
			xQueueSend(task_os.acm_event_queue, &acm_event, 0);
			
			sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
			return;
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		}
	}
	
	else if (memcmp(cmd, "set/channel/", 12) == 0) {
		cmd += 12;
		uint8_t disp_id = atoi(cmd);
		if ((disp_id == 0) || (disp_id > 16)) {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		cmd = strchr(cmd, '/');
		if (cmd != NULL) {
			cmd++;
			int32_t channel = atoi(cmd);
			if (channel > 16) {
				sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
				return;
			}
			acm_event.type = ACM_EVENT_TYPE_SET_CHANNEL;
			acm_event.id = disp_id - 1;
			acm_event.data[0] = (uint8_t)channel;
			xQueueSend(task_os.acm_event_queue, &acm_event, 0);
			sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		}
	}
	
	else if(memcmp(cmd, "set/cc/", 7) == 0) {
		cmd += 7;
		uint8_t disp_id = atoi(cmd);
		if ((disp_id == 0) || (disp_id > 16)) {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		cmd = strchr(cmd, '/');
		if (cmd != NULL) {
			cmd++;
			int32_t cc_number = atoi(cmd);
			if ((cc_number < 0) || (cc_number > 127)) {
				sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
				return;
			}
			acm_event.type = ACM_EVENT_TYPE_SET_CC;
			acm_event.id = disp_id - 1;
			acm_event.data[0] = (uint8_t)cc_number;
			xQueueSend(task_os.acm_event_queue, &acm_event, 0);
			sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		}
	}
	
	else if (memcmp(cmd, "set/range/", 10) == 0) {
		cmd += 10;
		uint8_t disp_id = atoi(cmd);
		if ((disp_id == 0) || (disp_id > 16)) {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		cmd = strchr(cmd, '/');
		if (cmd != NULL) {
			cmd++;
			int32_t max_level = atoi(cmd);
			if ((max_level < 1) || (max_level > 127)) {
				sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
				return;
			}
			acm_event.type = ACM_EVENT_TYPE_SET_RANGE;
			acm_event.id = disp_id - 1;
			acm_event.data[0] = (uint8_t)max_level;
			xQueueSend(task_os.acm_event_queue, &acm_event, 0);
			sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
			return;
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		}
	}
	
	else if (memcmp(cmd, "set/color/", 10) == 0) {
		cmd += 10;
		uint8_t selected_color_element = 0;
		if(memcpy(cmd, "bg/", 3) == 0) {
			cmd += 3;
			selected_color_element = 1;
		} else if(memcpy(cmd, "border/", 7) == 0) {
			cmd += 7;
			selected_color_element = 2;
		} else if(memcpy(cmd, "text/", 5) == 0) {
			cmd += 5;
			selected_color_element = 3;
		} else if(memcpy(cmd, "bar/", 4) == 0) {
			cmd += 4;
			selected_color_element = 4;
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		uint8_t disp_id = atoi(cmd);
		if ((disp_id == 0) || (disp_id > 16)) {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			return;
		}
		cmd = strchr(cmd, '/');
		if (cmd != NULL) {
			cmd++;
			uint32_t color_value = 0;
			if (sscanf(cmd, "%6x", &color_value) == 1) {
				uint8_t r = (color_value >> 16) & 0xFF;
				uint8_t g = (color_value >> 8) & 0xFF;
				uint8_t b = color_value & 0xFF;
				acm_event.type = (acm_event_type_e)(ACM_EVENT_TYPE_SET_COLOR_BG + selected_color_element - 1);
				acm_event.id = disp_id - 1;
				acm_event.data[0] = r;
				acm_event.data[1] = g;
				acm_event.data[2] = b;
				xQueueSend(task_os.acm_event_queue, &acm_event, 0);
				sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
			} else {
				sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
			}
		} else {
			sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
		}
	}
	
	else if (memcmp(cmd, "fw/update", 9) == 0) {
		// Firmware update command received
		sendData((uint8_t *)ACM_RESPONSE_OK, ACM_RESPONSE_OK_LEN);
		HAL_Delay(100);
		acm_event.type = ACM_EVENT_FIRMWARE_UPDATE;
		xQueueSend(task_os.acm_event_queue, &acm_event, 0);
	}
	else {
		sendData((uint8_t *)ACM_RESPONSE_FAIL, ACM_RESPONSE_FAIL_LEN);
	}

}


void ACM::task_recv(void const *arg)
{
	ACM *p_this = (ACM *)arg;

	acm_data_event_t acm_ev = {};

	char command_string_buffer[128] = {0};
	size_t command_string_len = 0;
	while (1)
	{
		if(xQueueReceive(p_this->recv_data_queue, &acm_ev, portMAX_DELAY) == pdTRUE)
		{
			// Check if the buffer contains "\n" to indicate end of command
			// If it doen't - accumulate the buffer
			if (acm_ev.buffer[acm_ev.len - 1] != '\n') {
				// Accumulate the buffer
				if (command_string_len + acm_ev.len < sizeof(command_string_buffer) - 1)
				{
					memcpy(command_string_buffer + command_string_len, acm_ev.buffer, acm_ev.len);
					command_string_len += acm_ev.len;
					command_string_buffer[command_string_len] = '\0'; // Null-terminate the string
					continue;										  // Wait for more data
				}
			} 
			else {
				// If the buffer ends with "\n", copy it to command_string_buffer
				if (command_string_len + acm_ev.len < sizeof(command_string_buffer) - 1)
				{
					memcpy(command_string_buffer + command_string_len, acm_ev.buffer, acm_ev.len);
					command_string_len += acm_ev.len;
					command_string_buffer[command_string_len] = '\0'; // Null-terminate the string
					p_this->parseInputBuffer(command_string_buffer);
				}
				command_string_len = 0; // Reset command string length
			}
		}
	}
}

void ACM::task_send(void const *arg)
{
	ACM *p_this = (ACM *)arg;

	acm_data_event_t acm_ev = {};
	while (1)
	{
		if(xQueueReceive(p_this->send_data_queue, &acm_ev, portMAX_DELAY) == pdTRUE) {
			CDC_Transmit(0, acm_ev.buffer, acm_ev.len);
		}
	}
}
