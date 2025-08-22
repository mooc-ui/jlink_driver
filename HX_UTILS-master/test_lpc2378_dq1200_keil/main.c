#include "hx_utils.h"

/*****************************************************************************
 *   type.h:  Type definition Header file for NXP LPC230x Family
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;

#endif  /* __TYPE_H__ */



/*****************************************************************************
 *   target.h:  Header file for NXP LPC230x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TARGET_H
#define __TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

    /* If USB device is used, the CCLK setting needs to be 57.6Mhz, CCO will be 288Mhz
    to get precise USB clock 48Mhz. If USB is not used, you set any clock you want
    based on the table below. If you want to use USB, change "define USE_USB" from 0 to 1 */

#define	USE_USB					1

//This segment should not be modified
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif


    /* PLL Setting Table Matrix */
    /*
    	Main Osc.	CCLKCFG		Fcco		Fcclk 		M 	N
    	12Mhz		29		300Mhz		10Mhz		24	1
    	12Mhz		35		360Mhz		10Mhz		14	0
    	12Mhz		27		336Mhz		12Mhz		13	0
    	12Mhz		14		300Mhz		20Mhz		24	1
    	12Mhz		17		360Mhz		20Mhz		14	0
    	12Mhz		13		336Mhz		24Mhz		13	0
    	12Mhz		11		300Mhz		25Mhz		24	1
    	12Mhz		9		300Mhz		30Mhz		24	1
    	12Mhz		11		360Mhz		30Mhz		14	0
    	12Mhz		9		320Mhz		32Mhz		39	2
    	12Mhz		9		350Mhz		35Mhz		174	11
    	12Mhz		7		312Mhz		39Mhz		12	0
     	12Mhz		8		360Mhz		40Mhz		14	0
    	12Mhz		7		360Mhz		45Mhz		14	0
    	12Mhz		6		336Mhz		48Mhz		13	0
    	12Mhz		5		300Mhz		50Mhz		24	1
    	12Mhz		5		312Mhz		52Mhz		12	0
    	12Mhz		5		336Mhz		56Mhz		13	0
     	12Mhz		4		300Mhz		60Mhz		24	1
      	12Mhz		4		320Mhz		64Mhz		39	2
    	12Mhz		4		350Mhz		70Mhz		174	11
    	12Mhz		4		360Mhz		72Mhz		14	0
    	12Mhz		3		300Mhz		75Mhz		24	1
    	12Mhz		3		312Mhz		78Mhz		12	0
    	12Mhz		3		320Mhz		80Mhz		39	2
    	12Mhz		3		336Mhz		84Mhz		13	0
    */

#if USE_USB		/* 1 is USB, 0 is non-USB related */
    /* Fcck = 57.6Mhz, Fosc = 288Mhz, and USB 48Mhz */
#define PLL_MValue			11
#define PLL_NValue			0
#define CCLKDivValue		4
#define USBCLKDivValue		5

    /* System configuration: Fosc, Fcclk, Fcco, Fpclk must be defined */
    /* PLL input Crystal frequence range 4KHz~20MHz. */
#define Fosc	12000000
    /* System frequence,should be less than 80MHz. */
#define Fcclk	57600000
#define Fcco	288000000
#else

    /* Fcck = 50Mhz, Fosc = 300Mhz, and USB 48Mhz */
#define PLL_MValue			24
#define PLL_NValue			1
#define CCLKDivValue		5
#define USBCLKDivValue		6

    /* System configuration: Fosc, Fcclk, Fcco, Fpclk must be defined */
    /* PLL input Crystal frequence range 4KHz~20MHz. */
#define Fosc	12000000
    /* System frequence,should be less than 80MHz. */
#define Fcclk	50000000
#define Fcco	300000000

#endif

    /* APB clock frequence , must be 1/2/4 multiples of ( Fcclk/4 ). */
    /* If USB is enabled, the minimum APB must be greater than 16Mhz */
#if USE_USB
#define Fpclk	(Fcclk / 2)
#else
#define Fpclk	(Fcclk / 4)
#endif

    /******************************************************************************
    ** Function name:		TargetInit
    **
    ** Descriptions:		Initialize the target board; it is called in a
    **				necessary place, change it as needed
    **
    ** parameters:			None
    ** Returned value:		None
    **
    ******************************************************************************/
    extern void TargetInit(void);
    extern void ConfigurePLL( void );
    extern void TargetResetInit(void);

#ifdef __cplusplus
}
#endif

#endif /* end __TARGET_H */
/******************************************************************************
**                            End Of File
******************************************************************************/

