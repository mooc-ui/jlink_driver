#ifndef __HX_DEVICE_H__
#define __HX_DEVICE_H__

#include "hx_rtos_surpport.h"
#include "stdarg.h"
#include "stdint.h"

#pragma anon_unions

#define MAX_DEVICE_COUNT				(16)

#define IOCTL_CFG_BUFFER			(-1)
#define IOCTL_PSAM_RST				(9)
//ioctl(HX_DEV *dev, int cmd, void *buf, uint bufsize);

typedef struct __DEV_DRV_T DRIVER_T;

/*
	describe io
*/
typedef struct {
	uint32_t port;
	uint32_t pin;
} IO_t;

/*
	device type 
*/
typedef enum {
	DT_UNKNOWN = 0,	
	
	DT_CHAR,		//normal char device.
	DT_STREAM,		//like uart debug info devices.
	DT_CHAR_CMD,	//send and recive by single frame.and return extra int result
	
	DT_BLOCK,		//normal block device.
	
	DT_INTERFACE,	//seldefine or standard interface.
	DT_PSAM_IF,
	DT_ATC_IF,
} DEVTYPE_T;

/*
	device instance
*/
typedef struct __DEV_T{
	const char *name;
	int devid;	
	const void *driver;
	DEVTYPE_T devtype;
#ifdef HXUTILS_RTOS_SURPPORT
	int *opencntp;		//device open count 
#endif
} DEV_T;

/*
	interface instance
*/
typedef struct __HX_IF_MEMBER{
	int label;
	const void *dev;
} HX_IFMBR;
typedef struct __HX_INTERFACE {
	const char *name;
	int devid;	
	DRIVER_T *driver;
	DEVTYPE_T devtype;
	int count;
	const HX_IFMBR *members;
} INF_T;

//externs use, instance of opened device
typedef struct __HX_DEVICE {
	int type;		//type and flags
	int offset;
	union {
		const DEV_T *pdev;
		const INF_T *pinf;
	};
	HX_MUTEX_T mutex;
} HX_DEV;

/*
	device driver 
*/
typedef struct __DEV_DRV_T{
	/*
	id	: device instans identifation
	s	: special params string for device	
	return : success is 0, others is error
	*/
	int (*open)(HX_DEV *dev,const char *s);
	/*
	buf : buffer of read out.
	size : 	>0 length of wangt read no-block. 
	return: >0 count of read chars
			<=0 is error
	*/
	int (*read)(HX_DEV *dev,void *buf,int size);
	/*
	size :	>0 length of want write
	return :>0 is success,others is error
	*/
	int (*write)(HX_DEV *dev,const void *buf,int size);
	/*
	return : >=0 is write bytes success,others is error
	*/
	int (*close)(HX_DEV *dev);
	/*
		normal return 0;
		-0xFF is no this call
		-1 is not surpport cmd
	*/
	int (*ioctl)(HX_DEV *dev,int cmd,va_list va);
	
} DEV_DRV_T;


/*
	stdard input/output
*/
extern HX_DEV hx_stdin,hx_stdout,hx_stderr;
#define hxin	(&hx_stdin)
#define hxout	(&hx_stdout)
#define hxerr	(&hx_stderr)

/*
	export calls
*/
extern int hx_open(const char *dev_name,const char *param, HX_DEV* dev);
extern int hx_read(HX_DEV *dev,void *buf,int size);
extern int hx_write(HX_DEV *dev,const void *buf,int size);
extern int hx_close(HX_DEV *dev);
extern int hx_ioctl(HX_DEV *dev,int cmd,...);
extern int hx_flush(HX_DEV *dev);

extern const DEV_T **hx_dev_next(const DEV_T** pdev);
extern const DEV_T **hx_get_devtbl(void);
//extern int hx_devtbl_count(void);
//extern void hx_device_init(void);
extern const void *hx_find_device(int devtype,int devid);

/*
	BSL layer calls
*/
extern int hx_register_device(const DEV_T *dev);
extern const void *hx_interface_get_member(const INF_T *pinf,int m_label);

extern void hx_device_init(void);

//=======================================================================
/* import device table session vars */
extern unsigned int 
	HX_DEVTBL_SECTION$$Base,
	HX_DEVTBL_SECTION$$Limit,
	HX_DEVTBL_SECTION$$Length;
#define DEVTBL_BASE()	((unsigned int)&HX_DEVTBL_SECTION$$Base)
#define DEVTBL_LIMIT()	((unsigned int)&HX_DEVTBL_SECTION$$Limit)
#define DEVTBL_LEN()	((unsigned int)&HX_DEVTBL_SECTION$$Length)
	
/* register a extend device on rom */
#define HX_REGISTER_DEVICE(dev) \
	static const DEV_T* const __pdev_##dev \
		__attribute__((section("HX_DEVTBL_SECTION"))) = (const DEV_T*)&(dev); 

#define HX_REGISTER_DEVICE_WEAK(dev)	\
	const DEV_T* const __pdev_##dev \
		__attribute__((section("HX_DEVTBL_SECTION"),weak)) = (const DEV_T*)&(dev); 
	
/* 
	register a simple HX device on rom.
	name : const char * 
	id : int
	driver : const DRIVER_T *
	_type : DEVTYPE_T 
*/
#define HX_REGISTER_DEVICE_S(name,id,driver,_type)	\
	static const DEV_T __dev_##name = {#name,id,driver,_type}; \
	HX_REGISTER_DEVICE(__dev_##name)
	
#define HX_REGISTER_DEVICE_WEAK_S(name,id,driver,_type)	\
	const DEV_T __dev_##name __attribute__((weak)) = {#name,id,driver};\
	HX_REGISTER_DEVICE_WEAK(&(__dev_##name))
	
/* register a null device use for create the device table session */
#define HX_DEVTBL_INIT()	\
	HX_REGISTER_DEVICE_S(__null_dev,0,NULL,DT_CHAR)

#endif
