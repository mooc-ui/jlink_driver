
//=====================================================
/*
	this defination is use for send uart data
	please achive it to call true uart send data proc
*/
#include "chip.h"
#include "board.h"

/*
	this proc is use for disable uart interrupt
	please achive it to call true uart int proc
*/
void UARTX_DIS_INTR(int id)	
{ 
	if(id==0) 
		NVIC_DisableIRQ(UART0_IRQn); 
	else if(id==1) 
		NVIC_DisableIRQ(UART1_IRQn); 
	else if(id==2) 
		NVIC_DisableIRQ(UART2_IRQn); 
	else if(id==3) 
		NVIC_DisableIRQ(UART3_IRQn); 
}

void UARTX_EN_INTR(int id)	
{ 
	if(id==0) 
		NVIC_EnableIRQ(UART0_IRQn); 
	else if(id==1) 
		NVIC_EnableIRQ(UART1_IRQn); 
	else if(id==2) 
		NVIC_EnableIRQ(UART2_IRQn); 
	else if(id==3) 
		NVIC_EnableIRQ(UART3_IRQn); 
}

/*
	must enable tx interrupt.
*/
void UART_TX_START_BYTE(int id,int data)
{
	if(id==0) {
		//U0IER = IER_THRE | IER_RLS;
		LPC_UART0->THR = data;
	}else if(id==1) {
		//U1IER = IER_THRE | IER_RLS;
		LPC_UART1->THR = data;
	}else if(id==2) {
		LPC_GPIO1->FIOSET = (1<<29);//IOSET1|=1<<29;//允许发送
		//U2IER = IER_THRE | IER_RLS;
		LPC_UART2->THR = data;
	}else if(id==3) {
		//U3IER = IER_THRE | IER_RLS;
		LPC_UART3->THR = data;
	}

}	
void UART_TX_END_SESSION(int id)
{
	if(id==2){
		//U3IER = IER_RBR | IER_THRE | IER_RLS;
		LPC_GPIO1->FIOCLR = (1<<29);;//IOCLR1|=1<<29;//允许接受
	}
}
extern uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate );
int HX_UART_REOPEN(int id, int bps)
{
//	if(id==3){
//		LPC_GPIO1->FIOCLR = (1<<29);;//IOCLR1|=1<<29;//允许接受
//	}
	UARTInit(id,bps);
	return 0;
}


////----
///****************************************************************************
// *   $Id:: uart.c 5751 2010-11-30 23:56:11Z usb00423                        $
// *   Project: NXP LPC17xx UART example
// *
// *   Description:
// *     This file contains UART code example which include UART initialization, 
// *     UART interrupt handler, and APIs for UART access.
// *
// ****************************************************************************
// * Software that is described herein is for illustrative purposes only
// * which provides customers with programming information regarding the
// * products. This software is supplied "AS IS" without any warranties.
// * NXP Semiconductors assumes no responsibility or liability for the
// * use of the software, conveys no license or title under any patent,
// * copyright, or mask work right to the product. NXP Semiconductors
// * reserves the right to make changes in the software without
// * notification. NXP Semiconductors also make no representation or
// * warranty that such application will be suitable for the specified
// * use without further testing or modification.
//****************************************************************************/
//#include "lpc17xx.h"
//#include "type.h"
//#include "uart.h"
//#include <ctype.h>
//#include <stdio.h>
//#include <string.h>
//#include <stdarg.h>
//#include "hx_port.h"

//#define IER_RBR		0x01
//#define IER_THRE	0x02
//#define IER_RLS		0x04

//#define IIR_PEND	0x01
//#define IIR_RLS		0x03
//#define IIR_RDA		0x02
//#define IIR_CTI		0x06
//#define IIR_THRE	0x01

//#define LSR_RDR		0x01
//#define LSR_OE		0x02
//#define LSR_PE		0x04
//#define LSR_FE		0x08
//#define LSR_BI		0x10
//#define LSR_THRE	0x20
//#define LSR_TEMT	0x40
//#define LSR_RXFE	0x80

