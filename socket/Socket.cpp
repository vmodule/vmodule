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

int CSocket::Socket(struct addrinfo *ai, bool bolcking) {
	int sfd;
	int flags;
	if ((sfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1) {
		return -1;
	}
	if (!bolcking) {
		if ((flags = fcntl(sfd, F_GETFL, 0)) < 0
				|| fcntl(sfd, F_SETFL, flags | O_NONBLOCK) < 0) {
			perror("setting O_NONBLOCK");
			close(sfd);
			return -1;
		}
	}
	return sfd;
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
	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
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

int CSocket::NewSocket(const char *interface, int port,
		socket_protocol transport, bool blocking) {
	int sfd;
	struct linger ling = { 0, 0 };
	struct addrinfo *ai;
	struct addrinfo *next;
	struct addrinfo hints;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	char port_buf[NI_MAXSERV];
	int error;
	int success = 0;
	int flags = 1;
	hints.ai_socktype = IS_UDP(transport) ? SOCK_DGRAM : SOCK_STREAM;
	if (port == -1) {
		port = 0;
	}
	snprintf(port_buf, sizeof(port_buf), "%d", port);
	error = getaddrinfo(interface, port_buf, &hints, &ai);
	if (error != 0) {
		if (error != EAI_SYSTEM)
			fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(error));
		else
			sError("getaddrinfo()");
		return 1;
	}

	for (next = ai; next; next = next->ai_next) {
		if ((sfd = Socket(next, blocking)) == -1) {
			/* getaddrinfo can return "junk" addresses,
			 * we make sure at least one works before erroring.
			 */
			if (errno == EMFILE) {
				/* ...unless we're out of fds */
				sError("server_socket");
				return -1;
			}
			continue;
		}
#ifdef IPV6_V6ONLY
		if (next->ai_family == AF_INET6) {
			error = setsockopt(sfd, IPPROTO_IPV6, IPV6_V6ONLY, (char *) &flags,
					sizeof(flags));
			if (error != 0) {
				sError("setsockopt");
				close(sfd);
				continue;
			}
		}
#endif
		setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (void *) &flags,
				sizeof(flags));
		if (IS_TCP(transport)) {
			error = setsockopt(sfd, SOL_SOCKET, SO_KEEPALIVE, (void *) &flags,
					sizeof(flags));
			if (error != 0)
				sError("setsockopt");

			error = setsockopt(sfd, SOL_SOCKET, SO_LINGER, (void *) &ling,
					sizeof(ling));
			if (error != 0)
				sError("setsockopt");

			error = setsockopt(sfd, IPPROTO_TCP, TCP_NODELAY, (void *) &flags,
					sizeof(flags));
			if (error != 0)
				sError("setsockopt");
		}
		if ((next->ai_addr->sa_family == AF_INET
				|| next->ai_addr->sa_family == AF_INET6)) {
			union {
				struct sockaddr_in in;
				struct sockaddr_in6 in6;
			} my_sockaddr;
			socklen_t len = sizeof(my_sockaddr);
			if (getsockname(sfd, 
				(struct sockaddr*) &my_sockaddr, &len) == 0) {
				if (next->ai_addr->sa_family == AF_INET) {
					MY_LOGD("%s INET: %u\n",
							IS_UDP(transport) ? "UDP" : "TCP",
							ntohs(my_sockaddr.in.sin_port));
				} else {
					MY_LOGD("%s INET6: %u\n",
							IS_UDP(transport) ? "UDP" : "TCP",
							ntohs(my_sockaddr.in6.sin6_port));
				}
			}
		}
	}
	freeaddrinfo(ai);
	return sfd;
}

} /* namespace vmodule */
