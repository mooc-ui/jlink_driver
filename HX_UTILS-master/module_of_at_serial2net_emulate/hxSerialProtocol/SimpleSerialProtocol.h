#pragma once

/*
	�򵥴���Э��:
	����: �̶���������,�����в��ܳ��� ��ʼ�� 

*/

#include "ComPort.h"
//��ʼ�ַ�
#define START_SYMBOL		('$')
//���ͻ������ֵ
#define SEND_BUFFER_SIZE	(1024)
//֡�ֽڼ����ʱ MS
#define BYTE_TIME_OUT		(5)

class CSimpleSerialProtocol
{
public:
	CSimpleSerialProtocol();
	~CSimpleSerialProtocol();

private:
	UINT port;
	UINT baudrate;
	CComPort com;
	BYTE buffer[SEND_BUFFER_SIZE];
	BOOL _DoDialog(
		UINT timeout_in_ms,		//���ճ�ʱ
		const BYTE* data_send,	//��������
		UINT send_len,			//���ͳ���
		BYTE* data_recv,		//���ջ���
		UINT recv_len);			//���ճ���
	enum { 
		EI_NULL,
		EI_SEND_ERROR,		//���ͳ���
		EI_CHECKSUM_ERROR,	//У�����
		EI_RECV_TIME_OUT	//���ճ�ʱ
	} error_info;
	char error_buffer[1024];
public:

	BOOL Open(UINT port, UINT baudrate);
	void Close(void);
	BOOL DoDialog(
		UINT timeout_in_ms,		//���ճ�ʱ
		BOOL retry_times,		//���Դ���
		const BYTE* data_send,	//��������
		UINT send_len,			//���ͳ���
		BYTE* data_recv,		//���ջ���
		UINT recv_len);			//���ճ���
	const char* GetErrorInfo(void);

};

