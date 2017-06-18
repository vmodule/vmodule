/*
 * CSocket.cpp
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#include <socket/Socket.h>
#include <sutils/Logger.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CSocket"

#define MY_LOGD(format, ...)	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format, ...)	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)

namespace vmodule {

CSocket::CSocket() {
	// TODO Auto-generated constructor stub

}

CSocket::~CSocket() {
	// TODO Auto-generated destructor stub
}

int CSocket::Close(int fd) {
	int n;
	if ((n = close(fd)) == -1)
		ErrorLog("close error");
	return n;
}

void CSocket::ErrorLog(const char *s) {
	MY_LOGE("%s",s);
}

int CSocket::Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
	int n;

	again: if ((n = accept(fd, sa, salenptr)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			ErrorLog("accept error");
	}
	return n;
}

int CSocket::Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
	int n;

	if ((n = bind(fd, sa, salen)) < 0)
		ErrorLog("bind error");

	return n;
}

int CSocket::Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
	int n;
	n = connect(fd, sa, salen);
	if (n < 0) {
		ErrorLog("connect error");
	}

	return n;
}

int CSocket::Listen(int fd, int backlog) {
	int n;

	if ((n = listen(fd, backlog)) < 0)
		ErrorLog("listen error");

	return n;
}

int CSocket::Socket(int family, int type, int protocol) {
	int n;

	if ((n = socket(family, type, protocol)) < 0)
		ErrorLog("socket error");

	return n;
}

void CSocket::ReuseAddr(int fd) {
	int one = 1;
	int err = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &one,
			sizeof(int));
	assert(err == 0);
}

void CSocket::NoDelay(int fd) {
	int one = 1;
	int err = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &one,
			sizeof(int));
	assert(err == 0);
}

void CSocket::KeepAlive(int fd) {
	int one = 1;
	int err = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*) &one,
			sizeof(int));
	assert(err == 0);
}

void CSocket::NonBlocking(int fd) {

#if defined(TARGET_WINDOWS)
	u_long one = 1;
	int err = ioctlsocket(fd, FIONBIO, &one);
#else
	int flag = fcntl(fd, F_GETFL, 0);
	int err = fcntl(fd, F_SETFL, flag | O_NONBLOCK);
#endif
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
	ptr = (char *)vptr;
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

	ptr = (const char *)vptr;
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

} /* namespace vmodule */
