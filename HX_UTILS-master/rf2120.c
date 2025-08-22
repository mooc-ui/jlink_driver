#include "psam.h"
#include "stdio.h"
#include "hx_device.h"
#include "rf2120.h"

static HX_DEV rf2120_uart;

#define PC_to_RDR_IccPowerOn	(0x62)
#define PC_to_RDR_IccPowerOff	(0x63)
#define PC_to_RDR_XfrBlock		(0x6F)
#define RDR_to_PC_DataBlock		(0x80)
#define RDR_to_PC_SlotStatus	(0x81)

typedef __packed struct __RDR_Header
{
	uint8_t bMessageType;
	uint32_t dwLength;
	uint8_t bSlot;
	uint8_t bSeq;
	uint8_t data[3];
} RDR_Header;

#define ushort	unsigned short

static const ushort fcstab[] = { 
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, 
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, 
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, 
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 
};
static ushort calc_crc(const unsigned char *data, int length)
{
	ushort fcs = 0xFFFF;

	int end = length;

	for (int i = 0; i < end; i++)
	{
		fcs = (ushort)(((ushort)(fcs >> 8)) ^ fcstab[(fcs ^ data[i]) & 0xFF]);
	}

	return (ushort)(~fcs);
}

#define UART1_1516		(1)
#define hx_uart_putc(id,c)				hxl_putc(&rf2120_uart,c)
#define hx_uart_getc_timeout(id,pc,t)	hxl_getc_timeout(&rf2120_uart,pc,t)
#define hx_uart_send(id,s,l)			hxl_send(&rf2120_uart,s,l)
#define hx_uart_rxclr(id)				hxl_rxclr(&rf2120_uart)
#define xprintf(...)					hx_dbg(0,__VA_ARGS__)

#define APDU_DATA_SIZE		(600)

int _rf1516_frame(uint8_t *txbuf,int txlen,uint8_t *rxbuf,int *rxlen, uint8_t *rcmd, uint8_t *rstatus)
{
	uint8_t tmp[APDU_DATA_SIZE+20];
	uint8_t *p = tmp;
	#warning max of frame need
	int res;
	int c;
	uint16_t crc;
	
	*rxlen = 0;
	*rcmd = 0xFF;
	*rstatus = 0xFF;
	
	if(!txbuf || txlen<=0 || !rxbuf || !rxlen)
		return -99;
	
	HX_MSB_W2B(3+txlen,p);	//len
	p+=2;
	*p++ = 0x30;			//cmd  30 ccid data  FA test  FF error
	memcpy(p,txbuf,txlen);
	p+=txlen;
	crc = calc_crc(tmp,p-tmp);
	HX_MSB_W2B(crc,p);		//crc
	p+=2;
	*p++ = 0xCC;		//end with 0xcc
	
	int trys = 4;
	do{
		hx_uart_putc(UART1_1516,0xAA);
		again:
		res = hx_uart_getc_timeout(UART1_1516,&c,500);
		if(res)
			continue;
		if(c!=0xDD)
			goto again;
		break;
	}while(trys-->0);
	if(trys<=0)
		return -998;
	
	hx_uart_send(UART1_1516,(const char*)tmp,p-tmp);

	char buf[512];
	hx_dbg(0,">>%s\n",hx_dumphex2str(tmp,p-tmp,buf));
	
	//recv
	p = tmp;
	res = hx_uart_getc_timeout(UART1_1516,&c,100);	//0xBB
	if(res)
		return -2;
	if(c!=0xBB)
		return -3;
	res = hx_uart_getc_timeout(UART1_1516,&c,100);	//l1
	if(res)
		return -3;
	*p++ = c;
	res = hx_uart_getc_timeout(UART1_1516,&c,5);	//l2
	if(res)
		return -4;
	*p++ = c;
	res = hx_uart_getc_timeout(UART1_1516,&c,5);	//cmd
	if(res)
		return -5;
	*rcmd = c;
	*p++ = c;
	res = hx_uart_getc_timeout(UART1_1516,&c,5);	//status
	if(res)
		return -6;
	*rstatus = c;
	*p++ = c;
	
	uint16_t len = HX_MSB_B2W(tmp);
	//uint8_t cmd = tmp[2];
	len -=4;
	*rxlen = len;
	for(int i=0;i<len;i++){
		res = hx_uart_getc_timeout(UART1_1516,&c,5);		//data
		if(res)
			return -5;
		*p++ = c;
	}
	res = hx_uart_getc_timeout(UART1_1516,&c,5);			//crc1
	if(res)
		return -6;
	*p++ = c;
	res = hx_uart_getc_timeout(UART1_1516,&c,5);			//crc2
	if(res)
		return -7;
	*p++ = c;
	res = hx_uart_getc_timeout(UART1_1516,&c,5);			//end 0xCC
	if(res)
		return -8;
	if(c!=0xCC)
		return -9;
	*p++ = c;
	crc = calc_crc(tmp,p-tmp-3);
	if(crc != HX_MSB_B2W(p-3))
	{
		xprintf("crc check error : calc crc=0x%04X get 0x%04X\n",
			(int)crc,(int)HX_MSB_B2W(p-3));
		return -0xA;
	}
	
	*rxlen = len;
	if(len)
	memcpy(rxbuf,tmp+4,len);
	*rcmd = tmp[2];
	*rstatus = tmp[3];
	
	hx_dbg(0,"<<%s\n",hx_dumphex2str(rxbuf,len,buf));
	return 0;
}

