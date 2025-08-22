// Packet.cpp : 实现文件
//

#include "stdafx.h"
#include "Packet.h"

// CPacket

CPacket::CPacket()
: m_Data()
, m_Size(0)
{
}

CPacket::~CPacket()
{

}
// CPacket 成员函数
CPacket::CPacket(PACKAGE_TYPE t)
{
	m_Size = 4;
	m_Data.SetAtGrow(0,0xAA);
	m_Data.SetAtGrow(1,0x55);
	m_Data.SetAtGrow(2,(BYTE)t);
	m_Data.SetAtGrow(3,(BYTE)0);
	int sum = xmemsum();
	m_Data.SetAtGrow(4,(BYTE)sum);
	m_Data.SetAtGrow(5,(BYTE)(sum>>8));
}
CPacket::CPacket(const BYTE* s,int len,PACKAGE_TYPE type)
{
	if(P_COMMAND == type){
		int l = len;
		const BYTE* str = s;
		int i=0;
		m_Size = l+4;

		m_Data.SetAtGrow(i++,0xAA);
		m_Data.SetAtGrow(i++,0x55);

		m_Data.SetAtGrow(i++,(BYTE)type);
		
		m_Data.SetAtGrow(i++,(BYTE)l);

		while(l--){
			m_Data.SetAtGrow(i++,*str++);
		}
		int sum = xmemsum();
		m_Data.SetAtGrow(i++,(BYTE)sum);
		m_Data.SetAtGrow(i++,(BYTE)(sum>>8));
	}else{
		m_Size = 4;
		m_Data.SetAtGrow(0,0xAA);
		m_Data.SetAtGrow(1,0x55);
		m_Data.SetAtGrow(2,(BYTE)type);
		m_Data.SetAtGrow(3,(BYTE)0);
		int sum = xmemsum();
		m_Data.SetAtGrow(4,(BYTE)sum);
		m_Data.SetAtGrow(5,(BYTE)(sum>>8));
	
	}
}
CPacket::CPacket(const BYTE* s,PACKAGE_TYPE type)
{
	if(P_COMMAND == type){
		int l = strlen((const char *)s);
		const BYTE* str = s;
		int i=0;
		m_Size = l+4;

		m_Data.SetAtGrow(i++,0xAA);
		m_Data.SetAtGrow(i++,0x55);

		m_Data.SetAtGrow(i++,(BYTE)type);
		
		m_Data.SetAtGrow(i++,(BYTE)l);

		while(l--){
			m_Data.SetAtGrow(i++,*str++);
		}
		int sum = xmemsum();
		m_Data.SetAtGrow(i++,(BYTE)sum);
		m_Data.SetAtGrow(i++,(BYTE)(sum>>8));
	}else{
		m_Size = 4;
		m_Data.SetAtGrow(0,0xAA);
		m_Data.SetAtGrow(1,0x55);
		m_Data.SetAtGrow(2,(BYTE)type);
		m_Data.SetAtGrow(3,(BYTE)0);
		int sum = xmemsum();
		m_Data.SetAtGrow(4,(BYTE)sum);
		m_Data.SetAtGrow(5,(BYTE)(sum>>8));
	
	}
}
CPacket::CPacket(CString s,PACKAGE_TYPE type)
{
	if(P_COMMAND == type){
		int l=s.GetLength();
		const char *str = s.GetString();
		int i=0;
		m_Size = l+4;

		m_Data.SetAtGrow(i++,0xAA);
		m_Data.SetAtGrow(i++,0x55);

		m_Data.SetAtGrow(i++,(BYTE)type);
		
		m_Data.SetAtGrow(i++,(BYTE)l);

		while(l--){
			m_Data.SetAtGrow(i++,*str++);
		}
		int sum = xmemsum();
		m_Data.SetAtGrow(i++,(BYTE)sum);
		m_Data.SetAtGrow(i++,(BYTE)(sum>>8));
	}else{
		m_Size = 4;
		m_Data.SetAtGrow(0,0xAA);
		m_Data.SetAtGrow(1,0x55);
		m_Data.SetAtGrow(2,(BYTE)type);
		m_Data.SetAtGrow(3,(BYTE)0);
		int sum = xmemsum();
		m_Data.SetAtGrow(4,(BYTE)sum);
		m_Data.SetAtGrow(5,(BYTE)(sum>>8));
	
	}

}
CByteArray &CPacket::GetPacket(void)
{
	return m_Data;
}

