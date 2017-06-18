/*
 * QueueTest.cpp
 *
 *  Created on: Jun 18, 2017
 *      Author: jeffrey
 */



#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sutils/Logger.h>
#include <sutils/RefBase.h>
#include <sutils/Queue.hpp>
#include <sutils/FileUtils.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "QueueTest"

#ifdef DEBUG_ENABLE
#define MY_LOGD(format, ...)	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format, ...)	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
#else
#define MY_LOGD(format, ...)
#define MY_LOGE(format, ...)  vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
#endif
using namespace vmodule;

class Student {
public:
	int id;
};

int main(int argc, char **argv) {
	char* pCurrDir = new char[260];
	GetCurrentDirectory(260, pCurrDir);
	std::string pCurrDirString(pCurrDir);
#if defined(TARGET_WINDOWS)
	pCurrDirString.append("\\");
#else
	pCurrDirString.append("/");
#endif
	vmodule::Logger::Init(pCurrDirString);
	if (pCurrDir != NULL) {
		delete pCurrDir;
		pCurrDir = NULL;
	}
	Student t1, t2, t3, t4, t5, t6;
	t1.id = 10;
	t2.id = 11;
	t3.id = 12;
	t4.id = 13;
	t5.id = 14;
	t6.id = 15;
	CQueueItem<Student> item1(&t1);
	CQueueItem<Student> item2(&t2);
	CQueueItem<Student> item3(&t3);
	CQueueItem<Student> item4(&t4);
	CQueueItem<Student> item5(&t5);
	CQueueItem<Student> item6(&t6);
	CQueue<Student > sQueue;

	sQueue.EnQueue(&item1);
	sQueue.EnQueue(&item2);
	sQueue.EnQueue(&item3);
	sQueue.EnQueue(&item4);
	sQueue.EnQueue(&item5);
	sQueue.EnQueue(&item6);


	int length = sQueue.GetLength();
	printf("sQueue:length = %d\n", length);
	int i = 0;
	for (i = 0; i < length; i++) {
		CQueueItem<Student> *item = sQueue.DeQueue();
		if (item != NULL) {
			Student *st = item->GetObject();
			printf("st.id = %d\n", st->id);
		} else {
			printf("item == NULL");
		}
	}



	Student t7, t8, t9, t10, t11, t12;
	t7.id = 16;
	t8.id = 17;
	t9.id = 18;
	t10.id = 19;
	t11.id = 20;
	t12.id = 21;
	CQueueItem<Student> item7(&t7);
	CQueueItem<Student> item8(&t7);
	CQueueItem<Student> item9(&t9);
	CQueueItem<Student> item10(&t10);
	CQueueItem<Student> item11(&t11);
	CQueueItem<Student> item12(&t12);
	sQueue.EnQueue(&item7);
	sQueue.EnQueue(&item8);
	sQueue.EnQueue(&item9);
	sQueue.EnQueue(&item10);
	sQueue.EnQueue(&item11);
	sQueue.EnQueue(&item12);


	length = sQueue.GetLength();
	printf("sQueue:length = %d\n", length);
	for (i = 0; i < length; i++) {
		CQueueItem<Student> *item = sQueue.DeQueue();
		if (item != NULL) {
			Student *st = item->GetObject();
			printf("st.id = %d\n", st->id);
		} else {
			printf("item == NULL");
		}
	}


	Student t13, t14, t15, t16, t17, t18;
	t13.id = 16;
	t14.id = 17;
	t15.id = 18;
	t16.id = 19;
	t17.id = 20;
	t18.id = 21;
	CQueueItem<Student> item13(&t13);
	CQueueItem<Student> item14(&t14);
	CQueueItem<Student> item15(&t15);
	CQueueItem<Student> item16(&t16);
	CQueueItem<Student> item17(&t17);
	CQueueItem<Student> item18(&t18);
	CBlockingQueue<Student > sbQueue;

	sbQueue.EnQueue(&item13);
	sbQueue.EnQueue(&item14);
	sbQueue.EnQueue(&item15);
	sbQueue.EnQueue(&item16);
	sbQueue.EnQueue(&item17);
	sbQueue.EnQueue(&item18);

	length = sbQueue.GetLength();
	printf("sbQueue:length = %d\n", length);
	for (i = 0; i < length; i++) {
		CQueueItem<Student> *item = sbQueue.DeQueue();
		if (item != NULL) {
			Student *st = item->GetObject();
			printf("bst.id = %d\n", st->id);
		} else {
			printf("bitem == NULL");
		}
	}
	while(true);
	return 0;
}