//#define BUFSIZE		0x40


//void UART0_IRQHandler (void) 
//{
//  uint8_t IIRValue, LSRValue;
//  uint8_t Dummy = Dummy;
//	
//  IIRValue = LPC_UART0->IIR;
//    
//  IIRValue >>= 1;			/* skip pending bit in IIR */
//  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
//  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
//  {
//	LSRValue = LPC_UART0->LSR;
//	/* Receive Line Status */
//	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
//	{
//	  /* There are errors or break interrupt */
//	  /* Read LSR will clear the interrupt */
//	  //UART0Status = LSRValue;
//	  Dummy = LPC_UART0->RBR;		/* Dummy read on RX to clear 
//							interrupt, then bail out */
//	  return;
//	}
//	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
//	{
//	  /* If no error on RLS, normal ready, save into the data buffer. */
//	  /* Note: read RBR will clear the interrupt */
////	  UART0Buffer[UART0Count] = LPC_UART0->RBR;
////	  UART0Count++;
////	  if ( UART0Count == BUFSIZE )
////	  {
////		UART0Count = 0;		/* buffer overflow */
////	  }	
//		UART_RX_BYTE(0,LPC_UART0->RBR);
//	}
//  }
//  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
//  {
//	/* Receive Data Available */
////	UART0Buffer[UART0Count] = LPC_UART0->RBR;
////	UART0Count++;
////	if ( UART0Count == BUFSIZE )
////	{
////	  UART0Count = 0;		/* buffer overflow */
////	}
//	  UART_RX_BYTE(0,LPC_UART0->RBR);
//  }
//  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
//  {
//	/* uint8_tacter Time-out indicator */
//	//UART0Status |= 0x100;		/* Bit 9 as the CTI error */
//  }
//  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
//  {
//	/* THRE interrupt */
//	LSRValue = LPC_UART0->LSR;		/* Check status in the LSR to see if
//									valid data in U0THR or not */
//	if ( LSRValue & LSR_THRE )
//	{
//		UART_TX_BYTE(0);
//	  //UART0TxEmpty = 1;
//	}
//	else
//	{
//	  //UART0TxEmpty = 0;
//	}
//  }
//    
//}

///*****************************************************************************
//** Function name:		UART1_IRQHandler
//**
//** Descriptions:		UART1 interrupt handler
//**
//** parameters:			None
//** Returned value:		None
//** 
//*****************************************************************************/
//void UART1_IRQHandler (void) 
//{
//  uint8_t IIRValue, LSRValue;
//  uint8_t Dummy = Dummy;
//	
//  IIRValue = LPC_UART1->IIR;
//    
//  IIRValue >>= 1;			/* skip pending bit in IIR */
//  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
//  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
//  {
//	LSRValue = LPC_UART1->LSR;
//	/* Receive Line Status */
//	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
//	{
//	  /* There are errors or break interrupt */
//	  /* Read LSR will clear the interrupt */
//	  //UART1Status = LSRValue;
//	  Dummy = LPC_UART1->RBR;		/* Dummy read on RX to clear 
//								interrupt, then bail out */
//	  return;
//	}
//	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
//	{
//	  /* If no error on RLS, normal ready, save into the data buffer. */
//	  /* Note: read RBR will clear the interrupt */
////	  UART1Buffer[UART1Count] = LPC_UART1->RBR;
////	  UART1Count++;
////	  if ( UART1Count == BUFSIZE )
////	  {
////		UART1Count = 0;		/* buffer overflow */
////	  }	
//		UART_RX_BYTE(1,LPC_UART1->RBR);
//	}
//  }
//  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
//  {
////	/* Receive Data Available */
////	UART1Buffer[UART1Count] = LPC_UART1->RBR;
////	UART1Count++;
////	if ( UART1Count == BUFSIZE )
////	{
////	  UART1Count = 0;		/* buffer overflow */
////	}
//	  UART_RX_BYTE(1,LPC_UART1->RBR);
//  }
//  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
//  {
//	/* uint8_tacter Time-out indicator */
//	//UART1Status |= 0x100;		/* Bit 9 as the CTI error */
//  }
//  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
//  {
//	/* THRE interrupt */
//	LSRValue = LPC_UART1->LSR;		/* Check status in the LSR to see if
//								valid data in U0THR or not */
//	if ( LSRValue & LSR_THRE )
//	{
//		UART_TX_BYTE(1);
//	  //UART1TxEmpty = 1;
//	}
//	else
//	{
//	  //UART1TxEmpty = 0;
//	}
//  }

