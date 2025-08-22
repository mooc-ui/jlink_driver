/*
 * hxd_param.h
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
 *      Author: houxd
 */

#ifndef HX_UTILS_HXD_PARAM_H_
#define HX_UTILS_HXD_PARAM_H_

#include "hx_device.h"


typedef struct __PARAM_ITEM_T {
	const char *name;
	const char *data_type;
	// <type>,<count>[,<spilt-chars>]
	// "bin","bcd", "asc", "i32","i64","f32","f64"
	// bin,2	AA55		"\xAA\x55"
	// bcd,2,- "11-22"		"\x11\x22"
	// asc,0	"houxd"		"houxd"
	// asc,3	"abc"		"abc"
	// i32		100			100
	// ...
	void *data;
} PARAM_ITEM_T;


/*
	params device instance
*/
typedef struct __PARAM_DEV_T{
	/*
		must be extends DEV_T
	*/
	DEV_T dev;
	/*
		private propertys
	*/
	const PARAM_ITEM_T *tbl;
	int count;
} PARAM_DEV_T;

typedef struct __PARAM_DRIVER_T {
	DEV_DRV_T cdrv;
	int (*load)(void);
	int (*store)(void);
}PARAM_DRV_T;


/*
 * inner call
 */
extern int hxd_param_open(HX_DEV *dev,const char *param);
extern int hxd_param_read(HX_DEV *dev,void *buf,int _size);
extern int hxd_param_write(HX_DEV *dev,const void *buf, int size);
extern int hxd_param_close(HX_DEV *dev);

/*
 * .cdev = ...
 */
#define CDRV_PARAM_STD_DRIVER 	\
{								\
	.open = hxd_param_open,		\
	.read = hxd_param_read,		\
	.write = hxd_param_write,	\
	.close = hxd_param_close,	\
}

#define hx_register_params(param_dev) 	\
	hx_register_device((DEV_T*)param_dev);

#endif /* HX_UTILS_HXD_PARAM_H_ */
