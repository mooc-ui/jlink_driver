/*
 * hxd_param.h
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
 *      Author: houxd
 */

#ifndef HX_UTILS_HXD_IOPIN_H_
#define HX_UTILS_HXD_IOPIN_H_

#include "hx_device.h"

#define PIN_ATTR_INVERSE		(1u<<24)
//#define PIN_ATTR_OUTPUTONLY	(2u<<24)
//#define PIN_ATTR_INPUTONLY	(4u<<24)
//#define PIN_ATTR_ANALOG		(8u<<24)  

#define _IOPIN_ID(board,port,pin)		((0<<16)|(board<<12)|(port<<8)|(pin<<0))
#define uint	unsigned int

struct IODESC_T {
	/*
	port : port number 
	pin : pin mask, pin 5 is 1<<5
	mode : mode value
	*/
	void (*iomode)(uint port,uint pin,uint mode);
	/*
	port : port number 
	val : pin value, pin5 set is 1<<5
	*/
	void (*ioctrl)(uint port,uint val);
	/*
	port : port number 
	return : pin value
	*/
	uint (*ioval)(uint port);
};

typedef struct __IOPIN_DRV_T{
	/*
		must be extends DEV_DRV_T
	*/
	DEV_DRV_T drv;
	/*
		private propertys
	*/
	const struct IODESC_T **iotbl;
	int tbl_count;
} IOPIN_DRV_T;


#define HX_IOPIN_STD_DRIVER		\
{\
	.open = hxd_iopin_open,\
	.read = hxd_iopin_read,\
	.write = hxd_iopin_write,\
	.close = hxd_iopin_close,\
}

extern int hxd_iopin_open(HX_DEV *dev,const char *s);
extern int hxd_iopin_read(HX_DEV *dev,void *buf,int size);
extern int hxd_iopin_write(HX_DEV *dev,const void *buf, int size);
extern int hxd_iopin_close(HX_DEV *dev);


#endif /* HX_UTILS_HXD_PARAM_H_ */
