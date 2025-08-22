#include "stdafx.h"
#include "hxSerialProtocol.h"
#include "SimpleSerialProtocol.h"
#include "UniversalSerialProtocol.h"

#define DLL_EXPORT	extern "C" __declspec(dllexport)

DLL_EXPORT void CloseSerial(UINT port);
DLL_EXPORT void CloseComPort(UINT port);

static CComPort*						com_tab[100] = { NULL };
static CSimpleSerialProtocol*		simple_proto_tab[100] = { NULL };
static CUniversalSerialProtocol*	universal_proto_tab[100] = { NULL };

DLL_EXPORT BOOL OpenComPort(UINT port, UINT baudrate)
{
	CloseComPort(port);
	CComPort *com = new CComPort();
	if (com->Init(port, baudrate)) {
		com->Flush();
		com_tab[port] = com;
		return TRUE;
	}
	else
	{
		com_tab[port] = FALSE;
		com->Close();
		delete com;
		return FALSE;
	}
}
DLL_EXPORT void CloseComPort(UINT port)
{
	if (com_tab[port])
	{
		com_tab[port]->Close();
		com_tab[port] = NULL;
	}
}
DLL_EXPORT BOOL Send(UINT port,const BYTE* data,UINT len)
{
	if (com_tab[port])
	{
		return com_tab[port]->Send(data, len);
	}
	else
	{
		return FALSE;
	}
}
DLL_EXPORT INT Recive(UINT port, BYTE* data, UINT len,UINT timeout)
{
	if (com_tab[port])
	{
		int nrev=0;
		com_tab[port]->RevArray(data, len, &nrev, timeout);
		return nrev;
	}
	else
	{
		return -1;
	}
}
//�򿪶˿�
DLL_EXPORT BOOL OpenSerial(UINT port, UINT baudrate, BOOL use_uniproto)
{
	CloseSerial(port);

	if (use_uniproto)
	{
		CUniversalSerialProtocol *proto = new CUniversalSerialProtocol();
		if (proto->Open(port, baudrate))
		{
			universal_proto_tab[port] = proto;
			return TRUE;
		}
		else
		{
			universal_proto_tab[port] = NULL;
			proto->Close();
			delete proto;
			return FALSE;
		}
	}
	else
	{
		CSimpleSerialProtocol *proto = new CSimpleSerialProtocol();
		if (proto->Open(port, baudrate))
		{
			simple_proto_tab[port] = proto;
			return TRUE;
		}
		else
		{
			simple_proto_tab[port] = NULL;
			proto->Close();
			delete proto;
			return FALSE;
		}
	}
	return FALSE;
}
//�رն˿�
DLL_EXPORT void CloseSerial(UINT port)
{
	if (universal_proto_tab[port])
	{
		universal_proto_tab[port]->Close();
		universal_proto_tab[port] = NULL;
	}
	if (simple_proto_tab[port])
	{
		simple_proto_tab[port]->Close();
		simple_proto_tab[port] = NULL;
	}
}
//����һ�λỰ
DLL_EXPORT BOOL DoDialog(
	UINT port,				//�˿ں�
	UINT timeout_in_ms,		//���ճ�ʱ
	BOOL use_uniproto,		//ʹ��Э��
	BOOL retry_times,		//���Դ���
	const BYTE* data_send,	//��������
	UINT send_len,			//���ͳ���
	BYTE* data_recv,		//���ջ���
	UINT recv_len)			//���ճ���
{
	if (use_uniproto)
	{
		if (universal_proto_tab[port]) {
			CUniversalSerialProtocol &proto = *universal_proto_tab[port];
			return proto.DoDialog(
				timeout_in_ms,
				retry_times,
				data_send,
				send_len,
				data_recv,
				recv_len);
		}
	}
	else
	{
		if (simple_proto_tab[port]) {
			CSimpleSerialProtocol &proto = *simple_proto_tab[port];
			return proto.DoDialog(
				timeout_in_ms,
				retry_times,
				data_send,
				send_len,
				data_recv,
				recv_len);
		}
	}
	return FALSE;
}
const char * error_info_0 = "port not opend";
DLL_EXPORT const char* getErrorInfo(UINT port)
{
	if (universal_proto_tab[port])
	{
		return universal_proto_tab[port]->GetErrorInfo();
	}
	else if (simple_proto_tab[port])
	{
		return simple_proto_tab[port]->GetErrorInfo();
	}
	else
		return error_info_0;
}

