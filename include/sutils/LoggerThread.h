/*
 * LoggerImpl.h
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_LOGGERTHREAD_H_
#define VMODULE_LOGGERTHREAD_H_
#include <stdio.h>
#include <string>
#include <sutils/VmBlockingQueue.h>
#include <sutils/VmQueue.h>
#include <sutils/ILogger.h>
#include <sutils/ThreadDefs.h>
#include <sutils/PlatformDefs.h>

#if defined(TARGET_ANDROID)
#include <android/log.h>
#endif

namespace vmodule {

class LoggerThread {
public:
	LoggerThread();
	virtual ~LoggerThread();
	bool start();
	void stop();
	bool OpenLogFile(const std::string& logFilename,
			const std::string& backupOldLogToFilename);
	void CloseLogFile(void);
	bool WriteStringToLog(const std::string& logString);
	void PrintDebugString(const std::string& debugString);
	void sendDebugMessage(const std::string& logString);
#if defined(TARGET_ANDROID)
	int android_printf(int logLevel, const char *tag, const char *msg, ...);
#endif
	bool isRuning() const;
protected:
	static int _threadLoop(void* user);
private:
	bool mRunning;
	vthread_id_t m_ThreadId;
	mutable Mutex   mLock;
	Condition mExitedCondition;
	friend class Logger;
	FILEWRAP* m_file;
	VmBlockingQueue mBlockingQueue;
};

} /* namespace vmodule */


#endif /* LOGGERIMPL_H_ */
