/*
 * task_acm.h
 *
 */

#ifndef TASK_ACM_H_
#define TASK_ACM_H_

#include "main.h"
#include "usbd_cdc_acm_if.h"
#include "cmsis_os.h"
#include "task_prototype.h"

#ifdef __cplusplus
extern "C" {
#endif


#define ACM_RESPONSE_OK       		"OK\n\r"
#define ACM_RESPONSE_OK_LEN 		4
#define ACM_RESPONSE_FAIL     		"FAIL\n\r"
#define ACM_RESPONSE_FAIL_LEN 		6

typedef struct {
    uint8_t buffer[32];
    size_t len;
} acm_data_event_t;

typedef enum {
	ACM_EVENT_TYPE_SET_NAME = 0,
	ACM_EVENT_TYPE_SET_VALUE,
	ACM_EVENT_TYPE_SET_CHANNEL,
	ACM_EVENT_TYPE_SET_CC,
	ACM_EVENT_TYPE_SET_RANGE,
	ACM_EVENT_TYPE_SET_COLOR_BG,
	ACM_EVENT_TYPE_SET_COLOR_BORDER,
	ACM_EVENT_TYPE_SET_COLOR_TEXT,
	ACM_EVENT_TYPE_SET_COLOR_BAR,
	ACM_EVENT_FIRMWARE_UPDATE,
} acm_event_type_e; 

typedef struct {
	acm_event_type_e type;
	uint8_t id;
	uint8_t data[16];
} acm_event_t;


class ACM : public TaskPrototype {
public:
	void createTask() override;
	static ACM * getInstance();
	int sendData(uint8_t * data, size_t len);
private:
	static void task_recv(void const *arg);
	static void task_send(void const *arg);
	void parseInputBuffer(char * buffer);
public:
	QueueHandle_t recv_data_queue;
	QueueHandle_t send_data_queue;
private:
	osThreadId task_send_handle;
};


#ifdef __cplusplus
}
#endif


#endif /* TASK_MIDI_H_ */
