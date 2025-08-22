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
	!!! ǰ��̨��ѵ��,Ϊ�˱�֤buff�е���������ÿ����ѵʱ����������
	!!! ��������������,ֱ�����ط���
*/
extern int hx_uart_getc_noblock(int id,int *c);
extern int hx_uart_getc_timeout(int id, int *c, int timeout);
extern int hx_uart_getc_block(int id);
/*
	retrun >=0 is has \r or \n
	<0 is null
	!!! ǰ��̨��ѵ��,Ϊ�˱�֤buff�е���������ÿ����ѵʱ����������
	!!! ��������������,ֱ�����ظ���.
*/
extern int hx_uart_gets_noblock(int id, char *buff, int buff_size);
extern int hx_uart_gets_timeout(int id, char *buff, int buff_size,int timeout);
extern void hx_uart_gets_block(int id, char *buff, int buff_size);

/*
	tx funcs is all noblock, no need others
	once send begain, it will check wether 
	 last sending is compelted.
	 //	����ʱ��ǳ��ϸ���׷�,Ŀǰ�㹻��
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



