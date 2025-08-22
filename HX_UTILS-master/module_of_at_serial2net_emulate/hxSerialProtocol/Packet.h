#pragma once
#include "stdafx.h"
#include "ComPort.h"
// CPacket 命令目标




enum PACKAGE_TYPE{
	
	P_COMMAND = 1,		//命令包
	P_RESULT,			//返回结果
	P_ERROR,			//无法执行的包
	
	P_NAK,				//
	
	P_DEBUG,			//调试信息
	
	P_TYPE_MAX,			//总是定义在末尾,用于检测包类型是否合格

};


class CPacket : public CObject
{
public:
	
	CPacket();
	CPacket(CString s,PACKAGE_TYPE type=P_COMMAND);				/*初始化一个包*/
	CPacket(const BYTE *s,PACKAGE_TYPE type=P_COMMAND);
	CPacket(const BYTE *s,int len,PACKAGE_TYPE type=P_COMMAND);
	CPacket(PACKAGE_TYPE t);
	virtual ~CPacket();	

	CByteArray m_Data;
	int m_Size;		//除去校验后的数据长度
public:
	CByteArray &GetPacket(void);
	int GetSize();												/**/
	bool CPacket::SendTo(CComPort* port);						/*发送*/
	static bool ReciveFrom(										/*接收*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		UINT time_out
		);
	static bool ReciveFrom(										/*接收*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		int *s_len,
		UINT time_out
		);
	static bool __ReciveFrom(										/*接收*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		int *s_len,
		UINT time_out
		);
private:
	
	int xmemsum(void);
	static int xmemsum(BYTE *buffer,UINT l);
public:



	
};


