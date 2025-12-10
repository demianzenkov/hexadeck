/*
 * task_midi.h
 *
 */

#ifndef TASK_MIDI_H_
#define TASK_MIDI_H_

#include "main.h"
#include "cmsis_os.h"
#include "task_prototype.h"
#include "api_midi.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	MIDI_SYS_SET_NAME = 0,
	MIDI_SYS_SET_VALUE,
	MIDI_SYS_SET_CHANNEL,
	MIDI_SYS_SET_CC,
	MIDI_SYS_SET_RANGE,
	MIDI_SYS_SET_COLOR_BG,
	MIDI_SYS_SET_COLOR_BORDER,
	MIDI_SYS_SET_COLOR_TEXT,
	MIDI_SYS_SET_COLOR_BAR,
	MIDI_SYS_FIRMWARE_UPDATE,
} midi_sys_event_type_e; 


typedef struct {
    uint8_t message_type;
    uint8_t channel;
    uint8_t note;
    uint8_t value;
} midi_event_t;

typedef struct {
    uint8_t buffer[32];
	size_t len;
} midi_sysex_event_t;

typedef struct {
    uint8_t buffer[64];
	size_t len;
} midi_data_ev_t;


// void TaskMIDI_createTask();
// int TaskMIDI_sendEvent(midi_event_t * event);
void USBD_MIDI_DataInHandler(uint8_t * usb_rx_buffer, uint8_t usb_rx_buffer_length);

class TaskMIDI : public TaskPrototype {
public:
	void createTask() override;
	int sendEvent(midi_event_t * ev);
	int sendMidiCC(uint8_t ch, uint8_t cc, uint8_t value);
private:
	static void task(void const *arg);
	int parseSysexMessage(uint8_t * data_buffer, size_t len, midi_sysex_event_t * out_sysex_ev);
public:
	QueueHandle_t midi_data_input_queue;
	QueueHandle_t midi_output_queue;
};

extern TaskMIDI task_midi;

#ifdef __cplusplus
}
#endif


#endif /* TASK_MIDI_H_ */
