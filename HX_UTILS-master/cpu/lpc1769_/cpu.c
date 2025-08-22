
//=====================================================
/*
	this defination is use for send uart data
	please achive it to call true uart send data proc
*/
//#include "chip.h"
//#include "board.h"

//cpu registers 
#include "lpc17xx.h"

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80


static void UART0_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
  IIRValue = LPC_UART0->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART0->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  //UART0Status = LSRValue;
	  Dummy = LPC_UART0->RBR;		/* Dummy read on RX to clear 
							interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
	{
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
//	  UART0Buffer[UART0Count] = LPC_UART0->RBR;
//	  UART0Count++;
//	  if ( UART0Count == BUFSIZE )
//	  {
//		UART0Count = 0;		/* buffer overflow */
//	  }	
		UART_RX_BYTE(0,LPC_UART0->RBR);
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
//	UART0Buffer[UART0Count] = LPC_UART0->RBR;
//	UART0Count++;
//	if ( UART0Count == BUFSIZE )
//	{
//	  UART0Count = 0;		/* buffer overflow */
//	}
	  UART_RX_BYTE(0,LPC_UART0->RBR);
  }
  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
  {
	/* uint8_tacter Time-out indicator */
	//UART0Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART0->LSR;		/* Check status in the LSR to see if
									valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
		UART_TX_BYTE(0);
	  //UART0TxEmpty = 1;
	}
	else
	{
	  //UART0TxEmpty = 0;
	}
  }
    
}

/*****************************************************************************
** Function name:		UART1_IRQHandler
**
** Descriptions:		UART1 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
static void UART1_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
  IIRValue = LPC_UART1->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART1->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  //UART1Status = LSRValue;
	  Dummy = LPC_UART1->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
	{
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
//	  UART1Buffer[UART1Count] = LPC_UART1->RBR;
//	  UART1Count++;
//	  if ( UART1Count == BUFSIZE )
//	  {
//		UART1Count = 0;		/* buffer overflow */
//	  }	
		UART_RX_BYTE(1,LPC_UART1->RBR);
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
//	/* Receive Data Available */
//	UART1Buffer[UART1Count] = LPC_UART1->RBR;
//	UART1Count++;
//	if ( UART1Count == BUFSIZE )
//	{
//	  UART1Count = 0;		/* buffer overflow */
//	}
	  UART_RX_BYTE(1,LPC_UART1->RBR);
  }
  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
  {
	/* uint8_tacter Time-out indicator */
	//UART1Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART1->LSR;		/* Check status in the LSR to see if
								valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
		UART_TX_BYTE(1);
	  //UART1TxEmpty = 1;
	}
	else
	{
	  //UART1TxEmpty = 0;
	}
  }

}

/*****************************************************************************
** Function name:		UART2_IRQHandler
**
** Descriptions:		UART2 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
static void UART2_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
  IIRValue = LPC_UART2->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART2->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  //UART2Status = LSRValue;
	  Dummy = LPC_UART2->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
	{
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
//	  UART1Buffer[UART1Count] = LPC_UART1->RBR;
//	  UART1Count++;
//	  if ( UART1Count == BUFSIZE )
//	  {
//		UART1Count = 0;		/* buffer overflow */
//	  }	
		UART_RX_BYTE(2,LPC_UART2->RBR);
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
//	UART1Buffer[UART1Count] = LPC_UART1->RBR;
//	UART1Count++;
//	if ( UART1Count == BUFSIZE )
//	{
//	  UART1Count = 0;		/* buffer overflow */
//	}
	  UART_RX_BYTE(2,LPC_UART2->RBR);
  }
  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
  {
	/* uint8_tacter Time-out indicator */
	//UART2Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART2->LSR;		/* Check status in the LSR to see if
								valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
		UART_TX_BYTE(2);
	  //UART2TxEmpty = 1;
	}
	else
	{
	  //UART2TxEmpty = 0;
	}
  }

}
/*****************************************************************************
** Function name:		UART3_IRQHandler
**
** Descriptions:		UART3 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
static void UART3_IRQHandler (void) 
{
  uint8_t IIRValue, LSRValue;
  uint8_t Dummy = Dummy;
	
  IIRValue = LPC_UART3->IIR;
    
  IIRValue >>= 1;			/* skip pending bit in IIR */
  IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
  if ( IIRValue == IIR_RLS )		/* Receive Line Status */
  {
	LSRValue = LPC_UART3->LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	  /* There are errors or break interrupt */
	  /* Read LSR will clear the interrupt */
	  //UART3Status = LSRValue;
	  Dummy = LPC_UART3->RBR;		/* Dummy read on RX to clear 
								interrupt, then bail out */
	  return;
	}
	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */			
	{
	  /* If no error on RLS, normal ready, save into the data buffer. */
	  /* Note: read RBR will clear the interrupt */
//	  UART1Buffer[UART1Count] = LPC_UART1->RBR;
//	  UART1Count++;
//	  if ( UART1Count == BUFSIZE )
//	  {
//		UART1Count = 0;		/* buffer overflow */
//	  }	
		UART_RX_BYTE(3,LPC_UART3->RBR);
	}
  }
  else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
  {
	/* Receive Data Available */
//	UART1Buffer[UART1Count] = LPC_UART1->RBR;
//	UART1Count++;
//	if ( UART1Count == BUFSIZE )
//	{
//	  UART1Count = 0;		/* buffer overflow */
//	}
	  UART_RX_BYTE(3,LPC_UART3->RBR);
  }
  else if ( IIRValue == IIR_CTI )	/* uint8_tacter timeout indicator */
  {
	/* uint8_tacter Time-out indicator */
	//UART3Status |= 0x100;		/* Bit 9 as the CTI error */
  }
  else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
  {
	/* THRE interrupt */
	LSRValue = LPC_UART3->LSR;		/* Check status in the LSR to see if
								valid data in U0THR or not */
	if ( LSRValue & LSR_THRE )
	{
		UART_TX_BYTE(3);
	  //UART3TxEmpty = 1;
	}
	else
	{
	  //UART3TxEmpty = 0;
	}
  }

}

