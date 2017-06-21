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

int CTcpSocket::Listen(int fd, int backlog) {
	int n;

	if ((n = listen(fd, backlog)) < 0)
		sError("listen error");

	return n;
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

} /* namespace vmodule */
