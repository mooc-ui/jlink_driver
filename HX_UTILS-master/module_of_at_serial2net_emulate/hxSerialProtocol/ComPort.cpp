#include "stdafx.h"
#include "ComPort.h"
#include <assert.h>

#define _CRT_SECURE_NO_WARNINGS


CComPort::CComPort()
{
	m_hComm = NULL;

	// initialize overlapped structure members to zero
	m_ov.Offset = 0;
	m_ov.OffsetHigh = 0;

	// create events
	m_ov.hEvent = NULL;
	m_hWriteEvent = NULL;
	m_hShutdownEvent = NULL;

	m_szWriteBuffer = NULL;

}

CComPort::~CComPort()
{
	SetEvent(m_hShutdownEvent);
	if(m_hComm){
		::CloseHandle(m_hComm);
		m_hComm = NULL;
	}
	
	delete [] m_szWriteBuffer;
}
void CComPort::Close(void)
{
	if(m_hComm){
		::CloseHandle(m_hComm);
		m_hComm = NULL;
	}
}
BOOL CComPort::Init(	   UINT  portnr,		// portnumber (1..4)
						   UINT  baud,			// baudrate
						   BYTE  parity,		// parity 
						   UINT  databits,		// databits 
						   UINT  stopbits,		// stopbits 
						   UINT  writebuffersize,// size to the writebuffer
						   DWORD dwCommEvents)	// EV_RXCHAR, EV_CTS etc	
{
	if(portnr <= 0){
//		AfxMessageBox("请选择正确的设备!");
		return false;
	}
	// create events
	if (m_ov.hEvent != NULL)
		ResetEvent(m_ov.hEvent);
	m_ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hWriteEvent != NULL)
		ResetEvent(m_hWriteEvent);
	m_hWriteEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (m_hShutdownEvent != NULL)
		ResetEvent(m_hShutdownEvent);
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// initialize the event objects
	//m_hEventArray[0] = m_hShutdownEvent;	// highest priority
	//m_hEventArray[1] = m_ov.hEvent;
	//m_hEventArray[2] = m_hWriteEvent;

	// initialize critical section
	InitializeCriticalSection(&m_csCommunicationSync);
	
	// set buffersize for writing and save the owner
	if (m_szWriteBuffer != NULL)
		delete [] m_szWriteBuffer;
	m_szWriteBuffer = new BYTE[writebuffersize];

	m_nPortNr = portnr;

	m_nWriteBufferSize = writebuffersize;

	BOOL bResult = FALSE;
	char *szPort = new char[50];
	char *szBaud = new char[50];

	// now it critical!
	EnterCriticalSection(&m_csCommunicationSync);

	// if the port is already opened: Close it
	if (m_hComm != NULL)
	{
		CloseHandle(m_hComm);
		m_hComm = NULL;
	}
	// prepare port strings
	sprintf_s(szPort,50, "COM%d", portnr);
	sprintf_s(szBaud,50, "baud=%d parity=%c data=%d stop=%d", baud, parity, databits, stopbits);

	// get a handle to the port
	m_hComm = CreateFileA(szPort,						// communication port string (COMX)
					     GENERIC_READ | GENERIC_WRITE,	// read/write types
					     0,								// comm devices must be opened with exclusive access
					     NULL,							// no security attributes
					     OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
					     FILE_FLAG_OVERLAPPED,			// Async I/O
					     0);							// template must be 0 for comm devices

	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		// port not found
		delete [] szPort;
		delete [] szBaud;

		return FALSE;
	}

	// set the timeout values
	m_CommTimeouts.ReadIntervalTimeout = 1000;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.ReadTotalTimeoutConstant = 1000;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = 1000;
	m_CommTimeouts.WriteTotalTimeoutConstant = 1000;

	// configure
	if (!SetCommTimeouts(m_hComm, &m_CommTimeouts))	return(0);
	if (!SetCommMask(m_hComm, dwCommEvents))		return(0);
	if (!GetCommState(m_hComm, &m_dcb))				return(0);
	m_dcb.fRtsControl = RTS_CONTROL_ENABLE;		
	if (!BuildCommDCBA(szBaud, &m_dcb))				return(0);
	if (!SetCommState(m_hComm, &m_dcb))				return(0);


	delete [] szPort;
	delete [] szBaud;

	// flush the port
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// release critical section
	LeaveCriticalSection(&m_csCommunicationSync);

//	INFO_EDITMODE(_T("Initialisation for communicationport %d completed.\nUse Startmonitor to communicate.\r\n", (int)portnr));
	
	return TRUE;
}
void CComPort::Flush(void)
{
	PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
}

