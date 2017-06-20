/*
 * SocketTask.h
 *
 *  Created on: Jun 20, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_SOCKET_SOCKETTASK_H_
#define VMODULE_SOCKET_SOCKETTASK_H_

namespace vmodule {

class CSocketTask {
public:
	CSocketTask();
	virtual ~CSocketTask();
private:
    int    sfd;
	//Used for generating a list of SocketTask structures
	CSocketTask   *next;
};

} /* namespace vmodule */
#endif /* SOCKETTASK_H_ */
