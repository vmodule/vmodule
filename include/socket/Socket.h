/*
 * CSocket.h
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SOCKET_SOCKET_H_
#define VMODULE_SOCKET_SOCKET_H_
#include <sutils/RefBase.h>
#include <socket/SocketTask.h>
#if !defined(TARGET_WINDOWS)
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <netinet/tcp.h>
#include <netinet/in.h>
#endif
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
namespace vmodule {

class CSocket: virtual public RefBase {
public:
	CSocket();
	virtual ~CSocket();
	int Close(int fd);
	void sError(const char *s);
	int Bind(int fd, const struct sockaddr *sa, socklen_t salen);
	int Connect(int fd, const struct sockaddr *sa, socklen_t salen);
	int Socket(int family, int type, int protocol);
	int Listen(int fd, int backlog);
	ssize_t Read(int fd, void *ptr, size_t nbytes);
	ssize_t Write(int fd, const void *ptr, size_t nbytes);
	ssize_t Readn(int fd, void *vptr, size_t n);
	ssize_t Writen(int fd, const void *vptr, size_t n);
	int NewUnixSocket(const char *path,
			int access_mask,bool blocking = false);
	int NewTcpSocket(int port,bool blocking = false) = 0;
	int NewUdpSocket(int port,bool blocking = false) = 0;
	//Using class CSocketTask desciption one socket port..
private:
	static CSocketTask *mListenList;
};

} /* namespace vmodule */
#endif /* CSOCKET_H_ */
