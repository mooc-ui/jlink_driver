#include "hx_utils.h"
#include "stdio.h"
#include "hxd_uart.h"
#include "hxd_param.h"
#include "hxd_atc.h"
#include "../board/brd_dq1200.h"
#include "../cpu/cpu_lpc2378.h"
#include "hxd_iopin.h"
#include "hxd_ioport.h" 
#include "string.h"

HX_DEV g_u0_com;

extern unsigned int __get_fpclk(void);

//-----------------------------------------------------------------
void mb_iomode(uint port,uint pin,uint mode)
{
	cpu_iomode(port,pin,(IOMODE_T)mode);
}
void mb_ioctrl(uint port,uint val)
{
	cpu_ioctrl(port,val);
}
uint mb_ioval(uint port)
{
	return cpu_ioval(port);
}
static const struct IODESC_T mbio = {mb_iomode,mb_ioctrl,mb_ioval};

//-----------------------------------------------------------------
void sb_iomode(uint port,uint pin,uint mode)
{
}
void sb_ioctrl(uint port,uint val)
{
}

uint sb_ioval(uint port)
{
	return 0;
}

static const struct IODESC_T sbio = {sb_iomode,sb_ioctrl,sb_ioval};

//-----------------------------------------------------------------
static const struct IODESC_T *g_iotbl[] = {&mbio,&sbio};

static const IOPORT_DRV_T gpio_drv = {
	.drv = HX_IOPORT_STD_DRIVER,
	.iodesc = &mbio,
};
static const DEV_T cdev_gpio_tbl[] = {
	{"p0", 0,(const DEV_DRV_T*)&gpio_drv},
	{"p1", 1,(const DEV_DRV_T*)&gpio_drv},
	{"p2", 2,(const DEV_DRV_T*)&gpio_drv},
};

static const IOPORT_DRV_T sbio_drv = {
	.drv = HX_IOPORT_STD_DRIVER,
	.iodesc = &sbio,
};
static const DEV_T cdev_sbio = {"sbio0", 0,(const DEV_DRV_T*)&sbio_drv,};

//-----------------------------------------------------------------
static const IOPIN_DRV_T iopin_drv = {
	.drv = HX_IOPIN_STD_DRIVER,
	.iotbl = g_iotbl,
	.tbl_count = sizeof(g_iotbl)/sizeof(g_iotbl[0]),
};
static const DEV_T cdev_at_io_pwr = 
	{"at_io_pwr",_IOPIN_ID(0,0,20),(const DEV_DRV_T*)&iopin_drv};
static const DEV_T cdev_at_io_rst = 
	{"at_io_rst",_IOPIN_ID(0,0,21),(const DEV_DRV_T*)&iopin_drv};
	
	
//--------------------------------------------------------
extern volatile unsigned __g_cpu_tickcount;
static unsigned int __fpclk;
static const PARAM_ITEM_T g_param_tbl[] = {
	{"clock",		"%u",	(void*)&__g_cpu_tickcount},
	{"pclk",		"%u",	&__fpclk},
};

static int param_load(void)
{
	__fpclk = __get_fpclk();
	return 0;
}
static int param_store(void)
{
	return 0;
}

static const PARAM_DRV_T param_drv = {
	.cdrv = CDRV_PARAM_STD_DRIVER,
	.load = param_load,
	.store = param_store,
};

const PARAM_DEV_T brd_params = {
	.dev = {"param",0,&param_drv},
	.tbl = g_param_tbl,
	.count = sizeof(g_param_tbl)/sizeof(g_param_tbl[0]),
};



//---------------------------------------------------------

#define RS485_RD	PORT1,(1<<29)

#define RS485_CFG_RECV()	cpu_ioclr(RS485_RD)
#define RS485_CFG_SEND()	cpu_ioset(RS485_RD)

