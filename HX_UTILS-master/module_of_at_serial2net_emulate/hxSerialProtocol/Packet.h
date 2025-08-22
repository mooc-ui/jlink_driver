#pragma once
#include "stdafx.h"
#include "ComPort.h"
// CPacket ����Ŀ��




enum PACKAGE_TYPE{
	
	P_COMMAND = 1,		//�����
	P_RESULT,			//���ؽ��
	P_ERROR,			//�޷�ִ�еİ�
	
	P_NAK,				//
	
	P_DEBUG,			//������Ϣ
	
	P_TYPE_MAX,			//���Ƕ�����ĩβ,���ڼ��������Ƿ�ϸ�

};


class CPacket : public CObject
{
public:
	
	CPacket();
	CPacket(CString s,PACKAGE_TYPE type=P_COMMAND);				/*��ʼ��һ����*/
	CPacket(const BYTE *s,PACKAGE_TYPE type=P_COMMAND);
	CPacket(const BYTE *s,int len,PACKAGE_TYPE type=P_COMMAND);
	CPacket(PACKAGE_TYPE t);
	virtual ~CPacket();	

	CByteArray m_Data;
	int m_Size;		//��ȥУ�������ݳ���
public:
	CByteArray &GetPacket(void);
	int GetSize();												/**/
	bool CPacket::SendTo(CComPort* port);						/*����*/
	static bool ReciveFrom(										/*����*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		UINT time_out
		);
	static bool ReciveFrom(										/*����*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		int *s_len,
		UINT time_out
		);
	static bool __ReciveFrom(										/*����*/
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


