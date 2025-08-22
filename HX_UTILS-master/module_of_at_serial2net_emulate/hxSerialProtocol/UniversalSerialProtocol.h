#pragma once

#include "ComPort.h"


class CUniversalSerialProtocol
{
public:
	CUniversalSerialProtocol();
	~CUniversalSerialProtocol();

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

