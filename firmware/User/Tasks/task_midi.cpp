/*
 * task_midi.cpp
 *
 */

#include "task_midi.h"
#include "task_os.h"
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"


//uint8_t midi_msg_buffer[4];
extern USBD_HandleTypeDef hUsbDevice;


TaskMIDI * TaskMIDI::getInstance()
{
	static TaskMIDI instance;
	return &instance;
}

void TaskMIDI::createTask() {
	midi_data_input_queue = xQueueCreate(8, sizeof(midi_data_ev_t));
    midi_output_queue = xQueueCreate(16, sizeof(midi_event_t));
	midi_sysex_output_queue = xQueueCreate(32, sizeof(midi_sysex_event_t));

	osThreadDef(MIDITask, task, osPriorityNormal, 0, 512);
	task_handle = osThreadCreate(osThread(MIDITask), this);
}

int TaskMIDI::sendMidiCC(uint8_t ch, uint8_t cc, uint8_t value)
{
	midi_event_t midi_ev = {};
	midi_ev.message_type = MIDI_MSG_TYPE_CC;
	midi_ev.channel = ch;
	midi_ev.note = cc;
	midi_ev.value = value;
	sendEvent(&midi_ev);

	return 0;
}

int TaskMIDI::sendMidiSysex(const uint8_t *payload, size_t len)
{
	if(!payload || len == 0) {
		return -1;
	}
	midi_sysex_event_t sysex_ev = {};
	if(len + 3 > sizeof(sysex_ev.buffer)) {
		return -2;
	}
	sysex_ev.buffer[0] = MIDI_MSG_STATUS_SYSEX_START;
	sysex_ev.buffer[1] = 0x7D; // Non-commercial manufacturer ID
	memcpy(&sysex_ev.buffer[2], payload, len);
	sysex_ev.buffer[len + 2] = MIDI_MSG_STATUS_SYSEX_END;
	sysex_ev.len = len + 3;
	if(xQueueSend(midi_sysex_output_queue, &sysex_ev, 0) != pdPASS) {
		return -3;
	}
	return 0;
}

int TaskMIDI::sendEvent(midi_event_t * ev) {
    if(xQueueSend(midi_output_queue, ev, 0) != pdPASS) {
        return -1;
    }
    return 0;
}

int TaskMIDI::parseSysexMessage(uint8_t * data_buffer, size_t len, midi_sysex_event_t * out_sysex_ev) {
	// Universal SysEx parser for USB MIDI packets
	if (!data_buffer || !out_sysex_ev || len == 0) {
		return -1;
	}

	out_sysex_ev->len = 0;
	bool sysex_start_skipped = false;

	for (size_t i = 0; i + 3 < len; i += 4) {
		uint8_t cin = data_buffer[i] & 0x0F;
		int data_bytes = 0;
		switch (cin) {
			case 0x4: data_bytes = 3; break; // SysEx start/continue (3 bytes)
			case 0x5: data_bytes = 1; break; // SysEx ends with 1 byte
			case 0x6: data_bytes = 2; break; // SysEx ends with 2 bytes
			case 0x7: data_bytes = 3; break; // SysEx ends with 3 bytes
			default: data_bytes = 3; break; // Defensive: treat as 3
		}
		for (int j = 1; j <= data_bytes; ++j) {
			uint8_t val = data_buffer[i + j];
			if (!sysex_start_skipped && val == MIDI_MSG_STATUS_SYSEX_START) {
				sysex_start_skipped = true;
				continue;
			}
			// Only treat 0xF7 as end if this is a SysEx end packet (CIN 0x5, 0x6, 0x7) and this is the last data byte(s)
			bool is_sysex_end_packet = (cin == 0x5 || cin == 0x6 || cin == 0x7);
			bool is_last_data_byte = (j == data_bytes);
			if (is_sysex_end_packet && is_last_data_byte && val == MIDI_MSG_STATUS_SYSEX_END) {
				// Do not include this 0xF7 in buffer, stop parsing
				return 0;
			}
			if (out_sysex_ev->len < sizeof(out_sysex_ev->buffer)) {
				out_sysex_ev->buffer[out_sysex_ev->len++] = val;
			} else {
				return -2; // Buffer overflow
			}
		}
	}

	return 0;
}

