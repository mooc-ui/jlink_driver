#include "hx_utils.h"
#include "./board/dq1300/board.h"
#include "./cpu/lpc1769/cpu.h"
#include "./cpu/lpc1769/cpu.c"

/*
	Board UART achives
*/
void HX_BRD_UART_DIS_INTR(int id);
void HX_BRD_UART_EN_INTR(int id);
void HX_BRD_UART_TX_START_BYTE(int id,int data);
void HX_BRD_UART_TX_END_SESSION(int id);
int  HX_BRD_UART_REOPEN(int id, int bps);

void HX_BRD_UART_DIS_INTR(int id)	
{ 
	CPU_UARTX_DIS_INTR(id);
}

void HX_BRD_UART_EN_INTR(int id)	
{ 
	CPU_UARTX_EN_INTR(id);
}

void HX_BRD_UART_TX_START_BYTE(int id,int data)
{
	if(id==2)
		LPC_GPIO1->FIOSET |= (1<<29);	//485 允许发送
	CPU_UART_TX_START_BYTE(id,data);
}

void HX_BRD_UART_TX_END_SESSION(int id)
{
	if(id==2){
		LPC_GPIO1->FIOCLR |= (1<<29);	//485允许接受
	}
	CPU_UART_TX_END_SESSION(id);
}

int  HX_BRD_UART_REOPEN(int id, int bps)
{
	if(id==0) {
		LPC_PINCON->PINSEL0 &= ~0x000000F0;
		LPC_PINCON->PINSEL0 |= 0x00000050;  /* RxD0 is P0.3 and TxD0 is P0.2 */
		LPC_PINCON->PINMODE0 &= ~0x000000F0;
		LPC_PINCON->PINMODE0 |= 0x00000050;
	}else if(id==1) {
		LPC_PINCON->PINSEL4 &= ~0x0000000F;
		LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */
		LPC_PINCON->PINMODE4 &= ~0x0000000F;
		LPC_PINCON->PINMODE4 |= 0x00000005;
	}else if(id==2) {
		LPC_PINCON->PINSEL0 &= ~0x00F00000;
		LPC_PINCON->PINSEL0 |= 0x00500000;	/* Enable RxD1 P0.11, TxD1 P0.10 */
		LPC_PINCON->PINMODE0 &= ~0x00F00000;
		LPC_PINCON->PINMODE0 |= 0x00500000;  
		  //this is 485
		  //P1.29 is RD SEL
		LPC_PINCON->PINMODE3 |= 0x00000001<<26;
		LPC_GPIO1->FIODIR |= (1<<29);		/* P2.xx defined as Outputs */
		//default is recive low
		LPC_GPIO1->FIOCLR |= (1<<29);		
		//LPC_GPIO1->FIOSET = (1<<29);			
	}else if(id==3) {
		//	LPC_PINCON->PINSEL4 &= ~0x0000000F;
		//	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */
	}
	return CPU_UART_REOPEN(id,bps);
}



/*
	Board Params achives
*/
static int dummy = 99;
const DEV_PARAM_T g_param_tbl[] = {
	{"dummy",		"%d",	&dummy},
};
const int g_param_count = 1;
int HX_PARAM_LOADSTORE(int load0store1)
{
	return 0;
}