/******************************************************************************
 *   irq.h:  Interrupt related Header file for NXP LPC230x Family
 *   Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
//#include "type.h"
#ifndef __IRQ_H
#define __IRQ_H

#define I_Bit			0x80
#define F_Bit			0x40

#define SYS32Mode		0x1F
#define IRQ32Mode		0x12
#define FIQ32Mode		0x11

#define HIGHEST_PRIORITY	0x01
#define LOWEST_PRIORITY		0x0F
#define UART_PRIORITY 0X02
#define	WDT_INT			0
#define SWI_INT			1
#define ARM_CORE0_INT		2
#define	ARM_CORE1_INT		3
#define	TIMER0_INT		4
#define TIMER1_INT		5
#define UART0_INT		6
#define	UART1_INT		7
#define	PWM0_1_INT		8
#define I2C0_INT		9
#define SPI0_INT		10			/* SPI and SSP0 share VIC slot */
#define SSP0_INT		10
#define	SSP1_INT		11
#define	PLL_INT			12
#define RTC_INT			13
#define EINT0_INT		14
#define EINT1_INT		15
#define EINT2_INT		16
#define EINT3_INT		17
#define	ADC0_INT		18
#define I2C1_INT		19
#define BOD_INT			20
#define EMAC_INT		21
#define USB_INT			22
#define CAN_INT			23
#define MCI_INT			24
#define GPDMA_INT		25
#define TIMER2_INT		26
#define TIMER3_INT		27
#define UART2_INT		28
#define UART3_INT		29
#define I2C2_INT		30
#define I2S_INT			31

#define VIC_SIZE		32

#define VECT_ADDR_INDEX	0x100
#define VECT_CNTL_INDEX 0x200

/* Be aware that, from compiler to compiler, nested interrupt will have to
be handled differently. More details can be found in Philips LPC2000
family app-note AN10381 */

/* unlike Keil CARM Compiler, in ARM's RealView compiler, don't save and
restore registers into the stack in RVD as the compiler does that for you.
See RVD ARM compiler Inline and embedded assemblers, "Rules for
using __asm and asm keywords. */
static DWORD sysreg;		/* used as LR register */
#define IENABLE __asm { MRS sysreg, SPSR; MSR CPSR_c, #SYS32Mode }
#define IDISABLE __asm { MSR CPSR_c, #(IRQ32Mode|I_Bit); MSR SPSR_cxsf, sysreg }

void init_VIC( void );
DWORD install_irq( DWORD IntNumber, void *HandlerAddr, DWORD Priority );

#endif /* end __IRQ_H */

/******************************************************************************
**                            End Of File
******************************************************************************/



/*****************************************************************************
 *   target.c:  Target C file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
*****************************************************************************/
#include "LPC23xx.h"
//#include "type.h"
//#include "irq.h"
//#include "target.h"

/******************************************************************************
** Function name:		TargetInit
**
** Descriptions:		Initialize the target board; it is called in a necessary
**						place, change it as needed
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TargetInit(void)
{
    /* Add your codes here */
    return;
}

/******************************************************************************
** Function name:		GPIOResetInit
**
** Descriptions:		Initialize the target board before running the main()
**				function; User may change it as needed, but may not
**				deleted it.
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void GPIOResetInit( void )
{
    /* Reset all GPIO pins to default: primary function */
    PINSEL0 = 0x00000000;
    PINSEL1 = 0x00000000;
    PINSEL2 = 0x00000000;
    PINSEL3 = 0x00000000;
    PINSEL4 = 0x00000000;
    PINSEL5 = 0x00000000;
    PINSEL6 = 0x00000000;
    PINSEL7 = 0x00000000;
    PINSEL8 = 0x00000000;
    PINSEL9 = 0x00000000;
    PINSEL10 = 0x00000000;

    IODIR0 = 0x00000000;
    IODIR1 = 0x00000000;
    IOSET0 = 0x00000000;
    IOSET1 = 0x00000000;

    FIO0DIR = 0x00000000;
    FIO1DIR = 0x00000000;
    FIO2DIR = 0x00000000;
    FIO3DIR = 0x00000000;
    FIO4DIR = 0x00000000;

    FIO0SET = 0x00000000;
    FIO1SET = 0x00000000;
    FIO2SET = 0x00000000;
    FIO3SET = 0x00000000;
    FIO4SET = 0x00000000;
    return;
}

