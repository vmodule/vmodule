/*
 * UdpSocket.h
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SOCKET_UDPSOCKET_H_
#define VMODULE_SOCKET_UDPSOCKET_H_
#include <socket/Socket.h>

namespace vmodule {

class CUdpSocket: public vmodule::CSocket {
public:
	CUdpSocket();
	virtual ~CUdpSocket();
	int NewUdpSocket(int port, bool blocking = false) = 0;

};

} /* namespace vmodule */
#endif /* UDPSOCKET_H_ */
