/*
 * Logger.h
 *
 *  Created on: May 20, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_LOGGER_H_
#define VMODULE_LOGGER_H_

#include <sutils/Mutex.h>
#include <sutils/SingletonHelper.h>
#include <sutils/ILogger.h>
#include <sutils/Exception.h>
#include <sutils/Thread.h>
#include <sutils/LoggerThread.h>
namespace vmodule {
class Logger {
public:
	Logger();
	~Logger();
	static void Close();
	static void Log(int loglevel, const char *format, ...);
	static void Log(int loglevel, const char *tag, const char *format, ...);
	static void LogFunction(int loglevel, const char* functionName,
			const char* format, ...);
	static bool Init(const std::string& path);
	static void PrintDebugString(const std::string& line);
	static void MemDump(char *pData, int length);
	static void SetLogLevel(int level);
	static int GetLogLevel();
	static bool IsLogLevelLogged(int loglevel);
	static void LogString(int logLevel, const std::string& logString);
	static bool FormatLogString(int logLevel, const std::string& logString);
protected:
	class LoggerGlobals {
	public:
		LoggerGlobals() :
				m_repeatCount(0), m_repeatLogLevel(0), m_logLevel(
						VMODULE_LOG_DEBUG), m_extraLogLevels(0) {
		}
		~LoggerGlobals(){};
		LoggerThread m_thread;
		int m_repeatCount;
		int m_repeatLogLevel;
		std::string m_repeatLine;
		int m_logLevel;
		int m_extraLogLevels;
		bool start;
		Mutex critSec;
	};
	class LoggerGlobals m_globalInstance; // used as static global variable
};

class ILogImpl: public ILogger {
public:
	virtual ~ILogImpl() {
	}
	inline virtual void log(int logLevel, const char* message) {
		Logger::Log(logLevel, "%s", message);
	}
};

} /* namespace vmodule */

VMODULE_GLOBAL_REF(vmodule::Logger, s_globalsLogger);
#define s_globalsLogger VMODULE_GLOBAL_USE(vmodule::Logger)

#if defined(DEBUG_ENABLE)
#define VLOGD(tag,format,...) 	vmodule::Logger::Log(VMODULE_LOG_DEBUG,tag,format,##__VA_ARGS__)
#define VLOGI(tag,format,...) 	vmodule::Logger::Log(VMODULE_LOG_INFO,tag,format,##__VA_ARGS__)
#define VLOGW(tag,format,...) 	vmodule::Logger::Log(VMODULE_LOG_WARN,tag,format,##__VA_ARGS__)
#define VLOGE(tag,format,...) 	vmodule::Logger::Log(VMODULE_LOG_ERROR,tag,format,##__VA_ARGS__)
#else
#define VLOGD(tag,format,...)
#define VLOGI(tag,format,...)
#define VLOGW(tag,format,...) 	vmodule::Logger::Log(VMODULE_LOG_WARN,tag,format,##__VA_ARGS__)
#define VLOGE(tag,format,...) 	vmodule::Logger::Log(VMODULE_LOG_ERROR,tag,format,##__VA_ARGS__)
#endif

#define CONDITION(cond)     (__builtin_expect((cond)!=0, 0))

#ifndef DEBUG_ENABLE
#define VLOGD_IF(cond,tag,format,...)   ((void)0)
#else
#define VLOGD_IF(cond,tag,format,...) \
    ( (CONDITION(cond)) \
    ? ((void)vmodule::Logger::Log(VMODULE_LOG_DEBUG,tag,format,##__VA_ARGS__) \
    : (void)0 )
#endif

#endif /* LOGGER_H_ */

