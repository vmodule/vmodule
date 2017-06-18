/*
 * FileUtils.h
 *
 *  Created on: 2017��5��22��
 *      Author: sucx
 */

#ifndef VMODULE_SUTILS_FILEUTILS_H_
#define VMODULE_SUTILS_FILEUTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <vector>
#include <cctype>
#include <iterator>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <utility>
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <sutils/Logger.h>
#include <sutils/Errors.h>

namespace vmodule {

class CFileUtils {
public:
	static status_t DoMakeDir(const char *path, 
		uint8_t mode);
	static status_t MakeDir(const char * path, uint8_t mode);
	static status_t DeleteDir(const char * dirname);
	static bool FileExist(const char * filename);
	static int 	FileSize(const char * filename);
	static bool DirExist(const char * dirname);
	static bool IsDir(char* filename);
	static size_t WriteToFile(const char* fname, 
			uint8_t *buf, size_t size);
	static size_t ReadToBuffer(const char *fname, 
			uint8_t* buf, size_t size);
#if !defined(TARGET_WINDOWS)	
	static int GetCurrentDirectory(int size, char *name);
#endif	
};

} /* namespace vmodule */

#if !defined(TARGET_WINDOWS)	
int GetCurrentDirectory(int size, char *name);
#endif

#endif /* INCLUDE_SUTILS_FILEUTILS_H_ */
