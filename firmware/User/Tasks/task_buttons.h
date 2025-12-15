/*
 * task_buttons.h
 *
 */

#ifndef TASK_BUTTONS_H_
#define TASK_BUTTONS_H_

#include "main.h"
#include "cmsis_os.h"
#include "task_prototype.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	BUTTON_EVENT_SINGLE_PRESS = 0,
	BUTTON_EVENT_CENTRAL_QUAD_PRESS,
} button_event_type_e;

typedef struct {
	button_event_type_e type;
	uint8_t button_id;
	uint8_t state;
} button_event_t;

class Buttons : public TaskPrototype {
public:
	void createTask() override;
	static Buttons * getInstance();

private:
	static void task(void const *arg);

public:
	osSemaphoreId buttons_ready_sem;
private:
	uint32_t adc_values[4] = {};
	uint8_t button_state[16] = {};
	uint8_t prev_button_state[16] = {};
	uint32_t button_changed_time[16] = {};
	uint8_t button_changed[16] = {};
	uint32_t menu_entry_start = 0;
};

#ifdef __cplusplus
}
#endif


#endif /* TASK_BUTTONS_H_ */

