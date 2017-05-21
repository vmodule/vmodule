/*
 * VmQueue.cpp
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#include <sutils/VmQueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

namespace vmodule {
/*
 * Initializes a m queue.
 */
void mqueue_init(MQUEUE *mq) {
	assert(mq != NULL);
	mq->head = (mqueue_item*) malloc(sizeof(mq->head));
	mq->head->next = mq->head;
	mq->head->prev = mq->head;
	mq->fLength = 0;
}
/**
 * 将队列中的元素清除掉
 */
void mqueue_clear(MQUEUE *mq) {
	if (NULL == mq)
		return;
	if (NULL == mq->head)
		return;
	mq->head->next = mq->head;
	mq->head->prev = mq->head;
	mq->fLength = 0;
}

/**
 * 释放队列
 */
void mqueue_free(MQUEUE *mq) {
	mqueue_clear(mq);
	if (NULL == mq->head)
		return;
	free(mq->head);
	mq->head = NULL;
	mq = NULL;
}
/*
 * Adds an item to a m queue.
 * 从队列尾部插入元素
 */
void mqueue_push(MQUEUE *mq, MQUEUE_ITEM *item) {
	assert(mq != NULL);
	assert(mq->head !=NULL);
	assert(item != NULL);
	assert(item->mQueue==NULL);
	//1.让最后一个元素和item首尾相连(mq->head->prev记录了最后一个元素)
	mq->head->prev->next = item;
	item->prev = mq->head->prev;
	//2.将item的next指向head
	item->next = mq->head;
	//3.将mq->head的prev指向item
	mq->head->prev = item;
	item->mQueue = mq; //记录该元素已经加入到队列中
	mq->fLength++;
}
/**
 * 从队列首取元素
 * 元素0记录在mq->head的next域
 */
MQUEUE_ITEM *mqueue_pop(MQUEUE *mq) {
	MQUEUE_ITEM *item;
	assert(mq != NULL);
	if (mq->fLength == 0)
		return NULL;
	item = mq->head->next;
	//修改指针
	//如果一共就一个元素则item->next本身就指向mq->head
	mq->head->next = item->next;
	//如果一共就一个元素则item->next=mq->head,这里将mq->head->prev指向自己
	item->next->prev = mq->head;
	item->mQueue = NULL;
	mq->fLength--;
	return item;
}

void *getItemObject(const MQUEUE_ITEM* item) {
	assert(item != NULL);
	return item->Object;
}

void setItemObject(MQUEUE_ITEM* item, void *object) {
	assert(item != NULL);
	item->Object = object;
}

/**
 *需要判断元素是队尾还是中间还是头部
 */
void mqueue_remove(MQUEUE *mq, MQUEUE_ITEM* elem) {
	assert(elem != NULL);
	assert(elem->mQueue != NULL);
	assert(mq != NULL);
	assert(elem != mq->head);
	assert(elem->mQueue == mq);
	if (mq->fLength == 1) {
		mqueue_pop(mq);
		elem->mQueue = NULL;
		mq->fLength--;
	}
}

int mqueue_getlength(MQUEUE *mq) {
	assert(mq != NULL);
	return mq->fLength;
}

MQUEUE_ITEM *new_mqueue_item(void) {
	MQUEUE_ITEM *item = NULL;
	item = (MQUEUE_ITEM *) malloc(sizeof(MQUEUE_ITEM));
	item->Object = NULL;
	item->mQueue = NULL;
	item->next = NULL;
	item->prev = NULL;
	return item;
}

void mqueue_item_test() {
	struct student {
		int id;
	};
	MQUEUE mq;
	mqueue_init(&mq);
	struct student t1, t2, t3, t4, t5, t6;
	t1.id = 10;
	t2.id = 11;
	t3.id = 12;
	t4.id = 13;
	t5.id = 14;
	t6.id = 15;
	MQUEUE_ITEM *item1, *item2, *item3, *item4, *item5, *item6;
	item1 = new_mqueue_item();
	setItemObject(item1, &t1);

	item2 = new_mqueue_item();
	setItemObject(item2, &t2);

	item3 = new_mqueue_item();
	setItemObject(item3, &t3);

	item4 = new_mqueue_item();
	setItemObject(item4, &t4);

	item5 = new_mqueue_item();
	setItemObject(item5, &t5);

	item6 = new_mqueue_item();
	setItemObject(item6, &t6);

	mqueue_push(&mq, item1);
	mqueue_push(&mq, item2);
	mqueue_push(&mq, item3);
	mqueue_push(&mq, item4);
	mqueue_push(&mq, item5);
	mqueue_push(&mq, item6);

	int length = mqueue_getlength(&mq);
	printf("mqueue:length = %d\n", length);
	int i = 0;
	for (i = 0; i < length; i++) {
		MQUEUE_ITEM *item = mqueue_pop(&mq);
		if (item != NULL) {
			struct student *st = (struct student *) item->Object;
			printf("st.id = %d\n", st->id);
		} else {
			printf("item == NULL");
		}
	}

	struct student t7, t8, t9, t10, t11, t12;
	t7.id = 16;
	t8.id = 17;
	t9.id = 18;
	t10.id = 19;
	t11.id = 20;
	t12.id = 21;
	MQUEUE_ITEM *item7, *item8, *item9, *item10, *item11, *item12;
	item7 = new_mqueue_item();
	setItemObject(item7, &t7);

	item8 = new_mqueue_item();
	setItemObject(item8, &t8);

	item9 = new_mqueue_item();
	setItemObject(item9, &t9);

	item10 = new_mqueue_item();
	setItemObject(item10, &t10);

	item11 = new_mqueue_item();
	setItemObject(item11, &t11);

	item12 = new_mqueue_item();
	setItemObject(item12, &t12);

	mqueue_push(&mq, item7);
	mqueue_push(&mq, item8);
	mqueue_push(&mq, item9);
	mqueue_push(&mq, item10);
	mqueue_push(&mq, item11);
	mqueue_push(&mq, item12);

	length = mqueue_getlength(&mq);
	printf("mqueue:length = %d\n", length);
	for (i = 0; i < length; i++) {
		MQUEUE_ITEM *item = mqueue_pop(&mq);
		if (item != NULL) {
			struct student *st = (struct student *) item->Object;
			printf("st.id = %d\n", st->id);
		} else {
			printf("item == NULL");
		}
	}
}

} /* namespace vmodule */