/*
	this proc is use for disable uart interrupt
	please achive it to call true uart int proc
*/
static inline void CPU_UARTX_DIS_INTR(int id)	
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

static inline void CPU_UARTX_EN_INTR(int id)	
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
static inline void CPU_UART_TX_START_BYTE(int id,int data)
{
	if(id==0) {
		LPC_UART0->THR = data;
	}else if(id==1) {
		LPC_UART1->THR = data;
	}else if(id==2) {
		LPC_UART2->THR = data;
	}else if(id==3) {
		LPC_UART3->THR = data;
	}
}	
static inline void CPU_UART_TX_END_SESSION(int id)
{
	(void)id;
}
static inline int CPU_UART_REOPEN(int PortNum, int baudrate)
{
  uint32_t Fdiv;
  uint32_t pclkdiv, pclk;

  if ( PortNum == 0 )
  {
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 6~7 is for UART0 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}

    LPC_UART0->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate + 1;	/*baud rate */
    LPC_UART0->DLM = Fdiv / 256;							
    LPC_UART0->DLL = Fdiv % 256;
	LPC_UART0->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART0->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART0_IRQn);

    LPC_UART0->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
    return 0;
  }
  else if ( PortNum == 1 )
  {
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 8,9 are for UART1 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 8) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}

    LPC_UART1->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate  + 1;	/*baud rate */
    LPC_UART1->DLM = Fdiv / 256;							
    LPC_UART1->DLL = Fdiv % 256;
	LPC_UART1->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART1->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART1_IRQn);

    LPC_UART1->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
    return 0;
  }
  else if ( PortNum == 2 )
  {
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 16,17 are for UART2 */
	pclkdiv = (LPC_SC->PCLKSEL1 >> 16) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}

    LPC_UART2->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate  + 1;	/*baud rate */
    LPC_UART2->DLM = Fdiv / 256;							
    LPC_UART2->DLL = Fdiv % 256;
	LPC_UART2->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART2->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART2_IRQn);

    LPC_UART2->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
    return 0;
  }
  else if ( PortNum == 3 )
  {
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 8,9 are for UART1 */
	pclkdiv = (LPC_SC->PCLKSEL1 >> 18) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}

    LPC_UART3->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	Fdiv = ( pclk / 16 ) / baudrate  + 1;	/*baud rate */
    LPC_UART3->DLM = Fdiv / 256;							
    LPC_UART3->DLL = Fdiv % 256;
	LPC_UART3->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART3->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */

   	NVIC_EnableIRQ(UART3_IRQn);

    LPC_UART3->IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
    return 0;
  }
  return -1; 
}


static void CPU_TIMER_EN( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	LPC_TIM0->TCR = 1;
  }
  else if ( timer_num == 1 )
  {
	LPC_TIM1->TCR = 1;
  }
  else if ( timer_num == 2 )
  {
	LPC_TIM2->TCR = 1;
  }
  else if ( timer_num == 3 )
  {
	LPC_TIM3->TCR = 1;
  }
  return;
}

