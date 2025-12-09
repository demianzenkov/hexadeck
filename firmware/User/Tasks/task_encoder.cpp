/*
 * task_encoder.c
 *
 */

#include "task_encoder.h"
#include "task_lvgl.h"
#include "task_midi.h"
#include "task_os.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "string.h"



TaskEncoder task_encoder;

void TaskEncoder::createTask()
{
	encoder_state_queue = xQueueCreate(32, sizeof(encoder_state_t *));

	osThreadDef(encoderTask, task, osPriorityNormal, 0, 512);
	task_handle = osThreadCreate(osThread(encoderTask), this);
}

void TaskEncoder::task(void const *arg)
{
	TaskEncoder *p_this = (TaskEncoder *)arg;
	
	// midi_event_t enc_midi_ev = {};

	for (uint8_t i = 0; i < 16; i++)
	{
		p_this->encoder_state[i].encoder_id = i;
		p_this->encoder_state[i].state_a = 1;
		p_this->encoder_state[i].prev_state_a = 1;
		p_this->encoder_state[i].state_b = 1;
		p_this->encoder_state[i].prev_state_b = 1;
	}

	while (1)
	{
		// Read event from interrupts
		encoder_state_t * ev;
		if (xQueueReceive(p_this->encoder_state_queue, &ev, portMAX_DELAY) == pdTRUE)
		{
			if ((ev->prev_state_a == 0) && (ev->state_a == 1))
			{
				if (ev->state_b == 1)
				{
					encoder_event_t enc_ev = {.increase = false, .encoder_id = ev->encoder_id};
					xQueueSend(task_os.encoder_event_queue, &enc_ev, 0);
				}
				else if (ev->state_b == 0)
				{
					encoder_event_t enc_ev = {.increase = true, .encoder_id = ev->encoder_id};
					xQueueSend(task_os.encoder_event_queue, &enc_ev, 0);
				}
			}
		}
	}
}
