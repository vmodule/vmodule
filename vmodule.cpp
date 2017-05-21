#include <stdio.h>
#include <iostream>
#include <sutils/Logger.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CThread"

#ifdef DEBUG_ENABLE
#define MY_LOGD(fmt, arg...)  VLOGD(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#else
#define MY_LOGD(fmt, arg...)
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#endif

void vmodue_init() {
	//global_Logger.Init("/home/jeffrey/workSpace/workApp/vmodule/log/");
}

int main()
{
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	MY_LOGD("start %s\n",__func__);
	//s_globalsLoggerThread.stop();
	while(true){
		sleep(1);
		MY_LOGD("start %s\n",__func__);
	};
	return 0;
}
