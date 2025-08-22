#include "hx_board.h"
#include "hx_utils.h"
#include "stdio.h"

__weak void brd_iomode(int port,uint pin,IOMODE_T val)
{
	cpu_iomode(port,pin,val);
}
__weak uint brd_ioval(int port)
{
	return cpu_ioval(port);
}
__weak void brd_ioctrl(int port,uint pin,int val)
{
	cpu_ioctrl(port,pin,val);
}
__weak void brd_ioout(int port,uint val)
{
	cpu_ioout(port,val);
}
__weak void brd_ioclr(int port,uint pin)
{
	uint val = brd_ioval(port);
	val &= ~pin;
	brd_ioout(port,val);
}
__weak void brd_ioset(int port,uint pin)
{
	uint val = brd_ioval(port);
	val |= pin;
	brd_ioout(port,val);
}


static int gpio_open(const DEV_T *dev,const char *s)
{
	/* s-> "<00ff0000>,<0|1|2...>" */
	uint pin;
	int mode;
	if(sscanf(s,"%X,%d",&pin,&mode)==2){
		cpu_iomode(dev->devid,pin,(IOMODE_T)mode);
		return 0;
	}
	return -1;
}
static int gpio_read(const DEV_T *dev,void *buf,int size)
{
	*((uint32_t*)buf) = cpu_ioval(dev->devid);
	return 0;
}
static int gpio_write(const DEV_T *dev,const void *buf, int size)
{
	uint32_t val = *((uint32_t*)buf);
	cpu_ioout(dev->devid,val);
	return 0;
}
static int gpio_close(const DEV_T *dev)
{
	return gpio_open(dev,"FFFFFFFF,0");
}
const CDEV_DRV_T gpio_drv = {
	.open = gpio_open,
	.read = gpio_read,
	.write = gpio_write,
	.close = gpio_close,
};
static const DEV_T cdev_gpio_tbl[] = {
	{"GPIO0", 0,&gpio_drv},
	{"GPIO1", 1,&gpio_drv},
	{"GPIO2", 2,&gpio_drv},
};

////--------------------------------------------------------

//static int dummy;
//__weak const PARAM_ITEM_T g_param_tbl[] = {
//	{"dummy",		"%d",	&dummy},
//	//{"dev_sn",			"bcd 4",	Bus_Info_St.dev_sn},
//	//{"bus_no",			"bcd 4",	Bus_Info_St.BusNo},
//	//{"psam_no",			"bcd,6",	g_psam_nr},
//	//{"ip",				"%hhu 4 .",	ParamCardSt.IP},
//	//{"port",			"msb 2",	ParamCardSt.Port},
//};

//int param_open(const DEV_T *dev,const char *s)
//{
//	/* load params here */
//	return 0;
//}
//int param_close(const DEV_T *dev)
//{
//	
//	return 0;
//}

//static const CDEV_DRV_T param_drv = {
//	.open = param_open,
//	.close = param_close,
//};

//static const PARAM_DEV_T params = {
//	.dev = {"param",0,&param_drv},
//	.tbl = g_param_tbl,
//	.count = sizeof(g_param_tbl)/sizeof(g_param_tbl[0]),
//};
////----------------------------------------------------------------



//__weak int brd_init(void)
//{
//	HX_DBG_PRINTLN ("HX_UTILS:WARNING:board not config.");
//	hx_register_params(&params);
//	for(int i=0;i<sizeof(cdev_gpio_tbl)/sizeof(cdev_gpio_tbl[0]);
//		i++){
//		hx_register_char_device(&cdev_gpio_tbl[i]);
//	}
//	return 0;
//}

