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


class Buttons : public TaskPrototype {
public:
	void createTask() override;
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
	uint32_t bootloader_entry_start = 0;
};

extern Buttons buttons;

#ifdef __cplusplus
}
#endif


#endif /* TASK_BUTTONS_H_ */

