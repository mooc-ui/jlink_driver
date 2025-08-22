#ifndef __CPU_H__
#define __CPU_H__

#include "compile.h"

extern void cpu_interrupt_ctrl(int en);

int cpu_install_irq( int intnr, void *handler, void *arg );

int cpu_init_tickcount_1m_by_pclk( uint _pclk );
uint cpu_get_tickcount(void);

typedef enum __IOMODE_T {
	IM_IN = 0,
	IM_OUT = 1,
} IOMODE_T;

#define PORT0		(0)
#define PORT1		(1)
#define PORT2		(2)
#define PORT_MAX	(10)


extern void cpu_iomode(int port,uint pin,IOMODE_T val);
extern uint cpu_ioval(int port);
extern void cpu_ioctrl(int port,uint val);
extern void cpu_ioclr(int port,uint pin);
extern void cpu_ioset(int port,uint pin);



extern void cpu_uart_tx_start_byte(int id,int data);
extern void cpu_uart_tx_end_session(int id);
extern int cpu_uart_reopen(int id, int bps, int pclk);

#endif
