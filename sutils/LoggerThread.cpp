/*
 * LoggerThread.cpp
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sutils/LoggerThread.h>
#include <sutils/Logger.h>
#include <sutils/ThreadImpl.h>
#include <sutils/Errors.h>
#include <sutils/StringUtils.h>
#include <sutils/AutoBuffer.h>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#endif
#define USE_DEBUG_VIEW 1
namespace vmodule {
#if !defined(TARGET_WINDOWS)
LoggerThread::LoggerThread() :
mRunning(false), m_ThreadId(vthread_id_t(-1)), m_file(NULL) {
	start();
}

LoggerThread::~LoggerThread() {
	// TODO Auto-generated destructor stub
	mRunning = false;
	if (m_file)
	fclose(m_file);
	m_file = NULL;
}

bool LoggerThread::OpenLogFile(const std::string &logFilename,
		const std::string &backupOldLogToFilename) {

	if (m_file)
	return false; // file was already opened

	(void) remove(backupOldLogToFilename.c_str());// if it's failed, try to continue
	(void) rename(logFilename.c_str(), backupOldLogToFilename.c_str());// if it's failed, try to continue

	m_file = (FILEWRAP*) fopen(logFilename.c_str(), "wb");
	if (!m_file)
	return false;// error, can't open log file

	static const unsigned char BOM[3] = {0xEF, 0xBB, 0xBF};
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
LoggerThread::LoggerThread() :
		mRunning(false), m_file(INVALID_HANDLE_VALUE) {
	// TODO Auto-generated constructor stub
	start();
}

LoggerThread::~LoggerThread() {
	// TODO Auto-generated destructor stub
	printf("start %s\n", __FUNCTION__);
	mRunning = false;
	if (m_file != INVALID_HANDLE_VALUE)
		CloseHandle(m_file);
}

bool LoggerThread::OpenLogFile(const std::string &logFilename,
		const std::string &backupOldLogToFilename) {
	if (m_file != INVALID_HANDLE_VALUE)
		return false; // file was already opened
	if (logFilename.empty())
		return false;
	std::wstring strLogFileW(StringUtils::win32ConvertUtf8ToW(logFilename));
	std::wstring strLogFileOldW(StringUtils::win32ConvertUtf8ToW(backupOldLogToFilename));

	if (strLogFileW.empty())
		return false;

	if (!strLogFileOldW.empty()) {
		(void) DeleteFileW(strLogFileOldW.c_str()); // if it's failed, try to continue
		(void) MoveFileW(strLogFileW.c_str(), strLogFileOldW.c_str()); // if it's failed, try to continue
	}

	m_file = CreateFileW(strLogFileW.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_file == INVALID_HANDLE_VALUE)
		return false;

	static const unsigned char BOM[3] = { 0xEF, 0xBB, 0xBF };
	DWORD written;
	(void) WriteFile(m_file, BOM, sizeof(BOM), &written, NULL); // write BOM, ignore possible errors
	(void) FlushFileBuffers(m_file);

	return true;
}

void LoggerThread::CloseLogFile() {
	if (m_file != INVALID_HANDLE_VALUE) {
		CloseHandle(m_file);
		m_file = INVALID_HANDLE_VALUE;
	}
}

bool LoggerThread::WriteStringToLog(const std::string &logString) {
	if (m_file == INVALID_HANDLE_VALUE)
		return false;

	std::string strData(logString);
	StringUtils::Replace(strData, "\n", "\r\n");
	strData += "\r\n";

	DWORD written;
	const bool ret = (WriteFile(m_file, strData.c_str(), strData.length(),
			&written, NULL) != 0) && written == strData.length();

	return ret;

}

void LoggerThread::PrintDebugString(const std::string &debugString) {
#ifndef USE_DEBUG_VIEW
	std::cout << debugString.c_str() << std::endl;
#else
	::OutputDebugStringW(L"Debug Print: ");
	int bufSize = MultiByteToWideChar(CP_UTF8, 0, debugString.c_str(),
			debugString.length(), NULL, 0);
	AutoBuffer buf(sizeof(wchar_t) * (bufSize + 1)); // '+1' for extra safety
	if (MultiByteToWideChar(CP_UTF8, 0, debugString.c_str(),
					debugString.length(), (wchar_t*) buf.get(),
					buf.size() / sizeof(wchar_t)) == bufSize)
	::OutputDebugStringW(
			std::wstring((wchar_t*) buf.get(), bufSize).c_str());
	else
	::OutputDebugStringA(debugString.c_str());
	::OutputDebugStringW(L"\n");
#endif
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
	while (pInstance->mRunning) {
		{
			MQUEUE_ITEM *item = pInstance->mBlockingQueue.DeQueue();
			if (NULL != item) {
				if (item->Object) {
					std::string loginfo((char *)item->Object);
					pInstance->PrintDebugString(loginfo);
					pInstance->WriteStringToLog(loginfo);
					free(item->Object);
					item->Object = NULL;
				}
				free(item);
				item = NULL;
			} else {
				continue;
			}
		}
	}
	pInstance->m_ThreadId = vthread_id_t(-1);
	return 0;
}

bool LoggerThread::start() {
	bool res;
	if (mRunning) {
		// thread already started
		return INVALID_OPERATION;
	}
	m_ThreadId = vthread_id_t(-1);
	mRunning = true;
	res = createRawThread(_threadLoop, this, "LoggerThread", 0, &m_ThreadId);
	if (res == false) {
		mRunning = false;
		m_ThreadId = vthread_id_t(-1);
		return UNKNOWN_ERROR;
	}
	return NO_ERROR;
}

bool LoggerThread::isRuning() const {
	return mRunning;
}

void LoggerThread::stop() {
	mRunning = false;
	mBlockingQueue.ExitQueue(); //force wakeup mBlockingQueue
}

} /* namespace vmodule */
