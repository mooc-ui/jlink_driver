// hxSerialProtocol.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "hxSerialProtocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// ChxSerialProtocolApp

BEGIN_MESSAGE_MAP(ChxSerialProtocolApp, CWinApp)
END_MESSAGE_MAP()


// ChxSerialProtocolApp ����

ChxSerialProtocolApp::ChxSerialProtocolApp()
{
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� ChxSerialProtocolApp ����

ChxSerialProtocolApp theApp;


// ChxSerialProtocolApp ��ʼ��

BOOL ChxSerialProtocolApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
