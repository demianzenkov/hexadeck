
/*
 * task_prototype.h
 *
 */

#ifndef TASK_PROTOTYPE_H_
#define TASK_PROTOTYPE_H_

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

class TaskPrototype {
public:
	virtual void createTask() = 0;
private:
protected:
	osThreadId task_handle;
};

#ifdef __cplusplus
}
#endif

#endif /* TASK_PROTOTYPE_H_ */