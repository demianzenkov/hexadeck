/*
 * task_encoder.h
 *
 */

#ifndef TASK_ENCODER_H_
#define TASK_ENCODER_H_

#include "main.h"
#include "cmsis_os.h"
#include "task_prototype.h"

#ifdef __cplusplus
extern "C" {
#endif




#define ENC_DEBOUNCE_HIGH_FILTER 50
#define ENC_DEBOUNCE_LOW_FILTER 3000
#define ENC_DEBOUNCE_CMD_FILTER pdMS_TO_TICKS(250)

typedef struct {
	uint8_t encoder_id;
    uint8_t state_a;
    uint8_t prev_state_a;
    uint8_t state_b;
    uint8_t prev_state_b;
} encoder_state_t;

typedef struct {
	bool increase;		// true - increase, false - decrease
	uint8_t encoder_id;
} encoder_event_t;


class TaskEncoder : public TaskPrototype {
public:
	static TaskEncoder * getInstance();
	void createTask() override;
private:
	static void task(void const *arg);
public:
	QueueHandle_t encoder_state_queue;	// queue for interrupt events
	encoder_state_t encoder_state[16] = {};
};

// extern TaskEncoder task_encoder;

#ifdef __cplusplus
}
#endif


#endif /* TASK_ENCODER_H_ */
