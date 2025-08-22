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
		UINT timeout_in_ms,		//接收超时
		BOOL retry_times,		//重试次数
		const BYTE* data_send,	//发送数据
		UINT send_len,			//发送长度
		BYTE* data_recv,		//接收缓冲
		UINT recv_len);			//接收长度
	const char* GetErrorInfo(void);
};

