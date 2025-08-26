/*
 * task_encoder.h
 *
 */

#ifndef TASK_ENCODER_H_
#define TASK_ENCODER_H_

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ENCODER_MIN_VALUE   0
#define ENCODER_MAX_VALUE   127

typedef struct {
	uint8_t encoder_id;
    uint8_t state_a;
    uint8_t prev_state_a;
    uint8_t state_b;
    uint8_t prev_state_b;
    uint8_t prev_state;
    uint32_t ts;
    uint32_t prev_ts;
} encoder_ev_t;

extern uint8_t encoder_values[16];
extern uint8_t encoder_max_values[16];
void TaskEncoder_createTask();


#ifdef __cplusplus
}
#endif


#endif /* TASK_ENCODER_H_ */