BOOL CComPort::Send(const BYTE *s)
{
	return Send(s,strlen((const char*)s));
}
BOOL CComPort::Send(const BYTE *s,int size)
{
	return Send(this,s,size);
}
BOOL CComPort::Send(CComPort* port,const BYTE *s,int size)
{
	BOOL bWrite = TRUE;
	BOOL bResult = TRUE;
	DWORD BytesSent = 0;
	ResetEvent( port->m_hWriteEvent);
	// Gain ownership of the critical section
	EnterCriticalSection(& port->m_csCommunicationSync);
	if (bWrite)
	{
		// Initailize variables
		port->m_ov.Offset = 0;
		port->m_ov.OffsetHigh = 0;
		// Clear buffer
		PurgeComm( port->m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
		bResult = WriteFile( port->m_hComm,							// Handle to COMM Port
							 s,					// Pointer to Message buffer in calling finction
							size,	// Length of Message to Send
							&BytesSent,								// Where to store the number of bytes sent
							& port->m_ov);							// Overlapped structure
		// deal with any error codes
		if (!bResult)  {
			DWORD dwError = GetLastError();
			switch (dwError){
				case ERROR_IO_PENDING:{
						// continue to GetOverlappedResults()
						BytesSent = 0;
						bWrite = FALSE;
						break;
				}
				default:{
						// all other error codes
						 ASSERT("WriteFile()");
						 break;
				}
			}
		} 
		else{
			LeaveCriticalSection(& port->m_csCommunicationSync);
		}
	}

	if (!bWrite)
	{
		bWrite = TRUE;
	
		bResult = GetOverlappedResult( port->m_hComm,	// Handle to COMM port 
									  & port->m_ov,		// Overlapped structure
									  &BytesSent,		// Stores number of bytes sent
									  TRUE); 			// Wait flag
		LeaveCriticalSection(& port->m_csCommunicationSync);
		// deal with the error code 
		if (!bResult)  {
			 ASSERT("GetOverlappedResults() in WriteFile()");
		}	
	} // end if (!bWrite)

	// Verify that the data size Send equals what we tried to Send
	if (BytesSent != size){
		TRACE("WARNING: WriteFile() error.. Bytes Sent: %d; Message Length: %d\n", 
			BytesSent, size);
		//ASSERT(0);
		return false;
	}

	return true;
}
BOOL CComPort::RevByte(CComPort* port,BYTE* pRev )
{
	BOOL  bRead = TRUE; 
	BOOL  bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	BYTE RXBuff;
	
	
	COMSTAT comstat;
	ClearCommError(port->m_hComm, &dwError, &comstat);
	//给端口加锁	
	EnterCriticalSection(&port->m_csCommunicationSync);
	// ClearCommError() will update the COMSTAT structure and clear any other errors.
	bResult = ClearCommError(port->m_hComm, &dwError, &comstat);
	//解锁端口
	LeaveCriticalSection(&port->m_csCommunicationSync);
	if (comstat.cbInQue == 0)	// 输入缓存为0
		return false;			
	EnterCriticalSection(&port->m_csCommunicationSync);
	if (bRead){
		bResult = ReadFile(port->m_hComm,		// Handle to COMM port 
							   &RXBuff,				// RX Buffer Pointer
							   1,					// Read one byte
							   &BytesRead,			// Stores number of bytes read
							   &port->m_ov);		// pointer to the m_ov structure
		// deal with the error code 
		if (!bResult){ 
			switch (dwError = GetLastError()) { 
				case ERROR_IO_PENDING: 	{ 
					// asynchronous i/o is still in progress 
					// Proceed on to GetOverlappedResults();
					bRead = FALSE;
					break;
				}
				default:{
					// Another error has occured.  Process this error.
					ASSERT("ReadFile()");
					break;
				} 
			}
		}
		else{
			// ReadFile() returned complete. It is not necessary to call GetOverlappedResults()
			bRead = TRUE;
		}
	}  // Close if (bRead)

	if (!bRead){
		bRead = TRUE;
		bResult = GetOverlappedResult(port->m_hComm,	// Handle to COMM port 
									  &port->m_ov,		// Overlapped structure
									  &BytesRead,		// Stores number of bytes read
									  TRUE); 			// Wait flag
		// deal with the error code 
		if (!bResult)  {
			ASSERT("GetOverlappedResults() in ReadFile()");
		}	
	} 
	LeaveCriticalSection(&port->m_csCommunicationSync);
	*pRev = RXBuff;
	return true;
}





BOOL CComPort::RevByte(BYTE* pRev)
{
	return RevByte(this,pRev);
}
BOOL CComPort::RevByte(BYTE* pRev,int timeout)
{
	ULONG t = ::GetTickCount();
	do{
		if(RevByte(this,pRev))
			return true;
		Sleep(1);
	}while(::GetTickCount()-t<(UINT)timeout);

	/*int t = timeout;
	while(t-->0){
		
		Sleep(1);
	}*/
	return false;
}
BOOL CComPort::RevArray(BYTE* revBuffer,int n,int* nrev,int timeout)
{
	BYTE *p = revBuffer;
	int nr=0;
	while(n--){
		BYTE c;
		if(RevByte(&c,timeout)){
			*p++=c;
			nr++;
		}
		else{
			*nrev = nr;
			return false;
		}
	}
	*nrev = nr;
	return true;
}