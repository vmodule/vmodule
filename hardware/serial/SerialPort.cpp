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

#define _DEBUG 1
#define LOG_TAG "CSerialPort"

#ifdef _DEBUG
#define MY_LOGD(fmt, arg...)  VLOGD(LOG_TAG,fmt, ##arg)
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)
#else
#define MY_LOGD(fmt, arg...)  ((void)0)
#define MY_LOGE(fmt, arg...)  ((void)0)
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
	if (tcgetattr(m_file, &tio))
	memset(&tio, 0, sizeof(tio));

	tio.c_cflag = baud | CS8 | CLOCAL | CREAD;
	// Disable output processing, including messing with end-of-line characters.
	tio.c_oflag &= ~OPOST;
	tio.c_iflag = IGNPAR;
	tio.c_lflag = 0; /* turn of CANON, ECHO*, etc */
	/* no timeout but request at least one character per read */
	tio.c_cc[VTIME] = 0;
	tio.c_cc[VMIN] = 1;
	tcsetattr(m_file, TCSANOW, &tio);
	tcflush(m_file, TCIFLUSH);

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
	m_file = open(name,O_RDWR|O_NOCTTY|O_NDELAY);
	if(m_file <= -1) {
		MY_LOGE("open Serial client Error..");
		return INVALID_OPERATION;
	}
	return OK;
#endif
}

status_t CSerialPort::ReadByte(unsigned char &byte) {
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
	if(bResult < 0) {
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
			unsigned char cRecved = 0x00;
			do {
				if (client->ReadByte(cRecved) == OK) {
					MY_LOGD("cRecved=0x%02x", cRecved & 0xff);
				}

			} while (--BytesInQue);
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
	if (client == 0 || client->m_file < 0)
		return false;
	unsigned char buffer[256];
	bool result;
	result = select(client->m_file,
			&client->readset, 0, 0, &client->time_out);
	if(result == -1 && result == errno==EINTR)
		return true;
	else if (result == 0) //time out
		return true;

	if(client->m_file >=0 &&
			FD_ISSET(client->m_file, &client->readset)) {
		result = read(client->m_file, buffer, 256);
		if(result > 0 ) {
			//todo...
		}
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
	MY_LOGD("start %s:", __func__);
	MQUEUE_ITEM *item = mBlockingQueue.DeQueue();
	return true;
}

}
/* namespace vmodule */