/******************************************************************************
** Function name:		ConfigurePLL
**
** Descriptions:		Configure PLL switching to main OSC instead of IRC
**						at power up and wake up from power down.
**						This routine is used in TargetResetInit() and those
**						examples using power down and wake up such as
**						USB suspend to resume, ethernet WOL, and power management
**						example
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void ConfigurePLL ( void )
{
    DWORD MValue, NValue;

    if ( PLLSTAT & (1 << 25) )
    {
        PLLCON = 1;			/* Enable PLL, disconnected */
        PLLFEED = 0xaa;
        PLLFEED = 0x55;
    }

    PLLCON = 0;				/* Disable PLL, disconnected */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;

    SCS |= 0x20;			/* Enable main OSC */
    while( !(SCS & 0x40) );	/* Wait until main OSC is usable */

    CLKSRCSEL = 0x1;		/* select main OSC, 12MHz, as the PLL clock source */

    PLLCFG = PLL_MValue | (PLL_NValue << 16);
    PLLFEED = 0xaa;
    PLLFEED = 0x55;

    PLLCON = 1;				/* Enable PLL, disconnected */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;

    CCLKCFG = CCLKDivValue;	/* Set clock divider */
#if USE_USB
    USBCLKCFG = USBCLKDivValue;		/* usbclk = 288 MHz/6 = 48 MHz */
#endif

    while ( ((PLLSTAT & (1 << 26)) == 0) );	/* Check lock bit status */

    MValue = PLLSTAT & 0x00007FFF;
    NValue = (PLLSTAT & 0x00FF0000) >> 16;
    while ((MValue != PLL_MValue) && ( NValue != PLL_NValue) );

    PLLCON = 3;				/* enable and connect */
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
    while ( ((PLLSTAT & (1 << 25)) == 0) );	/* Check connect bit status */
    return;
}

/******************************************************************************
** Function name:		TargetResetInit
**
** Descriptions:		Initialize the target board before running the main()
**						function; User may change it as needed, but may not
**						deleted it.
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TargetResetInit(void)
{
#ifdef __DEBUG_RAM
    MEMMAP = 0x2;			/* remap to internal RAM */
#endif

#ifdef __DEBUG_FLASH
    MEMMAP = 0x1;			/* remap to internal flash */
#endif

#if USE_USB
    PCONP |= 0x80000000;		/* Turn On USB PCLK */
#endif
    /* Configure PLL, switch from IRC to Main OSC */
    ConfigurePLL();

    /* Set system timers for each component */
#if (Fpclk / (Fcclk / 4)) == 1
    PCLKSEL0 = 0x00000000;	/* PCLK is 1/4 CCLK */
    PCLKSEL1 = 0x00000000;
#endif
#if (Fpclk / (Fcclk / 4)) == 2
    PCLKSEL0 = 0xAAAAAAAA;	/* PCLK is 1/2 CCLK */
    PCLKSEL1 = 0xAAAAAAAA;	 //28.8       57.6
#endif
#if (Fpclk / (Fcclk / 4)) == 4
    PCLKSEL0 = 0x55555555;	/* PCLK is the same as CCLK */
    PCLKSEL1 = 0x55555555;
#endif

    /* Set memory accelerater module*/
    MAMCR = 0;
#if Fcclk < 20000000
    MAMTIM = 1;
#else
#if Fcclk < 40000000
    MAMTIM = 2;
#else
    MAMTIM = 3;
#endif
#endif
    MAMCR = 2;

    GPIOResetInit();

    init_VIC();
    return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

/* Initialize the interrupt controller */
/******************************************************************************
** Function name:		init_VIC
**
** Descriptions:		Initialize VIC interrupt controller.
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void init_VIC(void)
{
    DWORD i = 0;
    DWORD *vect_addr, *vect_cntl;

    /* initialize VIC*/
    VICIntEnClr = 0xffffffff;
    VICVectAddr = 0;
    VICIntSelect = 0;

    /* set all the vector and vector control register to 0 */
    for ( i = 0; i < VIC_SIZE; i++ )
    {
        vect_addr = (DWORD *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + i*4);
        vect_cntl = (DWORD *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + i*4);
        *vect_addr = 0x0;
        *vect_cntl = 0xF;
    }
    return;
}

/******************************************************************************
** Function name:		install_irq
**
** Descriptions:		Install interrupt handler
** parameters:			Interrupt number, interrupt handler address,
**						interrupt priority
** Returned value:		true or false, return false if IntNum is out of range
**
******************************************************************************/
DWORD install_irq( DWORD IntNumber, void *HandlerAddr, DWORD Priority )
{
    DWORD *vect_addr;
    DWORD *vect_cntl;

    VICIntEnClr|= (1 << IntNumber);	/* Disable Interrupt */
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
        VICIntEnable|=(1 << IntNumber);	/* Enable Interrupt */
        return( TRUE );
    }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
#include "stdio.h"


unsigned int __get_fpclk(void)
{
	return Fpclk;
}

int main(void)
{
	printf("111\n");
	hx_utils_init();
	hxt_term_run();
	while(1){
		printf("OK\n");
	}
}

