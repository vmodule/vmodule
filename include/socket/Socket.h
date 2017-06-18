/*
 * CSocket.h
 *
 *  Created on: Jun 17, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SOCKET_SOCKET_H_
#define VMODULE_SOCKET_SOCKET_H_
#include <sutils/RefBase.h>
namespace vmodule {

class CSocket: virtual public RefBase  {
public:
	CSocket();
	virtual ~CSocket();
};

} /* namespace vmodule */
#endif /* CSOCKET_H_ */
