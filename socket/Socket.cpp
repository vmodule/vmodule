/*
 * CSocket.cpp
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#include <socket/Socket.h>
#include <sutils/Logger.h>
#include <sys/stat.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CSocket"

#define MY_LOGD(format, ...)	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format, ...)	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)

namespace vmodule {

CSocketTask *CSocket::mListenList = NULL;
CSocket::CSocket() {
	// TODO Auto-generated constructor stub
}

CSocket::~CSocket() {
	// TODO Auto-generated destructor stub
	if (NULL != mListenList) {
		delete mListenList;
		mListenList = NULL;
	}
}

int CSocket::Close(int fd) {
	int n;
	if ((n = close(fd)) == -1)
		sError("close error");
	return n;
}

void CSocket::sError(const char *s) {
	MY_LOGE("%s", s);
}

int CSocket::Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	int n;

	if ((n = bind(fd, sa, salen)) < 0)
		sError("bind error");
	return n;
}

int CSocket::Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
	int n;
	n = connect(fd, sa, salen);
	if (n < 0) {
		sError("connect error");
	}

	return n;
}

int CSocket::Listen(int fd, int backlog) {
	int n;

	if ((n = listen(fd, backlog)) < 0)
		sError("listen error");

	return n;
}

int CSocket::Socket(int family, int type, int protocol) {
	int n;
	if ((n = socket(family, type, protocol)) < 0)
		sError("socket error");
	return n;
}

ssize_t CSocket::Read(int fd, void *ptr, size_t nbytes) {
	ssize_t n;

	again: if ((n = read(fd, ptr, nbytes)) == -1) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

ssize_t CSocket::Write(int fd, const void *ptr, size_t nbytes) {
	ssize_t n;

	again: if ((n = write(fd, ptr, nbytes)) == -1) {
		if (errno == EINTR)
			goto again;
		else
			return -1;
	}
	return n;
}

ssize_t CSocket::Readn(int fd, void *vptr, size_t n) {
	size_t nleft; //usigned int 剩余未读取的字节数
	ssize_t nread; //int 实际读到的字节数
	char *ptr;
	ptr = (char *) vptr;
	nleft = n; //n 未读取字节数

	while (nleft > 0) {
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if (nread == 0)
			break;

		nleft -= nread; //nleft = nleft - nread
		ptr += nread;
	}
	return n - nleft;
}

ssize_t CSocket::Writen(int fd, const void *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *) vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n;
}

int CSocket::NewUnixSocket(const char *path, int access_mask, bool blocking) {
	int sfd;
	struct linger ling = { 0, 0 };
	struct sockaddr_un addr;
	struct stat tstat;
	int old_umask;
	int flags;
	if (!path) {
		return 1;
	}
	if ((sfd = Socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		sError("socket()");
		return -1;
	}
	if (!blocking) {
		if ((flags = fcntl(sfd, F_GETFL, 0)) < 0
				|| fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
			sError("setting O_NONBLOCK");
			close(sfd);
			return -1;
		}
	}
	if (lstat(path, &tstat) == 0) {
		if (S_ISSOCK(tstat.st_mode))
			unlink(path);
	}
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *) &flags, sizeof(flags));
	setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *) &flags, sizeof(flags));
	setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *) &ling, sizeof(ling));
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
	assert(strcmp(addr.sun_path, path) == 0);
	old_umask = umask(~(access_mask & 0777));
	if (Bind(sfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		sError("bind()");
		close(sfd);
		umask(old_umask);
		return 1;
	}
	umask(old_umask);
	return sfd;
}

} /* namespace vmodule */
