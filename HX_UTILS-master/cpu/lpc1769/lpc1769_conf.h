//====================================================================================
//lpc2378 ≈‰÷√∂®“Â
#ifndef __LPC1769_CONF_H 
#define __LPC1769_CONF_H



#ifndef WORD
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;
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



/* TIME_INTERVALmS is a value to load the timer match register with
   to get a 1 mS delay */
#define TIME_INTERVALmS	1000

#define TIME_INTERVAL	(9000000/100 - 1)

 
#endif /* end __TARGET_H */
