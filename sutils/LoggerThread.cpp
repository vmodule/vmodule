/*
 * LoggerThread.cpp
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sutils/LoggerThread.h>
#include <sutils/Logger.h>
#include <sutils/ThreadImpl.h>

namespace vmodule {

#if !defined(TARGET_WINDOWS)
LoggerThread::LoggerThread() :
		exitLoggerThread(false), m_ThreadId(vthread_id_t(-1)), m_file(NULL) {
	createThread();
}

LoggerThread::~LoggerThread() {
	// TODO Auto-generated destructor stub
	exitLoggerThread = true;
	if (m_file)
		fclose(m_file);
	m_file = NULL;
}

bool LoggerThread::OpenLogFile(const std::string &logFilename,
		const std::string &backupOldLogToFilename) {

	if (m_file)
		return false; // file was already opened

	(void) remove(backupOldLogToFilename.c_str()); // if it's failed, try to continue
	(void) rename(logFilename.c_str(), backupOldLogToFilename.c_str()); // if it's failed, try to continue

	m_file = (FILEWRAP*) fopen(logFilename.c_str(), "wb");
	if (!m_file)
		return false; // error, can't open log file

	static const unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };
	(void) fwrite(BOM, sizeof(BOM), 1, m_file); // write BOM, ignore possible errors

	return true;
}

void LoggerThread::CloseLogFile() {
	if (m_file) {
		fclose(m_file);
		m_file = NULL;
	}
}

bool LoggerThread::WriteStringToLog(const std::string &logString) {
	if (!m_file)
		return false;

	const bool ret =
			(fwrite(logString.data(), logString.size(), 1, m_file) == 1)
					&& (fwrite("\n", 1, 1, m_file) == 1);
	(void) fflush(m_file);

	return ret;
}

#if defined(TARGET_ANDROID)
int LoggerThread::android_printf(int logLevel, const char *tag, const char *msg, ...)
{
	va_list argptr;
	int result=-1;

	va_start(argptr, msg);
	switch (logLevel) {
		case VMODULE_LOG_DEBUG:
		__android_log_vprint(ANDROID_LOG_DEBUG, tag, msg, argptr);
		result = 0;
		break;
		case VMODULE_LOG_INFO:
		__android_log_vprint(ANDROID_LOG_INFO, tag, msg, argptr);
		result = 0;
		break;
		case VMODULE_LOG_WARN:
		__android_log_vprint(ANDROID_LOG_WARN, tag, msg, argptr);
		result = 0;
		break;
		case VMODULE_LOG_ERROR:
		__android_log_vprint(ANDROID_LOG_ERROR, tag, msg, argptr);
		result = -1;
		break;
		default:
		__android_log_vprint(ANDROID_LOG_VERBOSE, tag, msg, argptr);
		result = 0;
	}
	va_end(argptr);
	return result;
}
#endif
void LoggerThread::PrintDebugString(const std::string &debugString) {
#if defined(TARGET_ANDROID)
	int tagIndex = debugString.find(":");
	std::string tag = debugString.substr(0,tagIndex);
	std::string logInfo = debugString.substr(tagIndex + 1);
	android_printf(Logger::GetLogLevel(),tag.c_str(),logInfo.c_str());
#elif defined(TARGET_POSIX)
	printf("%s\n",debugString.c_str());
#endif
}

#else
LoggerThread::LoggerThread() {
	// TODO Auto-generated constructor stub
	createThread();
}

LoggerThread::~LoggerThread() {
	// TODO Auto-generated destructor stub
}

bool LoggerThread::OpenLogFile(const std::string &logFilename,
		const std::string &backupOldLogToFilename) {
	return true;
}

void LoggerThread::CloseLogFile() {

}

bool LoggerThread::WriteStringToLog(const std::string &logString) {
	return false;
}

void LoggerThread::PrintDebugString(const std::string &debugString) {
	printf("%s\n",line.c_str());
}

#endif

void LoggerThread::sendDebugMessage(const std::string& logString) {
	MQUEUE_ITEM *item = new_mqueue_item(); //you must free it after. print
	int len = strlen(logString.c_str()) + 1;
	item->Object = malloc(len);
	memcpy(item->Object, logString.c_str(), len);
	mBlockingQueue.EnQueue(item);
}

int LoggerThread::_threadLoop(void* user) {
	LoggerThread* pInstance = static_cast<LoggerThread*>(user);
	if (pInstance == NULL)
		return -1;
	while (!pInstance->exitLoggerThread) {
		MQUEUE_ITEM *item = pInstance->mBlockingQueue.DeQueue();
		if (NULL != item) {
			if (item->Object) {
				pInstance->PrintDebugString((char *) item->Object);
				pInstance->WriteStringToLog((char *) item->Object);
				free(item->Object);
				item->Object = NULL;
			}
			free(item);
			item = NULL;
		}
	}
	return 0;
}

bool LoggerThread::createThread() {
	bool res;
	res = createRawThread(_threadLoop, this, "LoggerThread", 0, &m_ThreadId);
	if (res == false) {
		m_ThreadId = vthread_id_t(-1);
		return false;
	}
	return true;
}

} /* namespace vmodule */