//}

///*****************************************************************************
//** Function name:		UART2_IRQHandler
//**
//** Descriptions:		UART2 interrupt handler
//**
//** parameters:			None
//** Returned value:		None
//** 
//*****************************************************************************/
//void UART2_IRQHandler (void) 
//{
//  uint8_t IIRValue, LSRValue;
//  uint8_t Dummy = Dummy;
//	
//  IIRValue = LPC_UART2->IIR;
//    
//  IIRValue >>= 1;			/* skip pending bit in IIR */
//  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
//  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
//  {
//	LSRValue = LPC_UART2->LSR;
//	/* Receive Line Status */
//	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
//	{
//	  /* There are errors or break interrupt */
//	  /* Read LSR will clear the interrupt */
//	  //UART2Status = LSRValue;
//	  Dummy = LPC_UART2->RBR;		/* Dummy read on RX to clear 
//								interrupt, then bail out */
//	  return;
//	}
//	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
//	{
//	  /* If no error on RLS, normal ready, save into the data buffer. */
//	  /* Note: read RBR will clear the interrupt */
////	  UART1Buffer[UART1Count] = LPC_UART1->RBR;
////	  UART1Count++;
////	  if ( UART1Count == BUFSIZE )
////	  {
////		UART1Count = 0;		/* buffer overflow */
////	  }	
//		UART_RX_BYTE(2,LPC_UART2->RBR);
//	}
//  }
//  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
//  {
//	/* Receive Data Available */
////	UART1Buffer[UART1Count] = LPC_UART1->RBR;
////	UART1Count++;
////	if ( UART1Count == BUFSIZE )
////	{
////	  UART1Count = 0;		/* buffer overflow */
////	}
//	  UART_RX_BYTE(2,LPC_UART2->RBR);
//  }
//  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
//  {
//	/* uint8_tacter Time-out indicator */
//	//UART2Status |= 0x100;		/* Bit 9 as the CTI error */
//  }
//  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
//  {
//	/* THRE interrupt */
//	LSRValue = LPC_UART2->LSR;		/* Check status in the LSR to see if
//								valid data in U0THR or not */
//	if ( LSRValue & LSR_THRE )
//	{
//		UART_TX_BYTE(2);
//	  //UART2TxEmpty = 1;
//	}
//	else
//	{
//	  //UART2TxEmpty = 0;
//	}
//  }

//}

