#include "stdafx.h"
#include "UniversalSerialProtocol.h"


CUniversalSerialProtocol::CUniversalSerialProtocol()
{
}


CUniversalSerialProtocol::~CUniversalSerialProtocol()
{
}

BOOL CUniversalSerialProtocol::Open(UINT port, UINT baudrate)
{
	return TRUE;
}
void CUniversalSerialProtocol::Close(void)
{

}
BOOL CUniversalSerialProtocol::DoDialog(
	UINT timeout_in_ms,		//���ճ�ʱ
	BOOL retry_times,		//���Դ���
	const BYTE* data_send,	//��������
	UINT send_len,			//���ͳ���
	BYTE* data_recv,		//���ջ���
	UINT recv_len)			//���ճ���
{
	return TRUE;
}
const char* CUniversalSerialProtocol::GetErrorInfo(void)
{
	return NULL;
}