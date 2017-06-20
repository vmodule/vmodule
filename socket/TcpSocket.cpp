/*
 * CTcpSocket.cpp
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#include <socket/TcpSocket.h>

namespace vmodule {

CTcpSocket::CTcpSocket() {
	// TODO Auto-generated constructor stub

}

CTcpSocket::~CTcpSocket() {
	// TODO Auto-generated destructor stub
}

int CTcpSocket::Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
	int n;
	again: if ((n = accept(fd, sa, salenptr)) < 0) {
		if ((errno == ECONNABORTED) || (errno == EINTR))
			goto again;
		else
			sError("accept error");
	}
	return n;
}

int CTcpSocket::NewTcpSocket(int port, bool blocking) {
	int sfd;
	int flags;

	if ((sfd = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) == -1) {
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

	return sfd;
}

} /* namespace vmodule */
