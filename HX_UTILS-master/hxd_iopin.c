/*
 * hxd_iopin.c
 *
 *  Created on: 2016Äê11ÔÂ17ÈÕ
 *      Author: houxd
 */
#include "stdio.h"
#include "hx_utils.h"
#include "hxd_iopin.h"
#include "string.h"



/*
s : mode value whit asc
s == NULL ,not change io direct
s == "0"|"1"|... is 0|1|... for mode value
*/
int hxd_iopin_open(HX_DEV *dev,const char *s)
{
	/* s-> "1" or "0" ... is mode value */
	const DEV_T *pdev = dev->pdev;
	const IOPIN_DRV_T *drv = (const IOPIN_DRV_T *)pdev->driver;
	uint id = pdev->devid;
	uint board =  (id>>12) & 0xFFu;
	uint port = (id>>8) & 0xFFu;
	uint pin = id & 0xFFu;
	if(board>=drv->tbl_count)
		return -1;
	int mode = 1;
	if(s && s[0])
		mode = s[0] - '0';
	else
		return 0;
	drv->iotbl[board]->iomode(port,(1<<pin),mode);
	return 0;
}
/*
	buf[0] is val whit hex
*/
int hxd_iopin_read(HX_DEV *dev,void *buf,int size)
{
	const DEV_T *pdev = dev->pdev;
	const IOPIN_DRV_T *drv = (const IOPIN_DRV_T *)pdev->driver;
	uint id = pdev->devid;
	uint board =  (id>>12) & 0xFFu;
	uint port = (id>>8) & 0xFFu;
	uint pin = id & 0xFFu;
	uint val;
	if(board>=drv->tbl_count)
		return -1;
	val = drv->iotbl[board]->ioval(port);
	char *p = (char*)buf;
	if(val & (1<<pin))
		p[0] = 0x01;
	else
		p[0] = 0x00;
	if(id & PIN_ATTR_INVERSE)
		p[0] = !(p[0]);
	return 1;
}
int hxd_iopin_write(HX_DEV *dev,const void *buf, int size)
{
	const DEV_T *pdev = dev->pdev;
	const IOPIN_DRV_T *drv = (const IOPIN_DRV_T *)pdev->driver;
	uint id = pdev->devid;
	uint board =  (id>>12) & 0x0Fu;
	uint port = (id>>8) & 0x0Fu;
	uint pin = id & 0xFFu;
	char *p = (char*)buf;
	int set = p[0];
	uint val;
	if(id & PIN_ATTR_INVERSE)
		set = !set;
	
	HX_DBG_PRINTLN("iopin write:board=%u,port=%u,pin=%u,set=%u",
		board,port,pin,set);
	
	if(board>=drv->tbl_count)
		return -1;
	val = drv->iotbl[board]->ioval(port);
	if(set)
		val |= (1<<pin);
	else
		val &= ~(1<<pin);
	drv->iotbl[board]->ioctrl(port,val);
	return 1;
}
int hxd_iopin_close(HX_DEV *dev)
{
	return hxd_iopin_open(dev,"0");
}

