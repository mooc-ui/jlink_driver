// hxSerialProtocol.h : hxSerialProtocol DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// ChxSerialProtocolApp
// �йش���ʵ�ֵ���Ϣ������� hxSerialProtocol.cpp
//

class ChxSerialProtocolApp : public CWinApp
{
public:
	ChxSerialProtocolApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
