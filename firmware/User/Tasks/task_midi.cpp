/*
 * task_midi.c
 *
 */

#include "task_midi.h"
#include "task_os.h"
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"

TaskMIDI task_midi;


//uint8_t midi_msg_buffer[4];
extern USBD_HandleTypeDef hUsbDevice;



void TaskMIDI::createTask() {
	midi_data_input_queue = xQueueCreate(8, sizeof(midi_data_ev_t));
    midi_output_queue = xQueueCreate(16, sizeof(midi_event_t));

	osThreadDef(MIDITask, task, osPriorityNormal, 0, 512);
	task_handle = osThreadCreate(osThread(MIDITask), this);
}

int TaskMIDI::sendMidiCC(uint8_t ch, uint8_t cc, uint8_t value)
{
	midi_event_t midi_ev = {};
	midi_ev.message_type = MIDI_CC;
	midi_ev.channel = ch;
	midi_ev.note = cc;
	midi_ev.value = value;
	sendEvent(&midi_ev);

	return 0;
}

int TaskMIDI::sendEvent(midi_event_t * ev) {
    if(xQueueSend(midi_output_queue, ev, 0) != pdPASS) {
        return -1;
    }
    return 0;
}

void TaskMIDI::task(void const *arg)
{
	TaskMIDI *p_this = (TaskMIDI *)arg;
    midi_event_t midi_ev = {};
	midi_data_ev_t midi_data_ev = {};

	uint8_t usb_midi_report[4];


	MX_USB_DEVICE_Init();
	while (1) {
        if(xQueueReceive(p_this->midi_output_queue, &midi_ev, 0) == pdTRUE) {
            // Construct USB MIDI packet with correct Code Index Number (CIN)
            // Cable Number = 0, CIN = message_type upper 4 bits
            usb_midi_report[0] = (midi_ev.message_type >> 4) & 0x0F;
            usb_midi_report[1] = midi_ev.message_type | (midi_ev.channel & 0x0F);
            usb_midi_report[2] = midi_ev.note;
            usb_midi_report[3] = midi_ev.value;
            while (MIDI_GetState() != MIDI_IDLE) {
                continue;
            };
            MIDI_SendReport(usb_midi_report, 4);
        }
		
		if(xQueueReceive(p_this->midi_data_input_queue, &midi_data_ev, 0) == pdTRUE) {
			if(midi_data_ev.len != 4) {
				continue;
			}
			while (MIDI_GetState() != MIDI_IDLE) {
                continue;
            };
			midi_event_t midi_ev;
			midi_ev.message_type = midi_data_ev.buffer[1] & 0xF0;
			midi_ev.channel = midi_data_ev.buffer[1] & 0x0F;
			midi_ev.note = midi_data_ev.buffer[2];
			midi_ev.value = midi_data_ev.buffer[3];

			xQueueSend(task_os.midi_input_event_queue, &midi_ev, 0);
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
	xQueueSendFromISR(task_midi.midi_data_input_queue, &midi_data_ev, NULL);
}

