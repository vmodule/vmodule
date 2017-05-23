/*
 * SerialPort.h
 *
 *  Created on: 2017��5��23��
 *      Author: sucx
 */

#ifndef VMODULE_HARDWARE_SERIALPORT_H_
#define VMODULE_HARDWARE_SERIALPORT_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#endif

#include <sutils/PlatformDefs.h>
#include <sutils/Errors.h>
#include <sutils/Mutex.h>
#include <sutils/Thread.h>
#include <sutils/Timers.h>
#include <sutils/RefBase.h>
#include <sutils/VmBlockingQueue.h>
#include <sutils/VmQueue.h>

namespace vmodule {
class CSerialPortReadTask;
class CSerialPortWriteTask;

class CSerialPort: public virtual RefBase {
	friend class CSerialPortReadTask;
	friend class CSerialPortWriteTask;
public:
	CSerialPort();
	virtual ~CSerialPort();
public:
	/**
	 *  @param:  UINT portNo
	 *  @param:  UINT baud
	 *  @param:  char parity 是否进行奇偶校验,'Y'表示需要奇偶校验,'N'表示不需要奇偶校验
	 *  @param:  UINT databits 数据位的个数,默认值为8个数据位
	 *  @param:  UINT stopsbits 停止位使用格式,默认值为1
	 *  @param:  DWORD dwCommEvents 默认为EV_RXCHAR,即只要收发任意一个字符,则产生一个事件
	 *  @return: bool
	 *  @see:
	 */
	status_t InitPort(const char *name, int baud = CBR_9600, char parity = 'N',
			int databits = 8, int stopsbits = 1, unsigned long dwCommEvents =
			EV_RXCHAR);
	/**
	 *  @param:  unsigned char * pData
	 *  @param:  unsigned int length
	 *  @return: bool
	 *  @note:   length
	 *  @see:
	 */
	status_t WriteData(unsigned char* pData, unsigned int length);
	/** read one byte from serial buffer
	 *  @param:  char & cRecved
	 *  @return: bool
	 *  @note:
	 *  @see:
	 */
	status_t ReadByte(unsigned char &byte);

#if defined(TARGET_WINDOWS)
	int GetTotalBytes();
#endif

private:
	/**
	 *  @param:  UINT portNo
	 *  @return: true success,false error
	 *  @note:
	 *  @see:
	 */
	status_t openPort(const char* name);

	/** cloase sertial port
	 *  @return:
	 *  @note:
	 *  @see:
	 */
	void ClosePort();

protected:
	virtual void onFirstRef();
private:
	HANDLE m_file;
	Mutex m_critical;
#if defined(TARGET_WINDOWS)
	OVERLAPPED read_Overlapped;
	DWORD read_EventMask;
#else
	fd_set readset;
	int max_fd;
	struct timeval time_out;
#endif
	sp<CSerialPortReadTask> m_ReadTask;
	sp<CSerialPortWriteTask> m_WriteTask;
};

class CSerialPortReadTask: public virtual CThread {
public:
	CSerialPortReadTask(wp<CSerialPort> client);
	~CSerialPortReadTask();
	static const nsecs_t kWaitDuration = 100000000; // 100 ms
protected:
	wp<CSerialPort> mClient;
	virtual bool threadLoop();
};

class CSerialPortWriteTask: public virtual CThread {
public:
	CSerialPortWriteTask(wp<CSerialPort> client);
	~CSerialPortWriteTask();
	static const nsecs_t kWaitDuration = 100000000; // 100 ms
protected:
	virtual bool threadLoop();
	wp<CSerialPort> mClient;
	Mutex mMutex; // Guards mCaptureState
	Condition mCondition;
	VmBlockingQueue mBlockingQueue;
};

} /* namespace vmodule */

#endif /* INCLUDE_HARDWARE_SERIAL_SERIALPORT_H_ */
