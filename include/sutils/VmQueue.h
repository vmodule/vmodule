/*
 * VmQueue.h
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SUTILS_MQUEUE_H_
#define VMODULE_SUTILS_MQUEUE_H_

namespace vmodule {
/**
 * 通用双向循环链表
 * 尾进头出
 */
struct mqueue;
typedef struct mqueue_item MQUEUE_ITEM;
struct mqueue_item {
	MQUEUE_ITEM *next;
	MQUEUE_ITEM *prev;
	void* Object;
	mqueue *mQueue; //for selef..
};

typedef struct mqueue MQUEUE;
struct mqueue {
	MQUEUE_ITEM *head;
	int fLength;
};

MQUEUE_ITEM *mqueue_pop(MQUEUE *mq);
void mqueue_push(MQUEUE *mq, MQUEUE_ITEM *item);
void mqueue_init(MQUEUE *mq);
int mqueue_getlength(MQUEUE *mq);
void mqueue_free(MQUEUE *mq);
void mqueue_clear(MQUEUE *mq);
MQUEUE_ITEM *new_mqueue_item(void);
void *getItemObject(const MQUEUE_ITEM* item);
void setItemObject(MQUEUE_ITEM* item, void *object);
void mqueue_item_test();
} /* namespace vmodule */
#endif /* VMQUEUE_H_ */