static void CPU_TIMER_DEN( uint8_t timer_num )
{
  if ( timer_num == 0 )
  {
	LPC_TIM0->TCR = 0;
  }
  else if ( timer_num == 1 )
  {
	LPC_TIM1->TCR = 0;
  }
  else if ( timer_num == 2 )
  {
	LPC_TIM2->TCR = 0;
  }
  else if ( timer_num == 3 )
  {
	LPC_TIM2->TCR = 0;
  }
  return;
}
static void CPU_TIMER_RESET( uint8_t timer_num )
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
	regVal = LPC_TIM0->TCR;
	regVal |= 0x02;
	LPC_TIM0->TCR = regVal;
  }
  else if ( timer_num == 1 )
  {
	regVal = LPC_TIM1->TCR;
	regVal |= 0x02;
	LPC_TIM1->TCR = regVal;
  }
  else if ( timer_num == 2 )
  {
	regVal = LPC_TIM2->TCR;
	regVal |= 0x02;
	LPC_TIM2->TCR = regVal;
  }
  else if ( timer_num == 3 )
  {
	regVal = LPC_TIM3->TCR;
	regVal |= 0x02;
	LPC_TIM3->TCR = regVal;
  }
  return;
}


static void TIMER0_IRQHandler (void) 
{  
  if ( LPC_TIM0->IR & (0x1<<0) )
  {
	LPC_TIM0->IR = 0x1<<0;		/* clear interrupt flag */
	TICK_COUNT_INC(1);
  }
  if ( LPC_TIM0->IR & (0x1<<1) )
  {
    LPC_TIM0->IR = 0x1<<1;		/* clear interrupt flag */
  }
  if ( LPC_TIM0->IR & (0x1<<4) )
  {
	LPC_TIM0->IR = 0x1<<4;		/* clear interrupt flag */
  }
  if ( LPC_TIM0->IR & (0x1<<5) )
  {
	LPC_TIM0->IR = 0x1<<5;		/* clear interrupt flag */
  }
  return;
}
static void TIMER1_IRQHandler (void)  
{  
  if ( LPC_TIM1->IR & (0x1<<0) )
  {
    LPC_TIM1->IR = 0x1<<0;		/* clear interrupt flag */
  }
  if ( LPC_TIM1->IR & (0x1<<1) )
  {
    LPC_TIM1->IR = 0x1<<1;		/* clear interrupt flag */
  }
  if ( LPC_TIM1->IR & (0x1<<4) )
  {
	LPC_TIM1->IR = 0x1<<4;		/* clear interrupt flag */
  }
  if ( LPC_TIM1->IR & (0x1<<5) )
  {
	LPC_TIM1->IR = 0x1<<5;		/* clear interrupt flag */
  }
  return;
}

/* TIME_INTERVALmS is a value to load the timer match register with
   to get a 1 mS delay */
#define TIME_INTERVALmS	1000
static int CPU_TIMER_INIT ( uint8_t timer_num, uint32_t TimerInterval ) 
{
  uint32_t pclkdiv, pclk;

  if ( timer_num == 0 )
  {
	LPC_SC->PCONP |= (0x01<<1);

	LPC_TIM0->IR = 0x0F;          /* Clear MATx interrupt include DMA request */ 

	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 2~3 is for TIMER0 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 2) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}
	LPC_TIM0->PR  = pclk/1000000; /* set prescaler to get 1 M counts/sec */

	LPC_TIM0->MR0 = TIME_INTERVALmS * 1; /* Set up 10 mS interval */
	LPC_TIM0->MR1 = TIME_INTERVALmS * 1; /* Set up 10 mS interval */
#if TIMER_MATCH
	LPC_TIM0->EMR &= ~(0xFF<<4);
	LPC_TIM0->EMR |= ((0x3<<4)|(0x3<<6));
#else
	/* Capture 0 and 1 on rising edge, interrupt enable. */
	LPC_TIM0->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
#endif
	LPC_TIM0->MCR = (0x3<<0)|(0x3<<3);	/* Interrupt and Reset on MR0 and MR1 */
	NVIC_EnableIRQ(TIMER0_IRQn);
	return (0);
  }
  else if ( timer_num == 1 )
  {
	LPC_SC->PCONP |= (0x1<<2);

	LPC_TIM1->IR = 0x0F;          /* Clear MATx interrupt include DMA request */
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 4~5 is for TIMER0 */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 4) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	} 
	LPC_TIM1->PR  = pclk/1000000; /* set prescaler to get 1 M counts/sec */
	LPC_TIM1->MR0 = TIME_INTERVALmS * 1; /* Set up 10 mS interval */
	LPC_TIM1->MR1 = TIME_INTERVALmS * 1; /* Set up 10 mS interval */
#if TIMER_MATCH
	LPC_TIM1->EMR &= ~(0xFF<<4);
	LPC_TIM1->EMR |= ((0x3<<4)|(0x3<<6));
#else
	/* Capture 0/1 on rising edge, interrupt enable. */
	LPC_TIM1->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
#endif
	LPC_TIM1->MCR = (0x3<<0)|(0x3<<3);	/* Interrupt and Reset on MR0 and MR1 */
	NVIC_EnableIRQ(TIMER1_IRQn);
	return (0);
  }
  return (-1);
}

void hx_tick_count_init(void)
{
	CPU_TIMER_INIT(0,1);	//0,1ms
	CPU_TIMER_EN(0);
}