///*****************************************************************************
//** Function name:		UART3_IRQHandler
//**
//** Descriptions:		UART3 interrupt handler
//**
//** parameters:			None
//** Returned value:		None
//** 
//*****************************************************************************/
//void UART3_IRQHandler (void) 
//{
//  uint8_t IIRValue, LSRValue;
//  uint8_t Dummy = Dummy;
//	
//  IIRValue = LPC_UART3->IIR;
//    
//  IIRValue >>= 1;			/* skip pending bit in IIR */
//  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
//  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
//  {
//	LSRValue = LPC_UART3->LSR;
//	/* Receive Line Status */
//	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
//	{
//	  /* There are errors or break interrupt */
//	  /* Read LSR will clear the interrupt */
//	  //UART3Status = LSRValue;
//	  Dummy = LPC_UART3->RBR;		/* Dummy read on RX to clear 
//								interrupt, then bail out */
//	  return;
//	}
//	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
//	{
//	  /* If no error on RLS, normal ready, save into the data buffer. */
//	  /* Note: read RBR will clear the interrupt */
////	  UART1Buffer[UART1Count] = LPC_UART1->RBR;
////	  UART1Count++;
////	  if ( UART1Count == BUFSIZE )
////	  {
////		UART1Count = 0;		/* buffer overflow */
////	  }	
//		UART_RX_BYTE(3,LPC_UART3->RBR);
//	}
//  }
//  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
//  {
//	/* Receive Data Available */
////	UART1Buffer[UART1Count] = LPC_UART1->RBR;
////	UART1Count++;
////	if ( UART1Count == BUFSIZE )
////	{
////	  UART1Count = 0;		/* buffer overflow */
////	}
//	  UART_RX_BYTE(3,LPC_UART3->RBR);
//  }
//  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
//  {
//	/* uint8_tacter Time-out indicator */
//	//UART3Status |= 0x100;		/* Bit 9 as the CTI error */
//  }
//  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
//  {
//	/* THRE interrupt */
//	LSRValue = LPC_UART3->LSR;		/* Check status in the LSR to see if
//								valid data in U0THR or not */
//	if ( LSRValue & LSR_THRE )
//	{
//		UART_TX_BYTE(3);
//	  //UART3TxEmpty = 1;
//	}
//	else
//	{
//	  //UART3TxEmpty = 0;
//	}
//  }

//}
///*****************************************************************************
//** Function name:		UARTInit
//**
//** Descriptions:		Initialize UART port, setup pin select,
//**						clock, parity, stop bits, FIFO, etc.
//**
//** parameters:			portNum(0 or 1) and UART baudrate
//** Returned value:		true or false, return false only if the 
//**						interrupt handler can't be installed to the 
//**						VIC table
//** 
//*****************************************************************************/
//uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate )
//{
//  uint32_t Fdiv;
//  uint32_t pclkdiv, pclk;

//  if ( PortNum == 0 )
//  {
//	LPC_PINCON->PINSEL0 &= ~0x000000F0;
//	LPC_PINCON->PINSEL0 |= 0x00000050;  /* RxD0 is P0.3 and TxD0 is P0.2 */
//	LPC_PINCON->PINMODE0 &= ~0x000000F0;
//	LPC_PINCON->PINMODE0 |= 0x00000050;
//	/* By default, the PCLKSELx value is zero, thus, the PCLK for
//	all the peripherals is 1/4 of the SystemFrequency. */
//	/* Bit 6~7 is for UART0 */
//	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
//	switch ( pclkdiv )
//	{
//	  case 0x00:
//	  default:
//		pclk = SystemFrequency/4;
//		break;
//	  case 0x01:
//		pclk = SystemFrequency;
//		break; 
//	  case 0x02:
//		pclk = SystemFrequency/2;
//		break; 
//	  case 0x03:
//		pclk = SystemFrequency/8;
//		break;
//	}

//    LPC_UART0->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
//	Fdiv = ( pclk / 16 ) / baudrate + 1;	/*baud rate */
//    LPC_UART0->DLM = Fdiv / 256;							
//    LPC_UART0->DLL = Fdiv % 256;
//	LPC_UART0->LCR = 0x03;		/* DLAB = 0 */
//    LPC_UART0->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

//   	NVIC_EnableIRQ(UART0_IRQn);

//    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
//    return (TRUE);
//  }
//  else if ( PortNum == 1 )
//  {
//	LPC_PINCON->PINSEL4 &= ~0x0000000F;
//	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */
//	LPC_PINCON->PINMODE4 &= ~0x0000000F;
//	LPC_PINCON->PINMODE4 |= 0x00000005;
//	/* By default, the PCLKSELx value is zero, thus, the PCLK for
//	all the peripherals is 1/4 of the SystemFrequency. */
//	/* Bit 8,9 are for UART1 */
//	pclkdiv = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
//	switch ( pclkdiv )
//	{
//	  case 0x00:
//	  default:
//		pclk = SystemFrequency/4;
//		break;
//	  case 0x01:
//		pclk = SystemFrequency;
//		break; 
//	  case 0x02:
//		pclk = SystemFrequency/2;
//		break; 
//	  case 0x03:
//		pclk = SystemFrequency/8;
//		break;
//	}

