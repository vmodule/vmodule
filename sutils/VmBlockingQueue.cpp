/*
 * VmBlockingQueue.cpp
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#include <sutils/VmBlockingQueue.h>
#include <sutils/Errors.h>
namespace vmodule {

VmBlockingQueue::VmBlockingQueue() {
	// TODO Auto-generated constructor stub
	mqueue_init(&mQueue);
}

VmBlockingQueue::~VmBlockingQueue() {
	// TODO Auto-generated destructor stub
	waitCond.signal();
	mqueue_free(&mQueue);
}

MQUEUE_ITEM* VmBlockingQueue::DeQueue(nsecs_t waitDuration) {
	Mutex::Autolock l(waitMutex);
	status_t res;
	if (mQueue.fLength == 0) {
		if (waitDuration == -1)
			res = waitCond.wait(waitMutex);
		else {
			res = waitCond.waitRelative(waitMutex, waitDuration);
			if (res == TIMED_OUT)
				return NULL;
		}
	}
	MQUEUE_ITEM *item = mqueue_pop(&mQueue);
	return item;
}

void VmBlockingQueue::EnQueue(MQUEUE_ITEM* item) {
	{
		Mutex::Autolock l(&waitMutex);
		mqueue_push(&mQueue, item);
	}
	waitCond.signal();
}

} /* namespace vmodule */
