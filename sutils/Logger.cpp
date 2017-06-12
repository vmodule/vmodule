/*
 * Logger.cpp
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <memory>
#include <sutils/Logger.h>
#include <sutils/PlatformDefs.h>
#include <sutils/Timers.h>
#include <sutils/StringUtils.h>
#include <sutils/Thread.h>
#include <sutils/RefBase.h>
#include <sutils/ThreadImpl.h>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#endif
namespace vmodule {

static const char* const levelNames[] = { "UNKNOWN", "DEFAULT", "VERBOSE",
		"DEBUG", "INFO", "WARN", "ERROR", "FATAL", "SILENT" };

// add 1 to level number to get index of name
static const char* const logLevelNames[] = { "VMODULE_LOG_UNKNOWN" /*0*/,
		"VMODULE_LOG_DEFAULT" /*1*/, "VMODULE_LOG_VERBOSE" /*2*/,
		"VMODULE_LOG_DEBUG" /*3*/, "VMODULE_LOG_INFO" /*4*/,
		"VMODULE_LOG_WARN" /*5*/, "VMODULE_LOG_ERROR" /*6*/,
		"VMODULE_LOG_FATAL" /*7*/, "VMODULE_LOG_SILENT" /*8*/
};
LoggerImpl* Logger::m_loggerImpl = NULL;
// s_globals is used as static global with CLog global variables
#define s_globals VMODULE_GLOBAL_USE(Logger).m_globalInstance

Logger::Logger() {
	if (NULL == m_loggerImpl)
		m_loggerImpl = new LoggerImpl;
	if (NULL != m_loggerImpl) {
		Exception::SetLogger(m_loggerImpl);
		CThread::SetLogger(m_loggerImpl);
	}
}

Logger::~Logger() {
	if (NULL != m_loggerImpl) {
		CThread::SetLogger(NULL);
		Exception::SetLogger(NULL);
		delete m_loggerImpl;
		m_loggerImpl = NULL;
	}

	printf("start %s\n", __FUNCTION__);
}

void Logger::Close() {
	Mutex::Autolock waitLock(s_globals.critSec);
	s_globals.m_thread.CloseLogFile();
	s_globals.m_repeatLine.clear();
}

#if 0
void Logger::Log(int loglevel, const char *format, ...) {
	if (IsLogLevelLogged(loglevel)) {
		va_list va;
		va_start(va, format);
		LogString(loglevel, StringUtils::FormatV(format, va));
		va_end(va);
	}
}
#endif

void Logger::Log(int loglevel, const char *tag, const char *format, ...) {
	if (IsLogLevelLogged(loglevel)) {
		std::string tagInfo = "";
		if (tag && tag[0] && (strlen(tag) != 0))
			tagInfo.assign(tag).append(": ");
		else
			tagInfo = "Logger:";
		va_list va;
		va_start(va, format);
		LogString(loglevel, tagInfo + StringUtils::FormatV(format, va));
		va_end(va);
	}
}

void Logger::LogFunction(int loglevel, const char* functionName,
		const char* format, ...) {
	if (IsLogLevelLogged(loglevel)) {
		std::string fNameStr;
		if (functionName && functionName[0])
			fNameStr.assign(functionName).append(": ");
		va_list va;
		va_start(va, format);
		LogString(loglevel, fNameStr + StringUtils::FormatV(format, va));
		va_end(va);
	}
}

void Logger::LogString(int logLevel, const std::string& logString) {
	Mutex::Autolock waitLock(s_globals.critSec);
	std::string strData(logString);
	StringUtils::TrimRight(strData);
	if (!strData.empty()) {
#if 0
		if (s_globals.m_repeatLogLevel == logLevel
				&& s_globals.m_repeatLine == strData) {
			s_globals.m_repeatCount++;
			return;
		} else if (s_globals.m_repeatCount) {
			std::string strData2 = StringUtils::Format(
					"Previous line repeats %d times.", s_globals.m_repeatCount);
			WriteLogString(s_globals.m_repeatLogLevel, strData2);
			s_globals.m_repeatCount = 0;
		}
		s_globals.m_repeatLine = strData;
		s_globals.m_repeatLogLevel = logLevel;
#endif
		FormatLogString(logLevel, strData);
	}
}

bool Logger::Init(const std::string& path) {
	Mutex::Autolock waitLock(s_globals.critSec);
	return s_globals.m_thread.OpenLogFile(path + "log" + ".log",
			path + "log" + ".old.log");
}

void Logger::SetLogLevel(int level) {
	Mutex::Autolock waitLock(s_globals.critSec);
	if (level >= VMODULE_LOG_UNKNOWN && level <= VMODULE_LOG_SILENT) {
		s_globals.m_logLevel = level;
		Logger::Log(VMODULE_LOG_VERBOSE, "Log level changed to \"%s\"",
				logLevelNames[s_globals.m_logLevel + 1]);
	} else
		Logger::Log(VMODULE_LOG_ERROR, "%s: Invalid log level requested: %d",
				__FUNCTION__, level);
}

int Logger::GetLogLevel() {
	return s_globals.m_logLevel;
}

bool Logger::IsLogLevelLogged(int loglevel) {

#if defined(DEBUG_ENABLE) || defined(PROFILE)
	return true;
#else
	if (s_globals.m_logLevel >= VMODULE_LOG_DEBUG)
		return true;
	if (s_globals.m_logLevel <= VMODULE_LOG_UNKNOWN)
		return false;
	return loglevel >= VMODULE_LOG_VERBOSE;
#endif
}

void Logger::PrintDebugString(const std::string& line) {
#if defined(DEBUG_ENABLE) || defined(PROFILE)
	s_globals.m_thread.sendDebugMessage(line);
#endif
}

bool Logger::FormatLogString(int logLevel, const std::string& logString) {
	static const char* prefixFormat = "%02.2d:%02.2d:%02.2d T:%" PRIu64" %7s: ";

	std::string strData(logString);
	/* fixup newline alignment, number of spaces should equal prefix length */
	StringUtils::Replace(strData, "\n",
			"\n                                            ");
	int hour, minute, second;
	GetCurrentLocalTime(hour, minute, second);

	strData = StringUtils::Format(prefixFormat, hour, minute, second,
			(uint64_t) GetCurrentThreadId(), levelNames[logLevel]) + strData;
	PrintDebugString(strData);
	return true;
}

} /* namespace vmodule */
