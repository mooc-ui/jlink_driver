#pragma once

/*
	简单串口协议:
	特性: 固定长度数据,数据中不能出现 起始符 

*/

#include "ComPort.h"
//起始字符
#define START_SYMBOL		('$')
//发送缓冲最大值
#define SEND_BUFFER_SIZE	(1024)
//帧字节间隔超时 MS
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
		UINT timeout_in_ms,		//接收超时
		const BYTE* data_send,	//发送数据
		UINT send_len,			//发送长度
		BYTE* data_recv,		//接收缓冲
		UINT recv_len);			//接收长度
	enum { 
		EI_NULL,
		EI_SEND_ERROR,		//发送出错
		EI_CHECKSUM_ERROR,	//校验出错
		EI_RECV_TIME_OUT	//接收超时
	} error_info;
	char error_buffer[1024];
public:

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

