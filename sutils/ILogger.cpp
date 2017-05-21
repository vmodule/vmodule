/*
 * ILogger.cpp
 *
 *  Created on: May 21, 2017
 *      Author: jeffrey
 */

#include <sutils/ILogger.h>
#include <sutils/StringUtils.h>
#include <stdarg.h>

namespace vmodule {

ILogger::ILogger() {
	// TODO Auto-generated constructor stub

}

ILogger::~ILogger() {
	// TODO Auto-generated destructor stub
}

void ILogger::Log(int loglevel, const char *format, ...) {
	std::string strData;

	strData.reserve(16384);
	va_list va;
	va_start(va, format);
	strData = StringUtils::FormatV(format, va);
	va_end(va);

	log(loglevel, strData.c_str());
}

} /* namespace vmodule */
