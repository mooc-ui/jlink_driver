#include "hx_utils.h"

//static void _delay(void)
//{
//	int _1ms = 1;
//    int i;
//    while (_1ms--)
//    {
////        WDTFeed();        //喂狗
//        for(i=3000; i>0; i--);
//    }
//}

//=====================================================
/*
	this defination is use for send uart data
	please achive it to call true uart send data proc
*/
#include "uart.h"
#include "irq.h"
extern void WriteCom(tk_int16 iPortNum,tk_uint8 *ucpTrData,tk_int16 iTrLen);
//void UART_TX_BYTE(int id, int c)			
//{											
////	char __tmp__c__ = c;							
//	//
//	if(id==0){
////		WriteCom(id, (tk_uint8 *)&__tmp__c__, 1);	
////		hx_delay_ms(10);
////		return;
//		ucUART0_TR_BUF[0] = c;
//		iUART0_TR_LEN = 1;
//	}else if(id==1){
//		ucUART1_TR_BUF[0] = c;
//		iUART1_TR_LEN = 1;
//	}else if(id==2){
//		ucUART2_TR_BUF[0] = c;
//		iUART2_TR_LEN = 1;
//	}else if(id==3){
//		ucUART3_TR_BUF[0] = c;
//		iUART3_TR_LEN = 1;
//		
//	}
//	UART_SendTrBuf(id);
//	//_delay();
//}

/*
	this proc is use for disable uart interrupt
	please achive it to call true uart int proc
*/
void UARTX_DIS_INTR(int id)	
{ 
	if(id==0) 
		VICIntEnClr = 1 << UART0_INT;//NVIC_DisableIRQ(UART0_IRQn); 
	else if(id==1) 
		VICIntEnClr = 1 << UART1_INT;//NVIC_DisableIRQ(UART1_IRQn); 
	else if(id==2) 
		VICIntEnClr = 1 << UART2_INT;//NVIC_DisableIRQ(UART2_IRQn); 
	else if(id==3) 
		VICIntEnClr = 1 << UART3_INT;//NVIC_DisableIRQ(UART3_IRQn); 
}

void UARTX_EN_INTR(int id)	
{ 
	if(id==0) 
		VICIntEnable = 1 << UART0_INT;//NVIC_EnableIRQ(UART0_IRQn); 
	else if(id==1) 
		VICIntEnable = 1 << UART1_INT;//NVIC_EnableIRQ(UART1_IRQn); 
	else if(id==2) 
		VICIntEnable = 1 << UART2_INT;//NVIC_EnableIRQ(UART2_IRQn); 
	else if(id==3) 
		VICIntEnable = 1 << UART3_INT;//NVIC_EnableIRQ(UART3_IRQn); 
}

/*
	must enable tx interrupt.
*/
void UART_TX_START_BYTE(int id,int data)
{
	if(id==0) {
		//U0IER = IER_THRE | IER_RLS;
		U0THR = data;
	}else if(id==1) {
		//U1IER = IER_THRE | IER_RLS;
		U1THR = data;
	}else if(id==2) {
		//U2IER = IER_THRE | IER_RLS;
		U2THR = data;
	}else if(id==3) {
		
		IOSET1|=1<<29;//允许发送
		
		//U3IER = IER_THRE | IER_RLS;
		U3THR = data;
	}

}	
void UART_TX_END_SESSION(int id)
{
	if(id==3){
		//U3IER = IER_RBR | IER_THRE | IER_RLS;
		IOCLR1|=1<<29;//允许接受
	}
}

int HX_UART_REOPEN(int id, int bps)
{
	if(id==3){
		IODIR1|=(1<<29);//设为输出
		IOCLR1|=1<<29;//允许接收
	}
	UART_Init(id,bps);
	EnableUart(id);
	return 0;
}


//================================================================================
//cmds
#include "includes.h"
extern char g_psam_nr[6];
extern int g_rm_port;
extern int g_rm_ip[4];
const DEV_PARAM_T g_param_tbl[] = {
	{"dev_sn",			"bcd,4",	Bus_Info_St.dev_sn},
	{"bus_no",			"bcd,4",	Bus_Info_St.BusNo},
	{"psam_no",			"bcd,6",	g_psam_nr},
	{"ip",				"i32,4,.",	g_rm_ip},
	{"port",			"i32",		&g_rm_port},
};

const int g_param_count = sizeof(g_param_tbl)/sizeof(g_param_tbl[0]);
extern void HX_PARAM_LOADSTORE(int load0store1)
{
	
}

