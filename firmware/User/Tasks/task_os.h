/*
 * task_acm.h
 *
 */

#ifndef TASK_OS_H_
#define TASK_OS_H_

#include "main.h"
#include "cmsis_os.h"
#include "task_prototype.h"
#include "task_encoder.h"


#define ENCODER_DEFAULT_VALUE 		64
#define ENCODER_DEFAULT_MIN_VALUE   0
#define ENCODER_DEFAULT_MAX_VALUE   127
#define ENCODER_DEFAULT_STEP   		1

#define MIDI_DEFAULT_CHANNEL		0	// Channel - 1


#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	uint8_t channel;
	uint8_t cc;
	uint8_t min_value;
	uint8_t max_value;
	uint8_t current_value;
	uint8_t step;
	uint8_t name[16 + 1];
} module_state_t;



class TaskOS : public TaskPrototype {
public:
	TaskOS();
	void createTask();
private:
	static void task(void const *arg);

public:
	QueueHandle_t encoder_event_queue;
	QueueHandle_t button_event_queue;
	QueueHandle_t acm_event_queue;
	QueueHandle_t midi_input_event_queue;
	QueueHandle_t midi_sysex_input_event_queue;
	
	module_state_t module_states[16];

};

extern TaskOS task_os;

#ifdef __cplusplus
}
#endif


#endif /* TASK_OS_H_ */
