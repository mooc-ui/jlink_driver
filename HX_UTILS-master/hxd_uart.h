/*
 * hxd_uart.h
 *
 *  Created on: 2016Äê11ÔÂ14ÈÕ
 *      Author: houxd
 */

#ifndef HX_UTILS_HXD_UART_H_
#define HX_UTILS_HXD_UART_H_

#include "hx_device.h"


typedef struct __UART_PRIVATE_DATA_T {
	int rx_pos;
	int rx_read;
	int tx_pos;
	int tx_wtire;
} UART_PRIVATE_T;


/*
	uart device instance
*/
typedef struct __UART_DEV_T{
	/*
		must be extends DEV_T
	*/
	DEV_T dev;
	/*
		private propertys
	*/
	int rxbuf_size;
	int txbuf_size;
	volatile char *rxbuf;
	volatile char *txbuf;
	int default_bps;
	UART_PRIVATE_T *prv;
} UART_DEV_T;


typedef struct UART_DRIVER_T {
	DEV_DRV_T cdrv;
	int (*tx_start_byte)(const UART_DEV_T *dev,int _byte);
	int (*tx_end_session)(const UART_DEV_T *dev);
	int (*interrupt_ctrl)(const UART_DEV_T *dev,int enable);
	int (*open)(const UART_DEV_T *dev,int bps);
	int (*close)(const UART_DEV_T *dev);
}UART_DRV_T;

/*
 * inner call
 */
extern int hxd_uart_open(HX_DEV *dev,const char *param);
#define IOCTL_GETS_NOBLOCK			(101)
//hx_ioctl(d,IOCTL_GETS_NOBLOCK,buff,buff_size);
extern int hxd_uart_ioctl(HX_DEV *dev,int cmd,va_list va);
extern int hxd_uart_read(HX_DEV *dev,void *buf,int _size);
extern int hxd_uart_write(HX_DEV *dev,const void *buf, int size);
extern int hxd_uart_close(HX_DEV *dev);

/*
 * .cdev = ...
 */
#define CDRV_UART_STD_DRIVER 	\
{								\
	.open = hxd_uart_open,		\
	.read = hxd_uart_read,		\
	.write = hxd_uart_write,	\
	.close = hxd_uart_close,	\
	.ioctl = hxd_uart_ioctl,	\
}

/*
 * extern calls
 */
/*
 * use for get data at uart rx interrupt
 */
extern void hxd_uart_rx_byte(const UART_DEV_T *uart, int _byte);
/*
 * use for end send at uart rx interrupt
 */
extern void hxd_uart_tx_byte(const UART_DEV_T *uart);


extern int hx_register_uart_device(const UART_DEV_T *dev);

#endif /* HX_UTILS_HXD_UART_H_ */
