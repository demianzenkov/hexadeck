/*
 * task_midi.h
 *
 */

#ifndef TASK_MIDI_H_
#define TASK_MIDI_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIDI_NOTE_OFF   0x80
#define MIDI_NOTE_ON    0x90
#define MIDI_CC         0xB0

#define MIDI_CC_MODULATION  0x01
#define MIDI_CC_VOLUME      0x07

typedef struct {
    uint8_t message_type;
    uint8_t channel;
    uint8_t note;
    uint8_t value;
} midi_event_t;

void TaskMIDI_createTask();
int TaskMIDI_sendEvent(midi_event_t * event);
void USBD_MIDI_DataInHandler(uint8_t *usb_rx_buffer, uint8_t usb_rx_buffer_length);

#ifdef __cplusplus
}
#endif


#endif /* TASK_MIDI_H_ */
