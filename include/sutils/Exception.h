/*
 * Exception.h
 *
 *  Created on: May 21, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_EXCEPTION_H_
#define VMODULE_EXCEPTION_H_
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sutils/StringUtils.h>
#include <sutils/ILogger.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "Exception"

#ifdef __GNUC__
// The 'this' pointer counts as a parameter on member methods.
#define VMODULE_ATTRIB_EXCEPTION_FORMAT __attribute__((format(printf,2,3)))
#else
#define VMODULE_ATTRIB_EXCEPTION_FORMAT
#endif

#define VMODULE_COPYVARARGS(fmt) \
	va_list argList; \
	va_start(argList, fmt); \
	Set(fmt, argList); \
	va_end(argList)

#define VMODULE_STANDARD_EXCEPTION(E) \
  class E : public vmodule::Exception \
  { \
  public: \
    inline E(const char* message,...) \
    VMODULE_ATTRIB_EXCEPTION_FORMAT : Exception(#E) { VMODULE_COPYVARARGS(message); } \
    \
    inline E(const E& other) : Exception(other) {} \
  }

namespace vmodule {

class Exception {
private:

	std::string classname;
	std::string message;

protected:
	static ILogger* logger;

	inline Exception(const char* classname_) :
			classname(classname_) {
	}

	inline Exception(const char* classname_, const char* message_) :
			classname(classname_), message(message_) {
	}

	inline Exception(const Exception& other) :
			classname(other.classname), message(other.message) {
	}

	/**
	 * This method is called from the constructor of subclasses. It
	 * will set the message from varargs as well as call log message
	 */
	inline void Set(const char* fmt, va_list& argList) {
		message = StringUtils::FormatV(fmt, argList);
	}

	/**
	 * This message can be called from the constructor of subclasses.
	 * It will set the message and log the throwing.
	 */
	inline void SetMessage(const char* fmt, ...) VMODULE_ATTRIB_EXCEPTION_FORMAT
	{
		// calls 'set'
		VMODULE_COPYVARARGS(fmt);
	}

	inline void setClassname(const char* cn) {
		classname = cn;
	}

public:
	virtual ~Exception();

	inline virtual void LogThrowMessage(const char* prefix = NULL) const {
		if (logger)
			logger->Log(VMODULE_LOG_ERROR, LOG_TAG,
					"EXCEPTION Thrown (%s) : %s", classname.c_str(),
					message.c_str());
	}

	inline virtual const char* GetMessage() const {
		return message.c_str();
	}

	inline static void SetLogger(ILogger* exceptionLogger) {
		logger = exceptionLogger;
	}
};
} /* namespace vmodule */

VMODULE_STANDARD_EXCEPTION(UncheckedException);
#define  VMODULE_HANDLE_UNCHECKED \
  catch (const UncheckedException& ) { throw; } \
  catch (const UncheckedException* ) { throw; }

#endif /* EXCEPTION_H_ */
