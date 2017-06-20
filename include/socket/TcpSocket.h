/*
 * CTcpSocket.h
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SOCKET_TCPSOCKET_H_
#define VMODULE_SOCKET_TCPSOCKET_H_
#include <socket/Socket.h>

namespace vmodule {

class CTcpSocket: public vmodule::CSocket {
public:
	CTcpSocket();
	virtual ~CTcpSocket();
	int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
	int NewTcpSocket(int port, bool blocking = false) = 0;
};

} /* namespace vmodule */
#endif /* CTCPSOCKET_H_ */
