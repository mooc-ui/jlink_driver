/*
 * hxd_ioport.h
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
 *      Author: houxd
 */

#ifndef HX_UTILS_HXD_IOPORT_H_
#define HX_UTILS_HXD_IOPORT_H_

#include "hx_device.h"
#include "hxd_iopin.h"

#define uint	unsigned int
	
typedef struct __IOPORT_DRV_T{
	/*
		must be extends DEV_DRV_T
	*/
	DEV_DRV_T drv;
	/*
		private propertys
	*/
	const struct IODESC_T *iodesc;
} IOPORT_DRV_T;


#define HX_IOPORT_STD_DRIVER		\
{\
	.open = hxd_ioport_open,\
	.read = hxd_ioport_read,\
	.write = hxd_ioport_write,\
	.close = hxd_ioport_close,\
}

extern int hxd_ioport_open(HX_DEV *dev,const char *s);
extern int hxd_ioport_read(HX_DEV *dev,void *buf,int size);
extern int hxd_ioport_write(HX_DEV *dev,const void *buf, int size);
extern int hxd_ioport_close(HX_DEV *dev);


#endif /* HX_UTILS_HXD_PARAM_H_ */
