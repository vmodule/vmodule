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

class CQueue;
class CQueueItem {
public:
	CQueueItem(void* object = NULL)
		: next(NULL)
		, prev(NULL)
		, Object(object)
		, mQueue(NULL)
	{
	}

	virtual ~CQueueItem()
	{
		assert(mQueue == NULL);
	}

	bool IsMember(const CQueue& queue)
	{
		return (&queue == mQueue);
	}

	bool IsMemberOfAnyQueue()
	{
		return mQueue != NULL;
	}

	void* GetObject()
	{
		return Object;
	}

	void SetObject(void* obj)
	{
		Object = obj;
	}

	CQueueItem* Next()
	{
		return next;
	}

	CQueueItem* Prev()
	{
		return prev;
	}

	CQueue* InQueue()
	{
		return mQueue;
	}

	inline void Remove();

protected:
	CQueueItem *next;
	CQueueItem *prev;
	void* Object;
	CQueue *mQueue; //for selef..
	friend class CQueue;
};

class CQueue {
public:
	CQueue();
	virtual ~CQueue();
	void EnQueue(CQueueItem* item);
	CQueueItem* DeQueue();
	CQueueItem* GetHeadItem();
	CQueueItem* GetTailItem();
	void Remove(CQueueItem* object);
	int GetLength() {return fLength;}
protected:
	CQueueItem fHead;
	int fLength;
};

class CQueueIterator
{
public:
	CQueueIterator(CQueue* queue)
		: mQueue(queue)
		, fCurrentItem(queue->GetHeadItem())
	{
	}

	CQueueIterator(CQueue* queue, CQueueItem* startItem) :
			mQueue(queue)
	{
		if (startItem)
		{
			if(!startItem->IsMember(*queue))
				return;
			fCurrentItem = startItem;
		}
		else
			fCurrentItem = NULL;
	}

	~CQueueIterator(){}

	void Reset()
	{
		fCurrentItem = mQueue->GetHeadItem();
	}

	CQueueItem* GetCurrent()
	{
		return fCurrentItem;
	}

	void Next();

	bool IsDone()
	{
		return fCurrentItem == NULL;
	}

private:
	CQueue* mQueue;
	CQueueItem* fCurrentItem;
};

class CBlockingQueue
{
public:
	CBlockingQueue(){}
	~CBlockingQueue(){waitCond.signal();}
	void EnQueue(CQueueItem* obj);
	CQueueItem* DeQueue(nsecs_t waitDuration = -1); //-1 wait forever..
	void ExitQueue();
private:
	Condition waitCond;
	Mutex waitMutex;
	CQueue mQueue;
};

void CQueueItem::Remove()
{
	if (mQueue != NULL)
		mQueue->Remove(this);
}

} /* namespace vmodule */
#endif /* QUEUE_H_ */
