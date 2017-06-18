/*
 * FileUtils.cpp
 *
 *  Created on: 2017Äê5ÔÂ22ÈÕ
 *      Author: sucx
 */
#include <stdarg.h>
#include <assert.h>
#include <math.h>
#include <functional>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <sutils/FileUtils.h>

#define MY_LOGD(format,...) 	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format,...) 	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)

namespace vmodule {

status_t CFileUtils::DoMakeDir(const char *path, 
	uint8_t mode) {
	struct stat st;
	if (0 != stat(path, &st)) {
		//  Directory does not exist.
		if (0 != mkdir(path, mode) 
			&& EEXIST != errno) {
			MY_LOGD("fail to do_mkdir [%s]: %d[%s]",
					path, errno, strerror(errno));
			return BAD_VALUE;
		}
	} else if (!S_ISDIR(st.st_mode)) {
		MY_LOGD("!S_ISDIR");
		return BAD_VALUE;
	}
	return OK;
}

status_t CFileUtils::MakeDir(const char *path, 
	uint8_t mode) {
	bool ret = true;
	char* copypath = strdup(path);
	char*pp = copypath;
	char*sp;
	while (ret && 0 != (sp = strchr(pp, '/'))) {
		if (sp != pp) {
			*sp = '\0';
			ret = DoMakeDir(copypath, mode);
			*sp = '/';
		}
		pp = sp + 1;
	}
	if (ret)
		ret = DoMakeDir(path, mode);
	free(copypath);
	return ret;
}

status_t CFileUtils::DeleteDir(const char * dirname) {
	char chBuf[256];
	DIR * dir = NULL;
	struct dirent *ptr;
	int ret = 0;
	dir = opendir(dirname);
	if (NULL == dir) {
		return -1;
	}
	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0 
			|| strcmp(ptr->d_name, "..") == 0) {
			continue;
		}
		snprintf(chBuf, 256, "%s/%s", dirname, ptr->d_name);
		if (IsDir(chBuf)) {
			ret = DeleteDir(chBuf);
			if (0 != ret) {
				return UNKNOWN_ERROR;
			}
		} else {
			ret = remove(chBuf);
			if (0 != ret) {
				return UNKNOWN_ERROR;
			}
		}
	}
	closedir(dir);
	ret = remove(dirname);
	if (0 != ret) {
		return UNKNOWN_ERROR;
	}
	return OK;
}

bool CFileUtils::IsDir(char* filename) {
	struct stat buf;
	int ret = stat(filename, &buf);
	if (0 == ret) {
		if (buf.st_mode & S_IFDIR) {
			return true; //folder
		} else {
			return false; //file
		}
	}
	return false;
}

bool CFileUtils::DirExist(const char * dirname){
	if (NULL == dirname)
		return false;
	if (opendir(dirname) == NULL)
		return false;
	return true;
}

bool CFileUtils::FileExist(const char * filename) {
	if (NULL == filename)
		return false;
	if (access(filename, F_OK) == 0)
		return true;
	else
		return false;
}

int CFileUtils::FileSize(const char * filename) {
	struct stat statbuf;
	int result = 0;
	result = stat(filename, &statbuf);
	if (result != 0)
		return 0;
	else
		return statbuf.st_size; //if 0 is -> dont have any config
}

size_t CFileUtils::WriteToFile(const char* fname, 
	uint8_t *buf, size_t size) {
	int nw, cnt = 0;
	size_t written = 0;

	int fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
	if (fd < 0) {
		MY_LOGE( "failed to create file [%s]: %s",
			fname, strerror(errno));
		return 0;
	}

	while (written < size) {
		nw = write(fd, buf + written, size - written);
		if (nw < 0) {
			MY_LOGE( "failed to write to file [%s]: %s",
					fname, strerror(errno));
			break;
		}
		written += nw;
		cnt++;
	}
	fdatasync(fd);
	close(fd);
	return written;
}


size_t CFileUtils::ReadToBuffer(const char *fname, 
	uint8_t* buf, size_t size) {
	int nr, cnt = 0;
	size_t readCnt = 0;

	int fd = open(fname, O_RDONLY);
	if (fd < 0) {
		MY_LOGE( "failed to create file [%s]: %s",
			fname, strerror(errno));
		return readCnt;
	}
	//
	if (size == 0) {
		size = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
	}
	//
	while (readCnt < size) {
		nr = read(fd, buf + readCnt, size - readCnt);
		if (nr < 0) {
			MY_LOGE( "failed to read from file [%s]: %s",
					fname, strerror(errno));
			break;
		}
		readCnt += nr;
		cnt++;
	}
	close(fd);
	return readCnt;
}


#if !defined(TARGET_WINDOWS)	
int CFileUtils::GetCurrentDirectory(int size, char *name) {
#if defined(TARGET_ANDROID)
	if(!DirExist("/data/vmodule"))
		MakeDir("/data/vmodule",S_IRUSR | S_IWUSR | S_IXUSR);
	strcpy(name,"/data/vmodule");
	//name[strlen(name)] = '\0'; 
#else
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
#endif
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



