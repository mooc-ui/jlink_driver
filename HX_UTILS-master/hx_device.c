/*
 * hx_device.c
 *
 *  Created on: 2016Äê7ÔÂ25ÈÕ
 *      Author: houxd
 */

#include "hx_utils.h"
#include "stdio.h"
#include "string.h"
#include "stddef.h"

HX_DEVTBL_INIT()

static const DEV_T *devtbl[MAX_DEVICE_COUNT] = {0};

const DEV_T **hx_dev_next(const DEV_T** pdev)
{
	if(pdev==NULL)
		return devtbl;
	else if(pdev>=&devtbl[0] && pdev<&devtbl[MAX_DEVICE_COUNT-1])
		return pdev+1;
	else if(pdev==&devtbl[MAX_DEVICE_COUNT-1])
		return (const DEV_T **)DEVTBL_BASE();
	else if(pdev>=(const DEV_T **)DEVTBL_BASE() 
		&& pdev<((const DEV_T **)DEVTBL_LIMIT())-1)
		return pdev+1;
	else
		return NULL;
}

const void *hx_find_device(int devtype,int devid)
{
	const DEV_T **p = hx_dev_next(NULL);
	do{
		if(p && *p){
			if((*p)->devtype==devtype && (*p)->devid==devid)
				return *p;
		}
		p = hx_dev_next(p);
	}while(p);
	return NULL;
}
const void *hx_interface_get_member(const INF_T *pinf,int m_label)
{
	int count = pinf->count;
	for(int i=0;i<count;i++){
		if(pinf->members[i].label == m_label)
			return pinf->members[i].dev;
	}
	return 0;
}
int hx_register_device(const DEV_T *dev)
{
	//int pos = hx_devtbl_count();
	for(int i=0;i<MAX_DEVICE_COUNT;i++){
		if(!devtbl[i]
			|| strcmp(devtbl[i]->name,dev->name)==0){
			devtbl[i] = dev;
			return 0;
		}
	}
	return -1;
}
/*
	int pos = hx_devtbl_count();
	if(pos==MAX_DEVICE_COUNT)
		return pos;
	devtbl[pos].dev = dev;
	devtbl[pos].dev_type = type;
	return 0;
*/
//--------------------------------------------------------------
static FILE* __stdin_for_out = NULL;
static int stdio_open(HX_DEV *dev,const char *s)
{
	if(dev->pdev->devid == 0){
		__stdin_for_out = fopen("g","a+");
	}
	return 0;
}
static int stdio_read(HX_DEV *dev,void *buf,int size)
{
	int id = dev->pdev->devid;
	FILE *stdtbl[] = {stdin,stdout,stderr};
	return fread(buf,1,size,stdtbl[id]);
}
static int stdio_write(HX_DEV *dev,const void *buf, int size)
{
	FILE * stdtbl[] = {stdin,stdout,stderr};
	int res;
	int id = dev->pdev->devid;
	if(id==0 && __stdin_for_out){
		res = fwrite(buf,1,size,__stdin_for_out);
		fflush(stdtbl[id]);
	}else{
		res = fwrite(buf,1,size,stdtbl[id]);
		fflush(stdtbl[id]);
	}
	return res;
}
static int stdio_close(HX_DEV *dev)
{
	return 0;
}
static const DEV_DRV_T stdio_drv = {
	.open = stdio_open,
	.read = stdio_read,
	.write = stdio_write,
	.close = stdio_close,
};

//----------------------------------------------------------------
static const DEV_T cdev_stdin = {"stdin", 0,&stdio_drv};
static const DEV_T cdev_stdout = {"stdout",1,&stdio_drv};
static const DEV_T cdev_stderr = {"stderr",2,&stdio_drv};

HX_DEV hx_stdin,hx_stdout,hx_stderr;

void hx_device_init(void)
{
	hx_register_device(&cdev_stdin);
	hx_register_device(&cdev_stdout);
	hx_register_device(&cdev_stderr);
	
	hx_open("stdin","",&hx_stdin);
	hx_open("stdout","",&hx_stdout);
	hx_open("stderr","",&hx_stderr);
}

//--------------------------------------------------------------------

int hx_open(const char *dev_name,const char *param, HX_DEV* dev)
{
	if(!dev) return -1;
	const DEV_T **pdev = NULL; 
	do{
		pdev = hx_dev_next(pdev);
		if(pdev && *pdev && strcmp((*pdev)->name,dev_name)==0){
			dev->pdev = *pdev;
			const DEV_DRV_T *driver = (const DEV_DRV_T*)((*pdev)->driver);
			#ifdef HXUTILS_RTOS_SURPPORT
			if(dev->pdev->opencntp==NULL || *(dev->pdev->opencntp)==0)
				HX_MUTEX_INIT(dev->mutex);
			#endif
			if(driver && driver->open)
				return driver->open(dev,param);
			return 0;
		}
	}while(pdev);
	return -1;
}
int hx_read(HX_DEV *dev,void *buf,int size)
{
	int res = -1;
	const DEV_DRV_T *driver;
	if(!dev) 
		return res;
	HX_MUTEX_WAIT(dev->mutex);
	if(!dev->pdev)
		goto end;
	driver = (DEV_DRV_T*)(dev->pdev->driver);
	if(driver && driver->read)
		res = driver->read(dev,buf,size);
	else
		res = 0;
end:
	HX_MUTEX_RELEASE(dev->mutex);
	return res;
}

int hx_ioctl(HX_DEV *dev,int cmd,...)
{
	int res = -1;
	const DEV_DRV_T *driver;
	if(!dev)
		return res;
	HX_MUTEX_WAIT(dev->mutex);
	if(!dev->pdev)
		goto end;
	va_list va;
	va_start(va,cmd);
	driver = (DEV_DRV_T*)(dev->pdev->driver);
	if(driver && driver->ioctl)
		res = driver->ioctl(dev,cmd,va);
	else
		res = 0;
	va_end(va);
end:
	HX_MUTEX_RELEASE(dev->mutex);
	return res;
}
int hx_flush(HX_DEV *dev)
{
	return hx_write(dev,NULL,0);
}
int hx_write(HX_DEV *dev,const void *buf,int size)
{
	int res = -1;
	const DEV_DRV_T *driver;
	if(!dev)
		return res;
	HX_MUTEX_WAIT(dev->mutex);
	if(!dev->pdev)
		goto end;
	driver = (DEV_DRV_T*)(dev->pdev->driver);
	if(driver && driver->write)
		res = driver->write(dev,buf,size);
	else 
		res = 0;
end:
	HX_MUTEX_RELEASE(dev->mutex);
	return res;	
}
int hx_close(HX_DEV *dev)
{
	int res = -1;
	const DEV_DRV_T *driver;
	if(!dev) 
		return res;
	HX_MUTEX_WAIT(dev->mutex);
	if(!dev->pdev)
		goto end;
	driver = (DEV_DRV_T*)(dev->pdev->driver);
	if(driver && driver->close)
		res = driver->close(dev);
	else
		res = 0;
	memset(dev,0,sizeof(*dev));
end:
	HX_MUTEX_RELEASE(dev->mutex);
	HX_MUTEX_UNINIT(dev->mutex);
	return res;	
}

