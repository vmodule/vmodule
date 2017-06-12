/*
 * FileUtils.cpp
 *
 *  Created on: 2017Äê5ÔÂ22ÈÕ
 *      Author: sucx
 */

#include <sutils/FileUtils.h>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif
namespace vmodule {

#if !defined(TARGET_WINDOWS)	
int CFileUtils::GetCurrentDirectory(int size, char *name) {

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
}
#endif

} /* namespace vmodule */



#if !defined(TARGET_WINDOWS)	
int GetCurrentDirectory(int size, char *name)
{
	return vmodule::CFileUtils::GetCurrentDirectory(size, name);
}
#endif



