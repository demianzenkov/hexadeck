/*
 * task_midi.c
 *
 */

#include "task_midi.h"
#include "task_lvgl.h"
#include "task_encoder.h"
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"

osThreadId MIDITaskHandle;
QueueHandle_t midi_output_queue;
QueueHandle_t midi_input_queue;
uint8_t midi_msg_buffer[4];
extern USBD_HandleTypeDef hUsbDevice;

uint8_t usb_midi_report[4] = {
    0x19, // cable number, code index number
    0x90, // note on, channel 1
    0x3C, // note number
    0x7F, // velocity
};


static void TaskMIDI_task(void const *arg);

void TaskMIDI_createTask() {
    midi_output_queue = xQueueCreate(32, sizeof(midi_event_t));
	midi_input_queue = xQueueCreate(32, sizeof(midi_event_t));

	osThreadDef(MIDITask, TaskMIDI_task, osPriorityNormal, 0, 512);
	MIDITaskHandle = osThreadCreate(osThread(MIDITask), NULL);
}

int TaskMIDI_sendEvent(midi_event_t * ev) {
    if(xQueueSend(midi_output_queue, ev, 0) != pdPASS) {
        return -1;
    }
    return 0;
}

void TaskMIDI_task(void const *arg) {
	MX_USB_DEVICE_Init();
    midi_event_t midi_ev;
	while (1) {
        while(xQueueReceive(midi_output_queue, &midi_ev, 0) == pdTRUE) {
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
		
		while(xQueueReceive(midi_input_queue, &midi_ev, 0) == pdTRUE) {
			while (MIDI_GetState() != MIDI_IDLE) {
                continue;
            };
			if(midi_ev.note == MIDI_CC_MODULATION) {
				if(midi_ev.channel < 16) {
					ui.showBarLevel(midi_ev.channel, midi_ev.value);
					encoder_values[midi_ev.channel] = midi_ev.value;
				}
			}
			// Process incoming MIDI event
		}
		vTaskDelay(1);
	}
}


void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length)
{
	if(usb_rx_buffer_length != 4) {
		return;				
	}
	midi_event_t midi_ev;
	midi_ev.message_type = usb_rx_buffer[1] & 0xF0;
	midi_ev.channel = usb_rx_buffer[1] & 0x0F;
	midi_ev.note = usb_rx_buffer[2];
	midi_ev.value = usb_rx_buffer[3];
	xQueueSendFromISR(midi_input_queue, &midi_ev, NULL);
}