int rf1516_frame(uint8_t *txbuf,int txlen,uint8_t *rxbuf,int *rxlen)
{
	int res;
	//char tb[1024];
	uint8_t cmd,status;
	//xprintf("%s: tx %u Byte: %s\n",__FUNCTION__,txlen,hx_dumphex2str(txbuf,txlen,tb));
	hx_uart_rxclr(UART1_1516);
	res = _rf1516_frame(txbuf,txlen,rxbuf,rxlen,&cmd,&status);
	//xprintf("recv all data in one frame\n");
	//xprintf("%s: return 0x%X. cmd=%X status=%X rx %u Byte: %s\n",
	//	__FUNCTION__,(-res),cmd,status,*rxlen,hx_dumphex2str(rxbuf,*rxlen,tb));
	return res;
}
int rf1516_IccPowerOn(void)
{
	const RDR_Header tx = {PC_to_RDR_IccPowerOn,0,0,0,"\x00\x00\x00"};
	uint8_t buff[APDU_DATA_SIZE];
	int reslen = 0;
	int res = rf1516_frame((void*)&tx,10,buff,&reslen);
	if(res)
		return res;
	if(reslen<10)
		return res;
	RDR_Header *p = (void*)buff;
	if(p->bMessageType == RDR_to_PC_DataBlock)
		return 0;
	return -1;
}
int rf1516_IccPowerOff(void)
{
	const RDR_Header tx = {PC_to_RDR_IccPowerOff,0,0,0,"\x00\x00\x00"};
	uint8_t buff[APDU_DATA_SIZE];
	int reslen = 0;
	int res = rf1516_frame((void*)&tx,10,buff,&reslen);
	if(res)
		return res;
	if(reslen<10)
		return res;
	RDR_Header *p = (void*)buff;
	if(p->bMessageType == RDR_to_PC_SlotStatus)
		return 0;
	return -1;
}

int rf1516_XfrBlock(const void *apdu, int len,void *rbuf,int *rlen)
{
	RDR_Header tx = {PC_to_RDR_XfrBlock,0,0,0,"\x00\x00\x00"};
	tx.dwLength = len;
	uint8_t buff[APDU_DATA_SIZE];
	memcpy(buff,&tx,10);
	memcpy(buff+10,apdu,len);
	int reslen = 0;
	int res = rf1516_frame(buff,10+len,buff,&reslen);
	if(res)
		return res;
	if(reslen<10)
		return res;
	RDR_Header *p = (void*)buff;
	if(p->bMessageType != RDR_to_PC_DataBlock)
		return -1;
	unsigned rl = p->dwLength;
	if(rl && rl<APDU_DATA_SIZE){
		memcpy(rbuf,buff+10,rl);
		*rlen = rl;
		return 0;
	}
	return -2;
}
uint16_t rf1516_apdu(const void *apdu, int len,void *rbuf,int *rlen)
{
	int res;
	res = rf1516_XfrBlock(apdu,len,rbuf,rlen);
	if(res)
		return res;
	*rlen = *rlen - 2;
	res = HX_MSB_B2W(((uint8_t*)rbuf)+(*rlen));
	//char tb[128];
	xprintf("apdu return 0x%X\n",res);
	return res;
}
uint16_t rf1516_apdu2(const char *cmd,void *rbuf,int *rlen)
{	
	char buf[APDU_DATA_SIZE];
	int l = strlen(cmd);
	//l = l>>1<<1;
	hx_hexcode2bin(cmd,l,buf);
	return rf1516_apdu(buf,l/2,rbuf,rlen);
}

static int open(HX_DEV *dev,const char *s)
{
	int res;
	res = hx_open("u1_1516","",&rf2120_uart);
	if(res)
		return res;
	HX_OS_LOCK();
	res = rf1516_IccPowerOn();
	HX_OS_UNLOCK();
	return res;
}

static int ioctl(HX_DEV *dev,int cmd,va_list va)
{
	int res;
	if(cmd == IOCTL_RW_CMD){
		const uchar *buf = va_arg(va,const uchar*);
		uint len = va_arg(va,uint);
		uchar *rbuf = va_arg(va,uchar*);
		int *rlen = va_arg(va,int*);
		HX_OS_LOCK();
		res = rf1516_apdu(buf,len,rbuf,rlen);
		HX_OS_UNLOCK();
		return res;
	}else if(cmd==IOCRL_RC_IccPowerOn){
		return rf1516_IccPowerOn();
	}else if(cmd==IOCRL_RC_IccPowerOff){
		return rf1516_IccPowerOff();
	}
	return -1;
}

static int close(HX_DEV *dev)
{
	int res;
	HX_OS_LOCK();
	res = rf1516_IccPowerOff();
	HX_OS_UNLOCK();
	return res;
}
DEV_DRV_T g_carddev_drv = {
	.open = open,
	.ioctl = ioctl,
	.close = close,
};



