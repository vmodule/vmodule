/*
 * FileUtils.cpp
 *
 *  Created on: 2017Äê5ÔÂ22ÈÕ
 *      Author: sucx
 */

#include <sutils/FileUtils.h>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#endif
namespace vmodule {

int CFileUtils::getCurrentDirectory(int size, char *name) {
#if !defined(TARGET_WINDOWS)
	int cnt = readlink("/proc/self/exe", name, size);
	if (cnt < 0 || cnt >= size) {
		printf("***getCurrentDirectory Error***\n");
		return -1;
	}
	int i;
	for (i = cnt; i >=0; --i) {
		if (name[i] == '/')
		{
			name[i+1] = '\0';
			break;
		}
	}
	return strlen(name);
#else
	return ::GetCurrentDirectory(size, name);
#endif
}

} /* namespace vmodule */
