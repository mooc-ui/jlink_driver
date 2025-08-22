#include "hx_cpu.h"
#include "hx_utils.h"
#include "./cpu/lpc2368_reg.h"
#include "./cpu/lpc2368_conf.h"

__weak void brd_uart_rx_byte(int id, int data){}
__weak void brd_uart_tx_byte(int id){}



static volatile uint g_tickcount;

__weak uint cpu_get_tickcount(void)
{
	HX_DBG_PRINTLN ("HX_UTILS:WARNING:cpu not config.");
	return g_tickcount;
}
static uint sysreg;
__weak void cpu_interrupt_ctrl(int en)
{
	if(en){
		__asm { MRS sysreg, SPSR; MSR CPSR_c, #SYS32Mode }
	}else{
		__asm { MSR CPSR_c, #(IRQ32Mode|I_Bit); MSR SPSR_cxsf, sysreg }
	}
}
typedef volatile unsigned long IOREG_T;
typedef struct __IO_T 
{
	IOREG_T* IODIR;
	IOREG_T* IOSET;
	IOREG_T* IOCLR;
	IOREG_T* IOPIN;
} IO_T;

static const IO_T iotbl[] = {
	{&IODIR0,&IOSET0,&IOCLR0,&IOPIN0},
	{&IODIR1,&IOSET1,&IOCLR1,&IOPIN0},

};

__weak void cpu_iomode(int port,uint pin,IOMODE_T val)
{
	if(val == IM_IN)
		*(iotbl[port].IODIR) &= ~pin;
	else
		*(iotbl[port].IODIR) |= pin;
}
__weak uint cpu_ioval(int port)
{
	return *(iotbl[port].IOPIN);
}
__weak void cpu_ioctrl(int port,uint pin,int val)
{
	if(val == 0)
		*(iotbl[port].IOCLR) |= pin;
	else
		*(iotbl[port].IOSET) |= pin;
}
__weak void cpu_ioout(int port,uint val)
{
	*(iotbl[port].IOPIN) = val;
}
__weak void cpu_ioclr(int port,uint pin)
{
	cpu_ioctrl(port,pin,0);
}
__weak void cpu_ioset(int port,uint pin)
{
	cpu_ioctrl(port,pin,1);
}



static uint UART_Init( uint PortNum, uint baudrate ,uint _Fpclk);
static void EnableUart(BYTE portnum);
static void DisableUart(BYTE portnum);

__weak void cpu_uart_tx_start_byte(int id,int data)
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
		
		IOSET1|=1<<29;//????
		
		//U3IER = IER_THRE | IER_RLS;
		U3THR = data;
	}
}
__weak void cpu_uart_tx_end_session(int id)
{

}
__weak int cpu_uart_reopen(int id, int bps, int pclk)
{
	DisableUart(id);
	UART_Init(id,bps,pclk);
	EnableUart(id);
	return 0;
}


static DWORD install_irq( DWORD IntNumber, void *HandlerAddr, DWORD Priority )
{
    DWORD *vect_addr;
    DWORD *vect_cntl;
      
    VICIntEnClr = 1 << IntNumber;	/* Disable Interrupt */
    if ( IntNumber >= VIC_SIZE )
    {
		return ( FALSE );
    }
    else
    {
		/* find first un-assigned VIC address for the handler */
		vect_addr = (DWORD *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber*4);
		vect_cntl = (DWORD *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber*4);
		*vect_addr = (DWORD)HandlerAddr;	/* set interrupt vector */
		*vect_cntl = Priority;
		VICIntEnable = 1 << IntNumber;	/* Enable Interrupt */
		return( TRUE );
    }
}

