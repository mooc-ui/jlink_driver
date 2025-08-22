#include "hx_utils.h"
#include "./board/vd7400.h"
#include "LPC23XX.h"
#include "./cpu/lpc2368/uart.h"
#include "./cpu/lpc2368/uart.c"
#include "./cpu/lpc2368/irq.h"
//#include "./cpu/lpc2368/timer.h"
#include "./cpu/lpc2368/timer.c"
/*
	this proc is use for disable uart interrupt
	please achive it to call true uart int proc
*/
void UARTX_DIS_INTR(int id)	
{ 
	IDISABLE;
	
//	if(id==0) 
//		VICIntEnClr = 1 << UART0_INT;//NVIC_DisableIRQ(UART0_IRQn); 
//	else if(id==1) 
//		VICIntEnClr = 1 << UART1_INT;//NVIC_DisableIRQ(UART1_IRQn); 
//	else if(id==2) 
//		VICIntEnClr = 1 << UART2_INT;//NVIC_DisableIRQ(UART2_IRQn); 
//	else if(id==3) 
//		VICIntEnClr = 1 << UART3_INT;//NVIC_DisableIRQ(UART3_IRQn); 
}

void UARTX_EN_INTR(int id)	
{ 
	IENABLE;
//	if(id==0) 
//		VICIntEnable = 1 << UART0_INT;//NVIC_EnableIRQ(UART0_IRQn); 
//	else if(id==1) 
//		VICIntEnable = 1 << UART1_INT;//NVIC_EnableIRQ(UART1_IRQn); 
//	else if(id==2) 
//		VICIntEnable = 1 << UART2_INT;//NVIC_EnableIRQ(UART2_IRQn); 
//	else if(id==3) 
//		VICIntEnable = 1 << UART3_INT;//NVIC_EnableIRQ(UART3_IRQn); 
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
	if(id==0){
		PCONP |= 1<<3;
        PINSEL0 |= 0x00000050;
	}else if(id==1){
		PCONP |= 1<<4;
        PINSEL4 |= 0x0000000a;       /*p2.0 p2.1 RxD1 and TxD1 */
        
	}else if(id==2){
		 PCONP |= 1<<24;
        PINSEL4 |= 0x000a0000;       /* RxD2 and TxD2 P2.8 P2.9*/
       
	}else if(id==3){
		IODIR1|=(1<<29);//设为输出
		IOCLR1|=1<<29;//允许接收
		 PCONP|= 1<<25;
        PINSEL0|=0x0000000A; /* Enable TxD3 P0.0 RxD3 P0.1 */
       
	}
	UART_Init(id,bps);
	EnableUart(id);
	return 0;
}


//================================================================================
//cmds
//#include "includes.h"
int dummy;
const DEV_PARAM_T g_param_tbl[] = {
	{"dummy",		"%d",	&dummy},
	//{"dev_sn",			"bcd 4",	Bus_Info_St.dev_sn},
	//{"bus_no",			"bcd 4",	Bus_Info_St.BusNo},
	//{"psam_no",			"bcd,6",	g_psam_nr},
	//{"ip",				"%hhu 4 .",	ParamCardSt.IP},
	//{"port",			"msb 2",	ParamCardSt.Port},
};

const int g_param_count = sizeof(g_param_tbl)/sizeof(g_param_tbl[0]);
int HX_PARAM_LOADSTORE(int load0store1)
{
	return 0;
//	int res;
//	if(load0store1 == 0){
//		return 0;
//	}else{
//		res = Writebusn2flash((BYTE *) &Bus_Info_St);
//		if (res != 0){
//			hxt_printf("ERRPR: Write Bus Info To Flash Return %d\r\n",res);
//			return -9;
//		}
//		res = update_param_to_flash();
//		if (res != 0){
//			hxt_printf("ERRPR: Write Param To Flash Return %d\r\n",res);
//			return -9;
//		}
//		return 0;
//	}
}

