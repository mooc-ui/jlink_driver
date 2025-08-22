#ifndef __CPU_H__
#define __CPU_H__

#include "compile.h"
#include "stdint.h"

#define USE_RTX		1

extern void cpu_uart_interrupt_ctrl(int pid,int en);

int cpu_install_irq( int intnr, void *handler, void *arg );

extern int timer_init_use_for_tickcount(uint32_t sysclk);
uint cpu_get_tickcount(void);


extern void timer_init2(uint32_t sysclk);
extern uint timer_getval(void);
extern void timer_setval(uint val);

typedef enum __IOMODE_T {
	IM_IN = 0,
	IM_OUT = 1,
} IOMODE_T;

#define PORT0		(0)
#define PORT1		(1)
#define PORT2		(2)
#define PORT3		(3)
#define PORT4		(4)
#define PORT5		(5)
#define PORT6		(6)
#define PORT7		(7)
#define PORT_MAX	(10)


extern void cpu_iomode(int port,uint pin,IOMODE_T val);
extern uint cpu_ioval(int port);
extern void cpu_ioctrl(int port,uint val);
extern void cpu_ioset(int port,uint pin);
extern void cpu_ioclr(int port,uint pin);
extern void cpu_uart_tx_start_byte(int id,int data);
extern void cpu_uart_tx_end_session(int id);
extern int cpu_uart_reopen(uint32_t id, uint32_t bps, uint32_t sysclk);

#endif
