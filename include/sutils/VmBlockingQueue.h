/*
 * VmBlockingQueue.h
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_BLOCKING_QUEUE_H_
#define VMODULE_BLOCKING_QUEUE_H_
#include <sutils/Condition.h>
#include <sutils/Mutex.h>
#include <sutils/VmQueue.h>
#include <sutils/Timers.h>
namespace vmodule {

class VmBlockingQueue {
public:
	VmBlockingQueue();
	virtual ~VmBlockingQueue();
	MQUEUE_ITEM* DeQueue(nsecs_t waitDuration = -1); //-1 wait forever..
	void EnQueue(MQUEUE_ITEM* item);
	void ExitQueue();
private:
	Condition waitCond;
	Mutex waitMutex;
	MQUEUE mQueue;
};

} /* namespace vmodule */
#endif /* MQUEUEBLOCKING_H_ */
