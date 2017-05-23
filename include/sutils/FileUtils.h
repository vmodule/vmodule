/*
 * FileUtils.h
 *
 *  Created on: 2017Äê5ÔÂ22ÈÕ
 *      Author: sucx
 */

#ifndef VMODULE_FILEUTILS_H_
#define VMODULE_FILEUTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
namespace vmodule {

class CFileUtils {
public:
	static int getCurrentDirectory(int size, char *name);
};

} /* namespace vmodule */

#endif /* INCLUDE_SUTILS_FILEUTILS_H_ */