int CPacket::GetSize()
{
	return m_Size+2;
}

int CPacket::xmemsum(void)
{
	int l = m_Size;
	int res=0;
	int i=0;
	while(l--){
		res += m_Data.GetAt(i++);
	}
	return res;
}
int CPacket::xmemsum(BYTE *buffer,UINT l)
{
	int res=0;
	int i=0;
	while(l--){
		res += (unsigned int)buffer[i++];
	}
	return res;
}
bool CPacket::SendTo(CComPort* port)						/*发送*/
{
	if(port->Send(m_Data.GetData(),m_Data.GetSize()))
		return true;
	return false;
}
bool CPacket::ReciveFrom(										/*接收*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		UINT time_out
		)
{
	BYTE* rbuffer=s_res;
	BYTE c;
	if(!port->RevByte(&c,time_out)) return false;	/* 检查同步头 */
	if(c != 0xAA)							
		return false;
	if(!port->RevByte(&c,time_out)) return false;
	if(c != 0x55)
		return false;
	if(!port->RevByte(&c,time_out)) return false;	/* 获取包类型 */
	*type_res = (PACKAGE_TYPE)c;

	if(!port->RevByte(&c,time_out)) return false;
	int len = c;							/* 获得包长度 */
	int l=len;
	while(len--){							/* 存储包数据 */
		if(!port->RevByte(&c,time_out)) return false;	
		*rbuffer++ = c;		
	}
	*rbuffer = '\0';						/* 结尾写零 */	

	UINT sum = 0x00000000;
	if(!port->RevByte(&c,time_out)) return false;
	sum = c;					/* 接收校验和 */
	if(!port->RevByte(&c,time_out)) return false;
	sum	+= ((UINT)c)<<8 ; 
	if((xmemsum(s_res,l)+
		(unsigned int)0xAA+
		(unsigned int)0x55+
		*type_res+
		l
		) != sum){
		return false;	
	}		
	return true;
}



bool CPacket::ReciveFrom(										/*接收*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		int *s_len,
		UINT time_out
		)
{
	bool res = __ReciveFrom(port,type_res,s_res,s_len,time_out);
	//m_nRevTimeout = DEFAULT_RECIVE_TIMOUT_MS;
	if(!res){
		//reset port
		port->Close();
		port->Init(port->m_nPortNr);
		res = __ReciveFrom(port,type_res,s_res,s_len,time_out);
		if(!res)
			port->Close();
	}
	return res;
}
bool CPacket::__ReciveFrom(										/*接收*/
		CComPort* port,
		PACKAGE_TYPE* type_res,
		BYTE *s_res,
		int *s_len,
		UINT time_out
		)
{
	BYTE* rbuffer=s_res;
	BYTE c;
	if(!port->RevByte(&c,time_out)) return false;	/* 检查同步头 */
	if(c != 0xAA)							
		return false;
	if(!port->RevByte(&c,time_out)) return false;
	if(c != 0x55)
		return false;
	if(!port->RevByte(&c,time_out)) return false;	/* 获取包类型 */
	*type_res = (PACKAGE_TYPE)c;

	if(!port->RevByte(&c,time_out)) return false;
	int len = c;							/* 获得包长度 */
	int l=len;
	*s_len = l;
	while(len--){							/* 存储包数据 */
		if(!port->RevByte(&c,time_out)) return false;	
		*rbuffer++ = c;		
	}
	*rbuffer = '\0';						/* 结尾写零 */	

	UINT sum = 0x00000000;
	if(!port->RevByte(&c,time_out)) return false;
	sum = c;					/* 接收校验和 */
	if(!port->RevByte(&c,time_out)) return false;
	sum	+= ((UINT)c)<<8 ; 
	if((xmemsum(s_res,l)+
		(unsigned int)0xAA+
		(unsigned int)0x55+
		*type_res+
		l
		) != sum){
		return false;	
	}		
	return true;
}

