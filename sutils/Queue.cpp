/*
 * Queue.cpp
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#include <sutils/Queue.h>

namespace vmodule {

CQueue::CQueue() :
		fLength(0) {
	// TODO Auto-generated constructor stub
	fHead.next = &fHead;
	fHead.prev = &fHead;
}

CQueue::~CQueue() {
	// TODO Auto-generated destructor stub
}

void CQueue::EnQueue(CQueueItem* item) {
	if (item->mQueue == NULL)
		return;
	//1.让最后一个元素和item首尾相连(fHead.prev记录了最后一个元素)
	fHead.prev->next = item;
	item->prev = fHead.prev;
	//2.将item的next指向head
	item->next = &fHead;
	//3.将mq->head的prev指向item
	fHead.prev = item;
	item->mQueue = this; //记录该元素已经加入到队列中
	fLength++;
}

CQueueItem* CQueue::DeQueue() {
	if (fLength <= 0)
		return NULL;
	CQueueItem *item;
	item = fHead.next;
	//修改指针
	//如果一共就一个元素则item->next本身就指向mq->head
	fHead.next = item->next;
	//如果一共就一个元素则item->next=mq->head,这里将fHead.prev指向自己
	item->next->prev = &fHead;
	item->mQueue = NULL;
	fLength--;
	return item;
}

CQueueItem* CQueue::GetHeadItem() {
	if (fLength > 0)
		return fHead.next;
	return NULL;
}

CQueueItem* CQueue::GetTailItem() {
	if (fLength > 0)
		return fHead.prev;
	return NULL;
}

void CQueue::Remove(CQueueItem* item) {
	if (NULL == item)
		return;
	if (item == &fHead)
		return;
	if (item->mQueue != this)
		return;
	item->prev->next = item->next;
	item->next->prev = item->prev;
	item->mQueue = NULL;
	fLength--;
}

void CQueueIterator::Next()
{
	if (fCurrentItem == mQueue->GetTailItem())
		fCurrentItem = NULL;
	else
		fCurrentItem = fCurrentItem->Next();
}

CQueueItem* CBlockingQueue::DeQueue(nsecs_t waitDuration) {
	Mutex::Autolock l(waitMutex);
	status_t res;
	if (mQueue.GetLength() == 0) {
		if (waitDuration == -1)
			res = waitCond.wait(waitMutex);
		else {
			res = waitCond.waitRelative(waitMutex, waitDuration);
			if (res == TIMED_OUT)
				return NULL;
		}
	}
	CQueueItem *item = mQueue.DeQueue();
	return item;
}

void CBlockingQueue::EnQueue(CQueueItem* item) {
	{
		Mutex::Autolock l(&waitMutex);
		mQueue.EnQueue(item);
	}
	waitCond.signal();
}

void CBlockingQueue::ExitQueue(){
	waitCond.broadcast();
}

} /* namespace vmodule */
