/*
 * StringUtils.h
 *
 *  Created on: Jan 1, 2017
 *      Author: jeffrey
 */
#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <cctype>
#include <iterator>
#include <stdarg.h>

namespace vmodule {
class StringUtils {
public:
	static void ToUpper(std::string &str);
	static void ToUpper(std::wstring &str);
	static void ToLower(std::string &str);
	static void ToLower(std::wstring &str);
	static int Replace(std::string &str, char oldChar, char newChar);
	static int Replace(std::string &str, const std::string &oldStr,
			const std::string &newStr);
	static int Replace(std::wstring &str, const std::wstring &oldStr,
			const std::wstring &newStr);
	static std::vector<std::string> Split(const std::string& input,
			const std::string& delimiter, unsigned int iMaxStrings = 0);
	static std::vector<std::string> Split(const std::string& input,
			const char delimiter, size_t iMaxStrings = 0);
	static int FindNumber(const std::string& strInput,
			const std::string &strFind);
	static std::string Format(const char *fmt, ...);
	static std::string FormatV(const char *fmt, va_list args);
	static std::string Left(const std::string &str, size_t count);
	static std::string Mid(const std::string &str, size_t first, size_t count =
			std::string::npos);
	static std::string Right(const std::string &str, size_t count);
	static std::string& Trim(std::string &str);
	static std::string& Trim(std::string &str, const char* const chars);
	static std::string& TrimLeft(std::string &str);
	static std::string& TrimLeft(std::string &str, const char* const chars);
	static std::string& TrimRight(std::string &str);
	static std::string& TrimRight(std::string &str, const char* const chars);
	static std::string& RemoveDuplicatedSpacesAndTabs(std::string& str);
	static std::wstring StringToWString(const std::string &str);

	/*! \brief check whether a string is a natural number.
	 Matches [ \t]*[0-9]+[ \t]*
	 \param str the string to check
	 \return true if the string is a natural number, false otherwise.
	 */
	static bool IsNaturalNumber(const std::string& str);

	/*! \brief check whether a string is an integer.
	 Matches [ \t]*[\-]*[0-9]+[ \t]*
	 \param str the string to check
	 \return true if the string is an integer, false otherwise.
	 */
	static bool IsInteger(const std::string& str);

	/* The next several isasciiXX and asciiXXvalue functions are locale independent (US-ASCII only),
	 * as opposed to standard ::isXX (::isalpha, ::isdigit...) which are locale dependent.
	 * Next functions get parameter as char and don't need double cast ((int)(unsigned char) is required for standard functions). */
	inline static bool isasciidigit(char chr) // locale independent
			{
		return chr >= '0' && chr <= '9';
	}
	inline static bool isasciixdigit(char chr) // locale independent
			{
		return (chr >= '0' && chr <= '9') || (chr >= 'a' && chr <= 'f')
				|| (chr >= 'A' && chr <= 'F');
	}
	static int asciidigitvalue(char chr); // locale independent
	static int asciixdigitvalue(char chr); // locale independent
	inline static bool isasciiuppercaseletter(char chr) // locale independent
			{
		return (chr >= 'A' && chr <= 'Z');
	}
	inline static bool isasciilowercaseletter(char chr) // locale independent
			{
		return (chr >= 'a' && chr <= 'z');
	}
	inline static bool isasciialphanum(char chr) // locale independent
			{
		return isasciiuppercaseletter(chr) || isasciilowercaseletter(chr)
				|| isasciidigit(chr);
	}
};
}
