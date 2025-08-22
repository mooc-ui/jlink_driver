#ifndef __COMPORT_H__
#define __COMPORT_H__

class CComPort
{														 
public:
	CComPort();
	virtual		~CComPort();
public:										
	BOOL Init(									//��ʼ���˿�
		UINT portnr = 1, 
		UINT baud = 115200, 
		BYTE parity = 'N', 
		UINT databits = 8, 
		UINT stopsbits = 1,
		UINT nBufferSize = 4096,
		DWORD dwCommEvents = EV_RXCHAR | EV_CTS
		);
	void CComPort::Flush(void);
	BOOL Send(const BYTE *s);									//������0��β���ַ���
	BOOL Send(const BYTE *s,int size);							//���Ͷ������ֽ�����
	static BOOL Send(CComPort* port,const BYTE *s,int size);	//
	static BOOL RevByte(CComPort* port,BYTE* pRev );
	BOOL RevByte(BYTE* pRev);									//����һ���ֽ�
	BOOL RevByte(BYTE* pRev,int timeout);						//����ʱ�Ľ����ֽ�
	BOOL RevArray(BYTE* revBuffer,int n,int* nrev,int timeout);	//�����ֽ�����
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
	COMMTIMEOUTS		m_CommTimeouts;		//��ʱ����
	DCB					m_dcb;				//
	// misc
	UINT				m_nPortNr;			//�˿�
	BYTE*				m_szWriteBuffer;	//����
	DWORD				m_nWriteBufferSize;	//�����С

};

#endif __COMPORT_H__