__weak int cpu_install_irq( int intnr, void *handler, void *arg )
{
	return install_irq(intnr,handler,(DWORD)arg);
}
static void RTCHandler (void) __irq 
{ 
	if(RTC_ILR & B00000001){
		RTC_ILR |= B00000001;		//????
	}else if(RTC_ILR & B00000010){
		RTC_ILR |= B00000010;		//????
	}else if(RTC_ILR & B00000100){
		RTC_ILR |= B00000100;		//?????
		g_tickcount ++;
	}else{
	}
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

__weak int cpu_init_tickcount_1m_by_pclk( uint _pclk )
{
	unsigned int pclk;
    //???????  1 ??  0 ??
	//????	
    RTC_AMR = 0xFF;
	//???? 0:?....7?  0 ?? 1 ??
	//???? 
    RTC_CIIR = 0x00;
	//????: 0:??,1:??[3:2]:??4:???:0 pcklk 1xtal
	//??PCLK is 0
    RTC_CCR = 0;// 1<<4;
	//?????????	7??,[2:0]:000:16??,001:32??...111:2048??
	// 16?? ??
	RTC_CISS = B10000000;	
	
	//?????, ??????? 1000 * 16 ,???16??
	pclk = _pclk;
    RTC_PREINT = pclk / 16000 -1;			//????
    RTC_PREFRAC = pclk % 16000;	//????,???????????..?
	
	if ( cpu_install_irq( RTC_INT, (void *)RTCHandler, (void*)LOWEST_PRIORITY ) == FALSE )
    {
        return -1;
    }
	
	RTC_CCR |= 0x01;	//??rtc
	//??????? 0:????,1:????,2:?????
	//RTC_ILR |=	B00000110;	//???????
	
    return 0;
}


//==================================================



static void UART0Handler (void) __irq
{
    uchar IIRValue, LSRValue;
    uchar Dummy = Dummy;
    //	  I_UC_UNION i_uc_Tmp;
    //  IENABLE;				/* handles nested interrupt */

    IIRValue = U0IIR;

    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
        LSRValue = U0LSR;
        /* Receive Line Status */
        if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
        {
            /* There are errors or break interrupt */
            /* Read LSR will clear the interrupt */
            Dummy = U0RBR;		/* Dummy read on RX to clear
			interrupt, then bail out */
            //IDISABLE;   // by tiger 2012-12-08 ??
            VICVectAddr = 0;		/* Acknowledge Interrupt */
            return;
        }
        if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
        {
			brd_uart_rx_byte(0,U0RBR);
        }
    }
    else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
    {
		brd_uart_rx_byte(0,U0RBR);

    }
    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
        /* Character Time-out indicator */
        U0FCR = 0x07;
        //Dummy = U1RBR;
    }
    else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
    {
        /* THRE interrupt */
        LSRValue = U0LSR;		/* Check status in the LSR to see if
		valid data in U1THR or not */
        if ( LSRValue & LSR_THRE )
        {
			brd_uart_tx_byte(0);

        }
    }
    //  IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

static void UART1Handler (void) __irq
{
    uchar IIRValue, LSRValue;
    uchar Dummy = Dummy;
    //	  I_UC_UNION i_uc_Tmp;
    //  IENABLE;				/* handles nested interrupt */
    IIRValue = U1IIR;
    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
        LSRValue = U1LSR;
        /* Receive Line Status */
        if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
        {
            /* There are errors or break interrupt */
            /* Read LSR will clear the interrupt */
            Dummy = U1RBR;		/* Dummy read on RX to clear interrupt, then bail out */
            //	  IDISABLE;
            VICVectAddr = 0;		/* Acknowledge Interrupt */
            return;
        }
        if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
        {
			brd_uart_rx_byte(1,U1RBR);
            
        }
    }
    else if (IIRValue==IIR_RDA )	/* Receive Data Available */
    {
		brd_uart_rx_byte(1,U1RBR);       
    }
    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
        /* Character Time-out indicator */
        U1FCR = 0x07;
        //Dummy=U0RBR;
    }
    else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
    {
        /* THRE interrupt */
        LSRValue = U1LSR;		/* Check status in the LSR to see if
		valid data in U0THR or not */
        if ( LSRValue & LSR_THRE )
        {
           brd_uart_tx_byte(1);
        }
    }
    //  IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

static void UART2Handler (void) __irq
{
    uchar IIRValue, LSRValue;
    uchar Dummy = Dummy;
    //	  I_UC_UNION i_uc_Tmp;
    //  IENABLE;				/* handles nested interrupt */

    IIRValue = U2IIR;

    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
        LSRValue = U2LSR;
        /* Receive Line Status */
        if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
        {
            /* There are errors or break interrupt */
            /* Read LSR will clear the interrupt */
            Dummy = U2RBR;		/* Dummy read on RX to clear interrupt, then bail out */
            //	  IDISABLE;
            VICVectAddr = 0;		/* Acknowledge Interrupt */
            return;
        }
        if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
        {
            brd_uart_rx_byte(2,U2RBR);
        }
    }
    else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
    {
		brd_uart_rx_byte(2,U2RBR);
       
    }
    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
        /* Character Time-out indicator */
        //Dummy=U2RBR;
        U2FCR = 0x07;
    }
    else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
    {
        /* THRE interrupt */
        LSRValue = U2LSR;		/* Check status in the LSR to see if
		valid data in U0THR or not */
        if ( LSRValue & LSR_THRE )
        {
		brd_uart_tx_byte(2);
        }
    }
    //  IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

// WR 2013-05-22
static void UART3Handler (void) __irq
{
    uchar IIRValue, LSRValue;
    uchar Dummy = Dummy;
    //  IENABLE;				/* handles nested interrupt */
    IIRValue = U3IIR;
    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
        LSRValue = U3LSR;
        /* Receive Line Status */
        if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
        {
            /* There are errors or break interrupt */
            /* Read LSR will clear the interrupt */
            Dummy = U3RBR;		/* Dummy read on RX to clear interrupt, then bail out */
            //	  IDISABLE;
            VICVectAddr = 0;		/* Acknowledge Interrupt */
            return;
        }
        if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
        {
			int data = U3RBR;
			brd_uart_rx_byte(3,data);
            
        }
    }
    else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
    {
		int data = U3RBR;
		brd_uart_rx_byte(3,data);
       
    }
    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
        /* Character Time-out indicator */
        U3FCR = 0x07;
        //Dummy=U3RBR;
    }
    else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
    {
        /* THRE interrupt */
        LSRValue = U3LSR;		/* Check status in the LSR to see if
		valid data in U3THR or not */
        if ( LSRValue & LSR_THRE )
        {
			brd_uart_tx_byte(3);
        }
    }
    //  IDISABLE;
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}


