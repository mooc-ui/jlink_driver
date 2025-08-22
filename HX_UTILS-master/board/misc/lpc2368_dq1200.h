
/*
	if define below as zero ,that is to disbale this port.
*/
//DQ1200
#define UART0_RX_BUFF_SIZE			(1024)
#define UART0_TX_BUFF_SIZE			(1024)
#define UART1_RX_BUFF_SIZE			(0)
#define UART1_TX_BUFF_SIZE			(0)
#define UART2_RX_BUFF_SIZE			(0)
#define UART2_TX_BUFF_SIZE			(0)
#define UART3_RX_BUFF_SIZE			(512)
#define UART3_TX_BUFF_SIZE			(512)

#define UART_PORTS_NUM				(4)

#if UART_PORTS_NUM>4
	#error current version can not surpport uart count more than 4
#endif


//=====================================================
//config debug info output uart index
#define UART_DEBUG_PORT		(3)

//=====================================================
//config debug info output uart index
#define UART_TERMINAL_PORT	(3)

//=====================================================
//PORTS OF AT CMD PORT
#define UART_AT_PORT		(0)



#include "rtc.h"

