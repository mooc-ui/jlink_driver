#ifndef __COMPORT_H__
#define __COMPORT_H__

class CComPort
{														 
public:
	CComPort();
	virtual		~CComPort();
public:										
	BOOL Init(									//初始化端口
		UINT portnr = 1, 
		UINT baud = 115200, 
		BYTE parity = 'N', 
		UINT databits = 8, 
		UINT stopsbits = 1,
		UINT nBufferSize = 4096,
		DWORD dwCommEvents = EV_RXCHAR | EV_CTS
		);
	void CComPort::Flush(void);
	BOOL Send(const BYTE *s);									//发送以0结尾的字符串
	BOOL Send(const BYTE *s,int size);							//发送定长的字节序列
	static BOOL Send(CComPort* port,const BYTE *s,int size);	//
	static BOOL RevByte(CComPort* port,BYTE* pRev );
	BOOL RevByte(BYTE* pRev);									//接收一个字节
	BOOL RevByte(BYTE* pRev,int timeout);						//带超时的接收字节
	BOOL RevArray(BYTE* revBuffer,int n,int* nrev,int timeout);	//接收字节序列
	void Close(void);
private:

	// synchronisation objects
	CRITICAL_SECTION	m_csCommunicationSync;
	// handles
	HANDLE				m_hShutdownEvent;
	HANDLE				m_hComm;
	HANDLE				m_hWriteEvent;
	// structures
	OVERLAPPED			m_ov;				//
	COMMTIMEOUTS		m_CommTimeouts;		//超时设置
	DCB					m_dcb;				//
	// misc
	UINT				m_nPortNr;			//端口
	BYTE*				m_szWriteBuffer;	//缓存
	DWORD				m_nWriteBufferSize;	//缓存大小

};

#endif __COMPORT_H__


