/*
 * ILogger.cpp
 *
 *  Created on: May 21, 2017
 *      Author: jeffrey
 */

#include <sutils/ILogger.h>
#include <sutils/StringUtils.h>
#include <stdarg.h>

void ILogger::Log(int loglevel, const char *tag, const char *format, ...) {
	std::string strData;

	strData.reserve(16384);
	va_list va;
	va_start(va, format);
	strData = vmodule::StringUtils::FormatV(format, va);
	va_end(va);

	log(loglevel, tag, strData.c_str());
}