//    LPC_UART1->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
//	Fdiv = ( pclk / 16 ) / baudrate  + 1;	/*baud rate */
//    LPC_UART1->DLM = Fdiv / 256;							
//    LPC_UART1->DLL = Fdiv % 256;
//	LPC_UART1->LCR = 0x03;		/* DLAB = 0 */
//    LPC_UART1->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

//   	NVIC_EnableIRQ(UART1_IRQn);

//    LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
//    return (TRUE);
//  }
//  else if ( PortNum == 2 )
//  {
//	LPC_PINCON->PINSEL0 &= ~0x00F00000;
//	LPC_PINCON->PINSEL0 |= 0x00500000;	/* Enable RxD1 P0.11, TxD1 P0.10 */
//	LPC_PINCON->PINMODE0 &= ~0x00F00000;
//	LPC_PINCON->PINMODE0 |= 0x00500000;  
//	  //this is 485
//	  //P1.29 is RD SEL
//	LPC_PINCON->PINMODE3 |= 0x00000001<<26;
//	LPC_GPIO1->FIODIR = (1<<29);		/* P2.xx defined as Outputs */
//	//default is recive low
//	LPC_GPIO1->FIOCLR = (1<<29);		
//	//LPC_GPIO1->FIOSET = (1<<29);			
//	  
//	/* By default, the PCLKSELx value is zero, thus, the PCLK for
//	all the peripherals is 1/4 of the SystemFrequency. */
//	/* Bit 16,17 are for UART2 */
//	pclkdiv = (LPC_SC->PCLKSEL1 >> 16) & 0x03;
//	switch ( pclkdiv )
//	{
//	  case 0x00:
//	  default:
//		pclk = SystemFrequency/4;
//		break;
//	  case 0x01:
//		pclk = SystemFrequency;
//		break; 
//	  case 0x02:
//		pclk = SystemFrequency/2;
//		break; 
//	  case 0x03:
//		pclk = SystemFrequency/8;
//		break;
//	}

//    LPC_UART2->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
//	Fdiv = ( pclk / 16 ) / baudrate  + 1;	/*baud rate */
//    LPC_UART2->DLM = Fdiv / 256;							
//    LPC_UART2->DLL = Fdiv % 256;
//	LPC_UART2->LCR = 0x03;		/* DLAB = 0 */
//    LPC_UART2->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

//   	NVIC_EnableIRQ(UART2_IRQn);

//    LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
//    return (TRUE);
//  }
//  else if ( PortNum == 3 )
//  {
////	LPC_PINCON->PINSEL4 &= ~0x0000000F;
////	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */
//	
//	/* By default, the PCLKSELx value is zero, thus, the PCLK for
//	all the peripherals is 1/4 of the SystemFrequency. */
//	/* Bit 8,9 are for UART1 */
//	pclkdiv = (LPC_SC->PCLKSEL1 >> 18) & 0x03;
//	switch ( pclkdiv )
//	{
//	  case 0x00:
//	  default:
//		pclk = SystemFrequency/4;
//		break;
//	  case 0x01:
//		pclk = SystemFrequency;
//		break; 
//	  case 0x02:
//		pclk = SystemFrequency/2;
//		break; 
//	  case 0x03:
//		pclk = SystemFrequency/8;
//		break;
//	}

//    LPC_UART3->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
//	Fdiv = ( pclk / 16 ) / baudrate  + 1;	/*baud rate */
//    LPC_UART3->DLM = Fdiv / 256;							
//    LPC_UART3->DLL = Fdiv % 256;
//	LPC_UART3->LCR = 0x03;		/* DLAB = 0 */
//    LPC_UART3->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

//   	NVIC_EnableIRQ(UART3_IRQn);

//    LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
//    return (TRUE);
//  }
//  return( FALSE ); 
//}




///******************************************************************************
//**                            End Of File
//******************************************************************************/