static uint UART_Init( uint PortNum, uint baudrate ,uint _Fpclk)
{
    uint Fdiv;
    uchar Dummy ;
    Dummy=Dummy;
    if(PortNum == 0 )//GPRS??
    {
        //PCONP |= 1<<3;
        //PINSEL0 |= 0x00000050;     // zwjiang modify 2012-6-20  /* RxD0 and TxD0 */
        U0LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
        Fdiv = ( _Fpclk / 16 ) / baudrate ;	/*baud rate */
        U0DLM = Fdiv / 256;
        U0DLL = Fdiv % 256;
        U0LCR = 0x03;		/* DLAB = 0 */
        U0FCR = 0x00;
        U0FCR = 0x06;		/* Enable and reset TX and RX FIFO. */
        Dummy = U0RBR;
        if(install_irq( UART0_INT, (void *)UART0Handler, HIGHEST_PRIORITY )==FALSE)
        {
            return (FALSE);
        }
        U0IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
        return (TRUE);
    }
    else if ( PortNum == 1 )
    {
       // PCONP |= 1<<4;
       // PINSEL4 |= 0x0000000a;       /*p2.0 p2.1 RxD1 and TxD1 */
        U1LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
        Fdiv = ( _Fpclk / 16 ) / baudrate ;	/*baud rate */
        U1DLM = Fdiv / 256;
        U1DLL = Fdiv % 256;
        U1LCR = 0x03;		/* DLAB = 0 */
        U1FCR = 0x00;
        U1FCR = 0x06;//FIFO?????
        Dummy = U1RBR;
        if(install_irq(UART1_INT, (void *)UART1Handler, HIGHEST_PRIORITY )==FALSE )
        {
            return (FALSE);
        }
        U1IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART1 interrupt */
        return (TRUE);
    }
    else if(PortNum == 2)
    {
        //PCONP |= 1<<24;
       // PINSEL4 |= 0x000a0000;       /* RxD2 and TxD2 P2.8 P2.9*/
        U2LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
        Fdiv = ( _Fpclk / 16 ) / baudrate ;	/*baud rate */
        U2DLM = Fdiv / 256;
        U2DLL = Fdiv % 256;
        U2LCR = 0x03;		/* DLAB = 0 */
        U2FCR = 0x00;
        U2FCR = 0x06;		/* Enable and reset TX and RX FIFO. */
        Dummy = U2RBR;
        if ( install_irq( UART2_INT, (void *)UART2Handler, HIGHEST_PRIORITY ) == FALSE )
        {
            return (FALSE);
        }
        U2IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART2 interrupt */
        return (TRUE);
    }
    else if (PortNum == 3)//232??485
    {
      //  PCONP|= 1<<25;
      //  PINSEL0|=0x0000000A; /* Enable TxD3 P0.0 RxD3 P0.1 */
        U3LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
        Fdiv = ( _Fpclk / 16 ) / baudrate ;	/*baud rate */
        U3DLM = Fdiv / 256;
        U3DLL = Fdiv % 256;
        U3LCR = 0x03;		/* DLAB = 0 */
        U3FCR = 0x00;
        U3FCR = 0x06;
        Dummy = U3RBR;
        if(install_irq( UART3_INT, (void *)UART3Handler, HIGHEST_PRIORITY )==FALSE)
        {
            return (FALSE);
        }
        U3IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */
        return (TRUE);
    }                                     // zwjiang add begin 2012-5-8
    return( FALSE );
}

/************************????******************/
static void EnableUart(BYTE portnum)
{
    if(portnum==0)
    {
        VICIntEnable|=(1<<UART0_INT);
    }
    else if(portnum==1)
    {
        VICIntEnable|=(1<<UART1_INT);
    }
    else if(portnum==2)
    {
        VICIntEnable|=(1<< UART2_INT);

    }
    else if(portnum==3)
    {
        VICIntEnable|=(1<< UART3_INT);
    }
}
/********************????********************/
static void DisableUart(BYTE portnum)
{
    if(portnum==0)
    {
        VICIntEnClr|=(1<<UART0_INT);
    }
    else if(portnum==1)
    {
        VICIntEnClr|=(1<<UART1_INT);
    }
    else if(portnum==2)
    {
        VICIntEnClr|=(1<<UART2_INT);
    }
    else if(portnum==3)
    {
        VICIntEnClr|=(1<<UART3_INT);
    }
}