/*
	must enable tx interrupt.
*/
static int uart_tx_start_byte(const UART_DEV_T *dev,int data)
{
	int id = dev->dev.devid;
	if(id==3){
		RS485_CFG_SEND();
	}
	cpu_uart_tx_start_byte(id,data);
	return 0;
}	
static int uart_tx_end_session(const UART_DEV_T *dev)
{
	int id = dev->dev.devid;
	cpu_uart_tx_end_session(id);
	if(id==3){
		RS485_CFG_RECV();
	}
	return 0;
}
static int uart_open(const UART_DEV_T *dev, int bps)
{
	int id = dev->dev.devid;
	//uart3 is 485
	if(id==3){
		cpu_iomode(RS485_RD,IM_OUT);
		RS485_CFG_RECV();
	}
	return cpu_uart_reopen(id,bps,__get_fpclk());
}
static int uart_close(const UART_DEV_T *dev)
{
	return 0;
}
static int uart_interrupt_ctrl(const UART_DEV_T *dev,int en)
{
	cpu_interrupt_ctrl(en);
	return 0;
}
static const UART_DRV_T uart_drv = {
	.cdrv = CDRV_UART_STD_DRIVER,
	.tx_start_byte = uart_tx_start_byte,
	.tx_end_session = uart_tx_end_session,
	.interrupt_ctrl = uart_interrupt_ctrl,
	.open = uart_open,
	.close = uart_close,
};
static UART_PRIVATE_T uart0_prv;
char uart0_txbuf[1024],uart0_rxbuf[1024];
static UART_PRIVATE_T uart1_prv;
char uart1_txbuf[1024],uart1_rxbuf[1024];
static UART_PRIVATE_T uart2_prv;
char uart2_txbuf[1024],uart2_rxbuf[1024];
static UART_PRIVATE_T uart3_prv;
char uart3_txbuf[1024],uart3_rxbuf[1024];
static const UART_DEV_T cdev_uart0 = {
	.dev = {"u0_com",0,&uart_drv},
	.txbuf = uart0_txbuf,
	.rxbuf = uart0_rxbuf,
	.txbuf_size = 128,
	.rxbuf_size = 128,
	.default_bps = 115200,
	.prv = &uart0_prv,
};
static const UART_DEV_T cdev_uart1 = {
	.dev = {"uart1",1,&uart_drv},
	.txbuf = uart1_txbuf,
	.rxbuf = uart1_rxbuf,
	.txbuf_size = 1024,
	.rxbuf_size = 1024,
	.default_bps = 115200,
	.prv = &uart1_prv,
};
static const UART_DEV_T cdev_uart2 = {
	.dev = {"uart2",2,&uart_drv},
	.txbuf = uart2_txbuf,
	.rxbuf = uart2_rxbuf,
	.txbuf_size = 1024,
	.rxbuf_size = 1024,
	.default_bps = 115200,
	.prv = &uart2_prv,
};
static const UART_DEV_T cdev_uart3 = {
	.dev = {"u3_485",3,&uart_drv},
	.txbuf = uart3_txbuf,
	.rxbuf = uart3_rxbuf,
	.txbuf_size = 1024,
	.rxbuf_size = 1024,
	.default_bps = 57600,
	.prv = &uart3_prv,
};

static const UART_DEV_T cdev_at_uart = {
	.dev = {"at_uart",1,&uart_drv},
	.txbuf = uart1_txbuf,
	.rxbuf = uart1_rxbuf,
	.txbuf_size = 1024,
	.rxbuf_size = 1024,
	.default_bps = 115200,
	.prv = &uart1_prv,
};

const ATC_DEV_T g_cdev_sim800c = {
	.dev = {"sim800c",0,&hx_atc_drv},
	.param = &g_net_param,
	.nic = &nic_sim800c,
};

const ATC_DEV_T g_cdev_sim7600c = {
	.dev = {"sim7600c",0,&hx_atc_drv},
	.param = &g_net_param,
	.nic = &nic_sim7600c,
};

const ATC_DEV_T g_cdev_me3630 = {
	.dev = {"me3630",0,&hx_atc_drv},
	.param = &g_net_param,
	.nic = &nic_me3630_pid_c1b,
};

int brd_init(void)
{
	HX_DBG_PRINTLN ("BOARD:dq1200");
	cpu_init_tickcount_1m_by_pclk(__get_fpclk());
	for(int i=0;i<sizeof(cdev_gpio_tbl)/sizeof(cdev_gpio_tbl[0]);
		i++){
		hx_register_device(&cdev_gpio_tbl[i]);
	}
	hx_register_uart_device(&cdev_uart0);
	hx_register_uart_device(&cdev_uart1);
	hx_register_uart_device(&cdev_uart2);
	hx_register_uart_device(&cdev_uart3);
	hx_register_device(&cdev_sbio);
	
	hx_register_uart_device(&cdev_at_uart);
	
	//sim800c
	hx_register_device(&cdev_at_io_pwr);
	hx_register_device(&cdev_at_io_rst);
	
	hx_register_device((DEV_T*)&g_cdev_sim800c);
	hx_register_device((DEV_T*)&g_cdev_sim7600c);
	hx_register_device((DEV_T*)&g_cdev_me3630);
	
	return 0;
}



void brd_uart_rx_byte(int id, int data)
{
	if(id==0)
		hxd_uart_rx_byte(&cdev_uart0,data);
	else if(id==1)
		hxd_uart_rx_byte(&cdev_uart1,data);
	else if(id==2)
		hxd_uart_rx_byte(&cdev_uart2,data);
	else if(id==3)
		hxd_uart_rx_byte(&cdev_uart3,data);
	else
		fprintf(stderr,"warn:%s:id=%d\n",__FUNCTION__,id);
}
void brd_uart_tx_byte(int id)
{
	if(id==0)
		hxd_uart_tx_byte(&cdev_uart0);
	else if(id==1)
		hxd_uart_tx_byte(&cdev_uart1);
	else if(id==2)
		hxd_uart_tx_byte(&cdev_uart2);
	else if(id==3)
		hxd_uart_tx_byte(&cdev_uart3);
	else
		fprintf(stderr,"warn:%s:id=%d\n",__FUNCTION__,id);
}

