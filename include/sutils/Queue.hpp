/*
 * Queue.h
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SUTILS_QUEUE_H_
#define VMODULE_SUTILS_QUEUE_H_
#include <stdio.h>
#include <assert.h>
#include <sutils/Condition.h>
#include <sutils/Mutex.h>
#include <sutils/VmQueue.h>
#include <sutils/Timers.h>
/**
 * 通用双向循环链表
 * 尾进头出
 */
namespace vmodule {
template<typename T>
class CQueueItem;

//CQueue<student>
template<typename T>
class CQueue {
public:
	CQueue() :
			fLength(0) {
		fHead.next = &fHead;
		fHead.prev = &fHead;
	}
	virtual ~CQueue() {}
	void EnQueue(CQueueItem<T>* item);
	CQueueItem<T>* DeQueue();
	CQueueItem<T>* GetHeadItem();
	CQueueItem<T>* GetTailItem();
	void Remove(CQueueItem<T>* object);
	int GetLength() {
		return fLength;
	}
protected:
	CQueueItem<T> fHead;
	int fLength;
};

template<typename T>
void CQueue<T>::EnQueue(CQueueItem<T>* item) {
	if (item->mQueue != NULL)
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

template<typename T>
CQueueItem<T>* CQueue<T>::DeQueue() {
	if (fLength <= 0)
		return NULL;
	CQueueItem<T> *item;
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

template<typename T>
CQueueItem<T>* CQueue<T>::GetHeadItem() {
	if (fLength > 0)
		return fHead.next;
	return NULL;
}

template<typename T>
CQueueItem<T>* CQueue<T>::GetTailItem() {
	if (fLength > 0)
		return fHead.prev;
	return NULL;
}

template<typename T>
void CQueue<T>::Remove(CQueueItem<T>* item) {
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


//CQueueItem<student>
template<typename T>
class CQueueItem {
public:
	CQueueItem(T* object = NULL) 
		: next(NULL)
		, prev(NULL)
		, Object(object)
		, mQueue(NULL) {
	}

	virtual ~CQueueItem() {
		assert(mQueue == NULL);
	}

	bool IsMember(const CQueue<T >& queue) {
		return (&queue == mQueue);
	}

	bool IsMemberOfAnyQueue() {
		return mQueue != NULL;
	}

	T* GetObject() {
		return Object;
	}

	void SetObject(T* obj) {
		Object = obj;
	}

	CQueueItem<T>* Next() {
		return next;
	}

	CQueueItem<T>* Prev() {
		return prev;
	}

	CQueue<T >* InQueue() {
		return mQueue;
	}
	inline void Remove();
protected:
	CQueueItem<T> *next;
	CQueueItem<T> *prev;
	T* Object;
	CQueue<T > *mQueue; //for selef..
	friend class CQueue<T >;
};

template<typename T>
void CQueueItem<T>::Remove() {
	if (mQueue != NULL)
		mQueue->Remove(this);
}

//CQueueIterator<student>
template<typename T>
class CQueueIterator {
public:
	CQueueIterator(CQueue<T >* queue) 
		: mQueue(queue)
		, fCurrentItem(queue->GetHeadItem()) {
	}

	CQueueIterator(CQueue<T >* queue, 
		CQueueItem<T>* startItem) 
		: mQueue(queue) {
		if (startItem) {
			if (!startItem->IsMember(*queue))
				return;
			fCurrentItem = startItem;
		} else
			fCurrentItem = NULL;
	}

	~CQueueIterator() {
	}

	void Reset() {
		fCurrentItem = mQueue->GetHeadItem();
	}

	CQueueItem<T>* GetCurrent() {
		return fCurrentItem;
	}

	void Next();

	bool IsDone() {
		return fCurrentItem == NULL;
	}

private:
	CQueue<T >* mQueue;
	CQueueItem<T>* fCurrentItem;
};

template<typename T>
void CQueueIterator<T>::Next()
{
	if (fCurrentItem == mQueue->GetTailItem())
		fCurrentItem = NULL;
	else
		fCurrentItem = fCurrentItem->Next();
}


//CBlockingQueue<student>
template<typename T>
class CBlockingQueue {
public:
	CBlockingQueue() {
	}
	~CBlockingQueue() {
		waitCond.signal();
	}
	void EnQueue(CQueueItem<T >* obj);
	CQueueItem<T >* DeQueue(nsecs_t waitDuration = -1); //-1 wait forever..
	void ExitQueue();
	int GetLength(){return mQueue.GetLength();}
private:
	Condition waitCond;
	Mutex waitMutex;
	CQueue<T> mQueue;
};

template<typename T>
CQueueItem<T >* CBlockingQueue<T>::DeQueue(nsecs_t waitDuration) {
	Mutex::Autolock l(waitMutex);
	status_t res;
	if (mQueue.GetLength() == 0) {
		if (waitDuration == -1)
		res = waitCond.wait(waitMutex);
		else {
			res = waitCond.waitRelative(waitMutex, 
				waitDuration);
			if (res == TIMED_OUT)
			return NULL;
		}
	}
	CQueueItem<T > *item = mQueue.DeQueue();
	return item;
}

template<typename T>
void CBlockingQueue<T>::EnQueue(CQueueItem<T >* item) {
	{
		Mutex::Autolock l(&waitMutex);
		mQueue.EnQueue(item);
	}
	waitCond.signal();
}

template<typename T>
void CBlockingQueue<T>::ExitQueue() {
	waitCond.broadcast();
}

} /* namespace vmodule */
#endif /* QUEUE_H_ */
