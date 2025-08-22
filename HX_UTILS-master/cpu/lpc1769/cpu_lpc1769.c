#include "compile.h"
#include "./cpu/lpc1769/lpc1769_reg.h"
#include "./cpu/lpc1769/lpc1769_conf.h"
#include "./cpu/lpc1769/cpu_lpc1769.h"

//===============================================================
// tickcount
#if USE_RTX == 0
volatile unsigned __g_cpu_tickcount;

uint cpu_get_tickcount(void)
{
	return __g_cpu_tickcount;
}
void TIMER0_IRQHandler (void) 
{  
  if ( LPC_TIM0->IR & (0x1<<0) )
  {
    LPC_TIM0->IR = 0x1<<0;		/* clear interrupt flag */
	__g_cpu_tickcount ++;
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

#endif

void TIMER1_IRQHandler (void)  
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


void enable_timer( uint8_t timer_num )
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


void disable_timer( uint8_t timer_num )
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


void reset_timer( uint8_t timer_num )
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


static uint32_t init_timer ( uint8_t timer_num, uint32_t TimerInterval ,uint32_t SystemFrequency) 
{
  uint32_t pclkdiv, pclk=0;
	pclk=pclk;

  if ( timer_num == 0 )
  {
#if USE_RTX == 0
	LPC_SC->PCONP |= (0x01<<1);
//#if TIMER_MATCH
//	LPC_PINCON->PINSEL3 &= ~((0x3<<24)|(0x3<<26));
//	LPC_PINCON->PINSEL3 |= ((0x3<<24)|(0x3<<26));
//#else
//	LPC_PINCON->PINSEL3 &= ~((0x3<<20)|(0x3<<22));
//	LPC_PINCON->PINSEL3 |= ((0x3<<20)|(0x3<<22));
//#endif
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
#endif
	return (TRUE);
  }
  else if ( timer_num == 1 )
  {
	LPC_SC->PCONP |= (0x1<<2);
//#if TIMER_MATCH
//	LPC_PINCON->PINSEL3 &= ~((0x3<<12)|(0x3<<18));
//	LPC_PINCON->PINSEL3 |= ((0x3<<12)|(0x3<<18));
//#else
//	LPC_PINCON->PINSEL3 &= ~((0x3<<4)|(0x3<<6));
//	LPC_PINCON->PINSEL3 |= ((0x3<<4)|(0x3<<6));
//#endif
	LPC_TIM1->IR = 0x0F;          /* Clear MATx interrupt include DMA request */
	/* By default, the PCLKSELx value is zero, thus, the PCLK for
	all the peripherals is 1/4 of the SystemFrequency. */
	/* Bit 4~5 is for TIMER0 */
	  LPC_SC->PCLKSEL0 |= 1<<4;
	  LPC_SC->PCLKSEL0 &= ~(1<<5);
	pclkdiv = (LPC_SC->PCLKSEL0 >> 4) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;   //<<----------
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	} 
	//fuck when pr ==0 the tc increased with pclk
	//else pr increased with pclk, tc increase whit pr over
	LPC_TIM1->PR  = 0;//pclk; /* set prescaler to get 1 M counts/sec */
//	LPC_TIM1->MR0 = 1; /* Set up 10 mS interval */
//	LPC_TIM1->MR1 =  1; /* Set up 10 mS interval */
//#if TIMER_MATCH
//	LPC_TIM1->EMR &= ~(0xFF<<4);
//	LPC_TIM1->EMR |= ((0x3<<4)|(0x3<<6));
//#else
//	/* Capture 0/1 on rising edge, interrupt enable. */
//	LPC_TIM1->CCR = (0x1<<0)|(0x1<<2)|(0x1<<3)|(0x1<<5);
//#endif
//	LPC_TIM1->MCR = (0x3<<0)|(0x3<<3);	/* Interrupt and Reset on MR0 and MR1 */
	//NVIC_EnableIRQ(TIMER1_IRQn);		//no intrrupt
	return (TRUE);
  }
  return (FALSE);
}
void timer_init2(uint32_t sysclk)
{
	init_timer(1,1,sysclk);
	enable_timer(1);
}
uint timer_getval(void)
{
	return LPC_TIM1->TC;
}
void timer_setval(uint val)
{
	LPC_TIM1->TC = val;
}

int timer_init_use_for_tickcount(uint32_t sysclk)
{
	init_timer(0,1,sysclk);	//use for time tick in 1 ms
	enable_timer(0);
	return 0;
}
typedef void (*TimerCallback)(void);
void timer_add_callback(TimerCallback call)
{

}
//===============================================================
//gpio
LPC_GPIO_TypeDef* const io_tbl[] = {
	LPC_GPIO0,LPC_GPIO1,LPC_GPIO2,LPC_GPIO3,LPC_GPIO4,
};

void cpu_iomode(int port,uint pin,IOMODE_T val)
{
	if(val == IM_IN)
		io_tbl[port]->FIODIR &= ~pin;
	else
		io_tbl[port]->FIODIR |= pin;
}
uint cpu_ioval(int port)
{
	return io_tbl[port]->FIOPIN;
}
void cpu_ioctrl(int port,uint val)
{
	io_tbl[port]->FIOPIN = val;
}
void cpu_ioset(int port,uint pin)
{
	cpu_ioctrl((port),cpu_ioval((port))|(pin));
}
void cpu_ioclr(int port,uint pin)
{	
	cpu_ioctrl((port),cpu_ioval((port))&(~(pin)));
}

//=====================================================================
//uart
extern void brd_uart_rx_byte(int id, int data);
extern void brd_uart_tx_byte(int id);
extern void brd_uart_rx_byte(int id, int data);
extern void brd_uart_tx_byte(int id);

static uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate ,uint32_t SystemFrequency);
static void EnableUart(BYTE portnum);
static void DisableUart(BYTE portnum);