void TaskMIDI::task(void const *arg)
{
	TaskMIDI *p_this = (TaskMIDI *)arg;
    midi_event_t midi_ev = {};
	midi_data_ev_t midi_data_ev = {};
	midi_sysex_event_t midi_sysex_ev = {};


	MX_USB_DEVICE_Init();
	while (1) {
		if(xQueueReceive(p_this->midi_sysex_output_queue, &midi_sysex_ev, 0) == pdTRUE) {
			size_t index = 0;
			while(index < midi_sysex_ev.len) {
				size_t remaining = midi_sysex_ev.len - index;
				uint8_t usb_midi_report[4] = {0};
				uint8_t cin = 0x4;
				if(remaining >= 3) {
					if(remaining == 3) {
						cin = 0x7;
					}
					usb_midi_report[1] = midi_sysex_ev.buffer[index++];
					usb_midi_report[2] = midi_sysex_ev.buffer[index++];
					usb_midi_report[3] = midi_sysex_ev.buffer[index++];
				} else if(remaining == 2) {
					cin = 0x6;
					usb_midi_report[1] = midi_sysex_ev.buffer[index++];
					usb_midi_report[2] = midi_sysex_ev.buffer[index++];
					usb_midi_report[3] = 0x00;
				} else {
					cin = 0x5;
					usb_midi_report[1] = midi_sysex_ev.buffer[index++];
					usb_midi_report[2] = 0x00;
					usb_midi_report[3] = 0x00;
				}
				usb_midi_report[0] = cin;
				while (MIDI_GetState() != MIDI_IDLE) {
					continue;
				};
				MIDI_SendReport(usb_midi_report, 4);
			}
		}

		if(xQueueReceive(p_this->midi_output_queue, &midi_ev, 0) == pdTRUE) {
			if(midi_ev.message_type == MIDI_MSG_TYPE_CC) {
				uint8_t usb_midi_report[4];
				usb_midi_report[0] = (midi_ev.message_type >> 4) & 0x0F;
				usb_midi_report[1] = midi_ev.message_type | (midi_ev.channel & 0x0F);
				usb_midi_report[2] = midi_ev.note;
				usb_midi_report[3] = midi_ev.value;
				while (MIDI_GetState() != MIDI_IDLE) {
					continue;
				};
				MIDI_SendReport(usb_midi_report, 4);
			}
            // Construct USB MIDI packet with correct Code Index Number (CIN)
            // Cable Number = 0, CIN = message_type upper 4 bits
        }
		
		if(xQueueReceive(p_this->midi_data_input_queue, &midi_data_ev, 0) == pdTRUE) {
			// Process received MIDI data
			while (MIDI_GetState() != MIDI_IDLE) {
                continue;
            };
			midi_event_t midi_ev;
			uint8_t midi_cable_num = (midi_data_ev.buffer[0] >> 4) & 0x0F;
			uint8_t midi_cin = midi_data_ev.buffer[0] & 0x0F;

			midi_ev.message_type = midi_data_ev.buffer[1] & 0xF0;
			switch(midi_ev.message_type) {
				case MIDI_MSG_TYPE_NOTE_OFF:
					break;
				case MIDI_MSG_TYPE_NOTE_ON:
					break;
				case MIDI_MSG_TYPE_POLY_AT:
					break;
				case MIDI_MSG_TYPE_CC: {
					midi_ev.channel = midi_data_ev.buffer[1] & 0x0F;
					midi_ev.note = midi_data_ev.buffer[2];
					midi_ev.value = midi_data_ev.buffer[3];
					break;
				}
				case MIDI_MSG_TYPE_PC:
					break;
				case MIDI_MSG_TYPE_CH_AT:
					break;
				case MIDI_MSG_TYPE_PW:
					break;
				case MIDI_MSG_TYPE_SYS: {
					if(midi_data_ev.buffer[1] == MIDI_MSG_STATUS_SYSEX_START) {
						midi_sysex_event_t sysex_ev = {};
						if(p_this->parseSysexMessage(midi_data_ev.buffer, midi_data_ev.len, &sysex_ev) == 0) {
							xQueueSend(TaskOS::getInstance()->midi_sysex_input_event_queue, &sysex_ev, 0);
						}
					}
					continue;
				}

				default:
					// Unsupported message type
					continue;
			}
			
			xQueueSend(TaskOS::getInstance()->midi_input_event_queue, &midi_ev, 0);
		}

		vTaskDelay(1);
	}
}


void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length)
{
	midi_data_ev_t midi_data_ev = {};
	if(!usb_rx_buffer_length) {
		return;
	}
	memcpy(midi_data_ev.buffer, usb_rx_buffer, usb_rx_buffer_length);
	midi_data_ev.len = usb_rx_buffer_length;

	xQueueSendFromISR(TaskMIDI::getInstance()->midi_data_input_queue, &midi_data_ev, NULL);
}

