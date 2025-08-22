#ifndef __KX_SERIAL_H__
#define __KX_SERIAL_H__

//==================================================
//APIS
/*
	clear current rx buffer, remaber this!!!!
*/
extern void hx_uart_rxclr(int id);
/*
	return 0 is got char
	others is not
	!!! 前后台轮训中,为了保证buff中的数据能在每次轮训时都被处理完
	!!! 调用这个函数多次,直到返回非零
*/
extern int hx_uart_getc_noblock(int id,int *c);
extern int hx_uart_getc_timeout(int id, int *c, int timeout);
extern int hx_uart_getc_block(int id);
/*
	retrun >=0 is has \r or \n
	<0 is null
	!!! 前后台轮训中,为了保证buff中的数据能在每次轮训时都被处理完
	!!! 调用这个函数多次,直到返回负数.
*/
extern int hx_uart_gets_noblock(int id, char *buff, int buff_size);
extern int hx_uart_gets_timeout(int id, char *buff, int buff_size,int timeout);
extern void hx_uart_gets_block(int id, char *buff, int buff_size);

/*
	tx funcs is all noblock, no need others
	once send begain, it will check wether 
	 last sending is compelted.
	 //	除非时序非常严格的首发,目前足够了
*/
extern void hx_uart_putc(int id, int c);
/*
	when len==0 this is usefor wait last send compelte.
*/
extern int hx_uart_send(int id, const char *data, int len);
extern void hx_uart_put(int id,const char *s);
extern void hx_uart_puts(int id,const char *s);
extern int hx_uart_printf(int port_nr, const  char *format, ...);

/*
	init
*/
extern int hx_uart_init(int id, int bps);

#endif