void cpu_uart_tx_start_byte(int id,int data)
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
void cpu_uart_tx_end_session(int id)
{

}
int cpu_uart_reopen(uint32_t id, uint32_t bps, uint32_t sysclk)
{
	DisableUart(id);
	UARTInit(id,bps,sysclk);
	EnableUart(id);
	return 0;
}

#define UART_RX_BYTE(a,b)		brd_uart_rx_byte(a,b)
#define UART_TX_BYTE(a)			brd_uart_tx_byte(a)
static void EnableUart(BYTE portnum)
{
	if(portnum == 0)
		NVIC_EnableIRQ(UART0_IRQn);
	else if(portnum == 1)
		NVIC_EnableIRQ(UART1_IRQn);
	else if(portnum == 2)
		NVIC_EnableIRQ(UART2_IRQn);
	else if(portnum == 3)
		NVIC_EnableIRQ(UART3_IRQn);
}
static void DisableUart(BYTE portnum)
{
	if(portnum == 0)
		NVIC_DisableIRQ(UART0_IRQn);
	else if(portnum == 1)
		NVIC_DisableIRQ(UART1_IRQn);
	else if(portnum == 2)
		NVIC_DisableIRQ(UART2_IRQn);
	else if(portnum == 3)
		NVIC_DisableIRQ(UART3_IRQn);
}

void cpu_uart_interrupt_ctrl(int pid,int en)
{
	if(en)
		EnableUart(pid);
	else
		DisableUart(pid);
}
//static volatile uint32_t UART0Status, UART1Status, UART2Status, UART3Status;
//static volatile uint8_t UART0TxEmpty = 1, UART1TxEmpty = 1, UART2TxEmpty = 1, UART3TxEmpty = 1;
//volatile uint8_t UART0Buffer[BUFSIZE], UART1Buffer[BUFSIZE];
//volatile uint32_t UART0Count = 0, UART1Count = 0;

/*****************************************************************************
** Function name:		UART0_IRQHandler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
** 
*****************************************************************************/
void UART0_IRQHandler (void) 
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


void UART1_IRQHandler (void) 
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


void UART2_IRQHandler (void) 
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


void UART3_IRQHandler (void) 
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

static uint32_t UARTInit( uint32_t PortNum, uint32_t baudrate ,uint32_t SystemFrequency)
{
  uint32_t Fdiv;
  uint32_t pclkdiv, pclk;

  if ( PortNum == 0 )
  {
	LPC_PINCON->PINSEL0 &= ~0x000000F0;
	LPC_PINCON->PINSEL0 |= 0x00000050;  /* RxD0 is P0.3 and TxD0 is P0.2 */
	LPC_PINCON->PINMODE0 &= ~0x000000F0;
	LPC_PINCON->PINMODE0 |= 0x00000050;
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
    return (TRUE);
  }
  else if ( PortNum == 1 )
  {
	LPC_PINCON->PINSEL4 &= ~0x0000000F;
	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */
	LPC_PINCON->PINMODE4 &= ~0x0000000F;
	LPC_PINCON->PINMODE4 |= 0x00000005;
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
    return (TRUE);
  }
  else if ( PortNum == 2 )
  {
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
    return (TRUE);
  }
  else if ( PortNum == 3 )
  {
//	LPC_PINCON->PINSEL4 &= ~0x0000000F;
//	LPC_PINCON->PINSEL4 |= 0x0000000A;	/* Enable RxD1 P2.1, TxD1 P2.0 */
	
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
    return (TRUE);
  }
  return( FALSE ); 
}



