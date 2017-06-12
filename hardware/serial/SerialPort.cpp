/*
 * SerialPort.cpp
 *
 *  Created on: 2017��5��23��
 *      Author: Jeffrey
 */

#include <hardware/serial/SerialPort.h>
#include <sutils/Logger.h>
#include <assert.h>
#include <errno.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "CSerialPort"

#ifdef DEBUG_ENABLE
#define MY_LOGD(format,...) 	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format,...) 	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
#else
#define MY_LOGD(format, ...)  ((void)0)
#define MY_LOGE(format, ...)  ((void)0)
#endif

namespace vmodule {

CSerialPort::CSerialPort() :
		m_file(INVALID_HANDLE_VALUE), m_ReadTask(0), m_WriteTask(0) {
	// TODO Auto-generated constructor stub
#if defined(TARGET_WINDOWS)
	read_EventMask = 0;
#endif
}

CSerialPort::~CSerialPort() {
	// TODO Auto-generated destructor stub
	if (m_ReadTask != 0) {
		m_ReadTask->requestExit();
		m_ReadTask->join();
		m_ReadTask.clear();
		m_ReadTask = 0;
	}
	ClosePort();
}

void CSerialPort::onFirstRef() {
}

status_t CSerialPort::InitPort(const char* name, int baud,
		char parity /*= 'N'*/, int databits /*= 8*/, int stopsbits /*= 1*/,
		unsigned long dwCommEvents /*= EV_RXCHAR*/) {

	if (m_file != INVALID_HANDLE_VALUE) {
		MY_LOGE("device has init ..");
		return INVALID_OPERATION;
	}
	status_t result = openPort(name);
	if (result != OK) {
		MY_LOGE("Open Serial Error");
		return INVALID_OPERATION;
	}
	Mutex::Autolock _l(m_critical);
#if defined(TARGET_WINDOWS)
	ZeroMemory(&read_Overlapped,sizeof(read_Overlapped));
	if (read_Overlapped.hEvent != NULL) {
		ResetEvent(read_Overlapped.hEvent);
		read_Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}
	COMMTIMEOUTS CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 0;
	result = SetCommTimeouts(m_file, &CommTimeouts);
	if (result) {
		if (SetCommMask(m_file, dwCommEvents)) {
			DCB dcb;
			FillMemory(&dcb, sizeof(dcb), 0);
			if (!GetCommState(m_file, &dcb))
				return INVALID_OPERATION;
			else {
				char szDCBparam[50];
				sprintf(szDCBparam, "baud=%d parity=%c data=%d stop=%d", baud,
						parity, databits, stopsbits);
				result = GetCommState(m_file, &dcb)
						&& BuildCommDCB(szDCBparam, &dcb);
				if (result)
					result = SetCommState(m_file, &dcb);
				//dcb.fRtsControl = RTS_CONTROL_ENABLE;
			}
		}
	}
	PurgeComm(m_file,
	PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

#else
	struct termios tio;
	memset(&tio, 0, sizeof(tio));
	if (tcgetattr(m_file, &tio) != 0) {
		memset(&tio, 0, sizeof(tio));
		MY_LOGE("get termios Error");
	}
	//设置波特率
	cfsetispeed(&tio, baud); 
	//修改控制模式，保证程序不会占用串口
	tio.c_cflag |= CLOCAL;
	//修改控制模式，使得能够从串口中读取输入数据
	tio.c_cflag |= CREAD;
	
	//tio.c_cflag |= baud
#if 0
	//设置数据流控制
	switch(flow_ctrl)
	{

		case 0 : //不使用流控制
		tio.c_cflag &= ~CRTSCTS;
		break;

		case 1 ://使用硬件流控制
		tio.c_cflag |= CRTSCTS;
		break;
		case 2 ://使用软件流控制
		tio.c_cflag |= IXON | IXOFF | IXANY;
		break;
	}
#endif
	tio.c_cflag &= ~CRTSCTS;//default we are nor using hardware control..
	tio.c_cflag |= IXON | IXOFF | IXANY;
	//设置数据位
	tio.c_cflag &= ~CSIZE;//屏蔽其他标志位
	switch (databits)
	{
		case 5 :
			tio.c_cflag |= CS5;
		break;
		case 6 :
			tio.c_cflag |= CS6;
		break;
		case 7 :
			tio.c_cflag |= CS7;
		break;
		case 8:
			tio.c_cflag |= CS8;
		break;
		default:
		MY_LOGE("Unsupported data size/n");
	}
	//设置校验位
	switch (parity)
	{
		case 'n':
		case 'N': //无奇偶校验位。
			tio.c_cflag &= ~PARENB;
			tio.c_iflag &= ~INPCK;
		break;
		case 'o':
		case 'O'://设置为奇校验
			tio.c_cflag |= (PARODD | PARENB);
			tio.c_iflag |= INPCK;
		break;
		case 'e':
		case 'E'://设置为偶校验
			tio.c_cflag |= PARENB;
			tio.c_cflag &= ~PARODD;
			tio.c_iflag |= INPCK;
		break;
		case 's':
		case 'S'://设置为空格
			tio.c_cflag &= ~PARENB;
			tio.c_cflag &= ~CSTOPB;
		break;
		default:
			MY_LOGE("Unsupported parity/n");
			return INVALID_OPERATION;
	}

    // 设置停止位
    switch (stopsbits)
    {
       case 1:
    	   tio.c_cflag &= ~CSTOPB;
       break;
       case 2:
    	   tio.c_cflag |= CSTOPB;
       break;
       default:
    	   MY_LOGE("Unsupported stop bits/n");
    	   return INVALID_OPERATION;
    }

	//修改输出模式，原始数据输出
    tio.c_oflag &= ~OPOST;
	tio.c_iflag = IGNPAR;
	tio.c_lflag = 0; /* turn of CANON, ECHO*, etc */
	//设置等待时间和最小接收字符
    tio.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */
	tio.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */

	//如果发生数据溢出，接收数据，但是不再读取
	tcflush(m_file,TCIFLUSH);

	//激活配置 (将修改后的termios数据设置到串口中）
	if (tcsetattr(m_file,TCSANOW,&tio) != 0) {
		MY_LOGE("com set error!/n");
		return INVALID_OPERATION;
	}

	FD_ZERO(&readset);
	if(m_file >= 0)
	FD_SET(m_file, &readset);
	max_fd = m_file + 1;
	time_out.tv_sec = 10;
	time_out.tv_usec = 500000;
#endif
	if (m_ReadTask == 0) {
		m_ReadTask = new CSerialPortReadTask(this);
		m_ReadTask->run("C2-SerialPort");
	}
	return OK;
}

void CSerialPort::ClosePort() {

	if (m_file != INVALID_HANDLE_VALUE) {
#if defined(TARGET_WINDOWS)
		CloseHandle(m_file);
#else
		close(m_file);
#endif
		m_file = INVALID_HANDLE_VALUE;
	}
}

status_t CSerialPort::openPort(const char* name) {
	assert(name != NULL);
	Mutex::Autolock _l(m_critical);
#if defined(TARGET_WINDOWS)
	m_file = CreateFile(name,
	GENERIC_READ | GENERIC_WRITE, 0,
	NULL,
	OPEN_EXISTING, /*FILE_FLAG_OVERLAPPED*/0, 0);
	if (m_file == INVALID_HANDLE_VALUE) {
		MY_LOGE("open Serial client Error..");
		return INVALID_OPERATION;
	}
	return OK;
#else
	MY_LOGD("device name:%s",name);
	m_file = open(name,O_RDWR|O_NOCTTY|O_NDELAY);
	if(m_file <= -1) {
		MY_LOGE("open Serial client Error..");
		return INVALID_OPERATION;
	}
	int flag = fcntl(m_file, F_GETFL, 0);
	int err = fcntl(m_file, F_SETFL, flag | O_NONBLOCK);
	return OK;
#endif
}

status_t CSerialPort::ReadByte(u_int8_t &byte) {
	int bResult = -1;
	if (m_file == INVALID_HANDLE_VALUE)
		return INVALID_OPERATION;
	Mutex::Autolock _l(m_critical);
#if defined(TARGET_WINDOWS)
	DWORD BytesRead = 0;
	bResult = ReadFile(m_file, &byte, 1, &BytesRead, NULL);
	if ((!bResult)) {
		//DWORD dwError = GetLastError();
		PurgeComm(m_file, PURGE_RXCLEAR | PURGE_RXABORT);
		MY_LOGE("read serail Error");
		return INVALID_OPERATION;
	}
#else
	bResult = read(m_file, &byte, 1);
	if (bResult != 1) {
		MY_LOGE("read serail Error");
		return INVALID_OPERATION;
	}
#endif
	return OK;
}

status_t CSerialPort::WriteData(unsigned char* pData, unsigned int length) {
	int bResult = -1;
	if (m_file == INVALID_HANDLE_VALUE)
		return INVALID_OPERATION;
	Mutex::Autolock _l(m_critical);
#if defined(TARGET_WINDOWS)
	DWORD BytesWrite = 0;
	bResult = WriteFile(m_file, pData, length, &BytesWrite, NULL);
	if (!bResult) {
		//DWORD dwError = GetLastError();
		PurgeComm(m_file, PURGE_RXCLEAR | PURGE_RXABORT);
		return INVALID_OPERATION;
	}
#else
	bResult = write(m_file, pData, length);
	if(bResult < 0) {
		MY_LOGE("write serail Error");
		return INVALID_OPERATION;
	}
#endif
	return OK;
}

#if defined(TARGET_WINDOWS)
int CSerialPort::GetTotalBytes() {
	DWORD dwError = 0;
	COMSTAT comstat;
	memset(&comstat, 0, sizeof(COMSTAT));
	UINT BytesInQue = 0;
	if (ClearCommError(m_file, &dwError, &comstat)) {
		BytesInQue = comstat.cbInQue;
	}
	return BytesInQue;
}
#endif

CSerialPortReadTask::CSerialPortReadTask(wp<CSerialPort> client) :
		CThread(), mClient(client) {
	MY_LOGD("%s", __FUNCTION__);
	memset(buffer, 0, 1024);
	len = 0;
}

CSerialPortReadTask::~CSerialPortReadTask() {
	MY_LOGD("%s: Exit", __FUNCTION__);
}

#if defined(TARGET_WINDOWS)
bool CSerialPortReadTask::threadLoop() {
	sp<CSerialPort> client = mClient.promote();
	if (client == 0)
		return false;
	//MY_LOGD("start %s:", __func__);
	if (WaitCommEvent(client->m_file, &client->read_EventMask,
			&client->read_Overlapped)) {
		if (client->read_EventMask & EV_RXCHAR) {
			UINT BytesInQue = client->GetTotalBytes();
			if (BytesInQue == 0)
				return true;
			len = 0;
			memset(buffer, 0, 1024);
			do {
				if (client->ReadByte(buffer[len]) == OK) {
					MY_LOGD("buffer[%d]=0x%02x", len,buffer[len] & 0xff);
					len++;
				}
			} while (--BytesInQue);
			len++;
		}
	} else {
		DWORD dwRet = GetLastError();
		if (ERROR_IO_PENDING == dwRet) {
			MY_LOGD("I/O is pending...");
			// To do.
		} else {
			MY_LOGD("Wait failed with error %d", GetLastError());
		}

	}
	return true;
}
#else
bool CSerialPortReadTask::threadLoop() {
	sp<CSerialPort> client = mClient.promote();
	if (client == 0 || client->m_file < 0) {
		MY_LOGE("mClient Error");
		return false;
	}
	status_t result;
	result = select(client->m_file,
			&client->readset, 0, 0, /*&client->time_out*/NULL);
	if(result == -1 && errno==EINTR) {
		MY_LOGE("select Error(%d)",EINTR);
		return true;
	} else if (result == 0) {
		MY_LOGE("select Error TimeOut");
		return true;
	}

	if(client->m_file >=0 &&
			FD_ISSET(client->m_file, &client->readset)) {
		MY_LOGD("start read data from serial");
		len = 0;
		memset(buffer, 0, 1024);
		result = INVALID_OPERATION;
		do {
			result = client->ReadByte(buffer[len]);
			if (result == OK) {
				MY_LOGD("buffer[%d]=0x%02x",len,buffer[len] & 0xff);
				len++;
			}
		}while (result == OK);
		len++;
	}
	return true;
}
#endif

CSerialPortWriteTask::CSerialPortWriteTask(wp<CSerialPort> client) :
		CThread(), mClient(client) {
	MY_LOGD("%s", __FUNCTION__);
}

CSerialPortWriteTask::~CSerialPortWriteTask() {
	MY_LOGD("%s: Exit", __FUNCTION__);
}

bool CSerialPortWriteTask::threadLoop() {
	Mutex::Autolock _l(mMutex);
	sp<CSerialPort> client = mClient.promote();
	if (client == 0)
		return false;
	MY_LOGD("start %s:", __FUNCTION__);
	MQUEUE_ITEM *item = mBlockingQueue.DeQueue();
	return true;
}

}
/* namespace vmodule */
