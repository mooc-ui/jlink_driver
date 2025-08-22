#include "stdafx.h"
#include "SimpleSerialProtocol.h"


CSimpleSerialProtocol::CSimpleSerialProtocol()
{
}


CSimpleSerialProtocol::~CSimpleSerialProtocol()
{
}

BOOL CSimpleSerialProtocol::Open(UINT port, UINT baudrate)
{
	error_info = EI_NULL;
	this->port = port;
	this->baudrate = baudrate;
	return com.Init(port, baudrate);
}
const char* CSimpleSerialProtocol::GetErrorInfo(void)
{
	char *p;
	switch (error_info)
	{
	case EI_NULL:
		p = "no error";
		break;
	case EI_SEND_ERROR:
		p = "send error";
		break;
	case EI_CHECKSUM_ERROR:
		p = "check_sum error";
		break;
	case EI_RECV_TIME_OUT:
		p = "recv time out";
		break;
	default:
		p = "no error";
		break;
	}
	strcpy_s(error_buffer, p);
	return error_buffer;
}
void CSimpleSerialProtocol::Close(void)
{
	com.Close();
	return ;
}
UINT check_sum(BYTE *buffer, UINT l)
{
	int res = 0;
	int i = 0;
	while (l--) {
		res += (unsigned int)buffer[i++];
	}
	return res;
}
BOOL CSimpleSerialProtocol::DoDialog(
	UINT timeout_in_ms,		//֡��ʱ
	BOOL retry_times,		//�ط�����
	const BYTE* data_send,	//��������
	UINT send_len,			//���ͳ���
	BYTE* data_recv,		//���ջ���
	UINT recv_len)			//���ճ���
{
	int retry = retry_times;
	do {
		if (_DoDialog(timeout_in_ms, data_send, send_len, data_recv, recv_len))
			return TRUE;
	} while (retry_times-- > 0);
	return FALSE;
}
BOOL CSimpleSerialProtocol::_DoDialog(
	UINT timeout_in_ms,		//���ճ�ʱ,�ֽڼ���
	const BYTE* data_send,	//��������
	UINT send_len,			//���ͳ���
	BYTE* data_recv,		//���ջ���
	UINT recv_len)			//���ճ���
{
	buffer[0] = START_SYMBOL;
	memcpy(buffer + 1, data_send, send_len);
	UINT sum = check_sum(buffer, 1 + send_len);
	buffer[send_len + 1] = sum & 0xFF;
	buffer[send_len + 2] = (sum >> 8) & 0xFF;
	if (!com.Send(buffer, 1 + send_len + 2))
	{
		error_info = EI_SEND_ERROR;
		return FALSE;
	}
	LONG start = GetTickCount();

	error_info = EI_RECV_TIME_OUT;
	BYTE c;
	while (GetTickCount() < (start + timeout_in_ms))
	{
		//�ȴ���ʼ��
		if (!com.RevByte(&c, BYTE_TIME_OUT))
			continue;
		if (c != START_SYMBOL)
			continue;
		//��⵽��ʼ��,����һ֡����
		buffer[0] = START_SYMBOL;
		int l = recv_len + 2;
		for (int i = 1; i < l; i++)
		{
			if (com.RevByte(&c, BYTE_TIME_OUT))
				buffer[i] = c;
			else
				//�����м�ʱ��̫��
				continue;
		}
		//�������,����У��
		UINT calc_sum = check_sum(buffer, recv_len + 1);
		UINT data_sum = buffer[recv_len + 2];
		data_sum <<= 8;
		data_sum += buffer[recv_len + 1];

		if (calc_sum == data_sum)
		{
			memcpy(data_recv, buffer + 1, recv_len);
			error_info = EI_NULL;
			return TRUE;
		}
		else
		{
			error_info = EI_CHECKSUM_ERROR;
			continue;
		}
		error_info = EI_RECV_TIME_OUT;
	}
	return FALSE;
}