
#include "psam.h"
#include "stdio.h"
#include "hx_device.h"

#define PSAM_SLOTS_MAX		(4)

#define PSAM_VCC	2,(1<<6)
#define PSAM_RST	2,(1<<2)
#define PSAM_CLK	0,(1<<20)
#define PSAM_DATA	2,(1<<11)


typedef struct {
	IO_t vcc;
	IO_t rst;
	IO_t clk;
	IO_t data;
} PsamSlot_t;
static PsamSlot_t PsamSlotTbl[PSAM_SLOTS_MAX] = {0};
static PsamSt g_psam[PSAM_SLOTS_MAX] = {0};
static int psam_open(HX_DEV *dev,const char *s)
{
	int res;
	int id = dev->pdev->devid;
	if(id>PSAM_SLOTS_MAX)
		return -1;
	const INF_T *pinf = hx_find_device(DT_PSAM_IF,id);
	if(!pinf)
		return -2;
	const IO_t *d;
	d = hx_interface_get_member(pinf,IF_GPIO_PSAM_VCC);
	if(d==NULL)
		return -3;
	PsamSlotTbl[id].vcc = *d;
	d = hx_interface_get_member(pinf,IF_GPIO_PSAM_RST);
	if(d==NULL)
		return -4;
	PsamSlotTbl[id].rst = *d;
	d = hx_interface_get_member(pinf,IF_GPIO_PSAM_CLK);
	if(d==NULL)
		return -5;
	PsamSlotTbl[id].clk = *d;
	d = hx_interface_get_member(pinf,IF_GPIO_PSAM_DATA);
	if(d==NULL)
		return -6;
	PsamSlotTbl[id].data = *d;
	
	g_psam[id].slot = id;
	g_psam[id].bps = PSAM_BPS_9600;
	g_psam[id].ucSmart_N = 0;
	HX_OS_LOCK();
	res = psam_init(&g_psam[id]);
	HX_OS_UNLOCK();
	return res;
}
static void *psam_buf;
static uint psam_bufzise;
static uint psam_rlen;
static ushort psam_rst;
/*
	HX_DEV *dev, int cmd, const uchar *buf, uint len, uchar *rbuf, *uint rlen)
*/
static int psam_ioctl(HX_DEV *dev,int cmd,va_list va)
{
	int res;
	int id = dev->pdev->devid;
	if(id>PSAM_SLOTS_MAX)
		return -1;
	if(cmd == IOCTL_RW_CMD){
		const uchar *buf = va_arg(va,const uchar*);
		uint len = va_arg(va,uint);
		uchar *rbuf = va_arg(va,uchar*);
		uint *rlen = va_arg(va,uint*);
		HX_OS_LOCK();
		res = psam_cmd(&g_psam[id],buf,len,rbuf,rlen);
		HX_OS_UNLOCK();
		return res;
	}else if(cmd == IOCTL_CFG_BPS){
		g_psam[id].bps = va_arg(va,uint);
		HX_OS_LOCK();
		res = psam_init(&g_psam[id]);
		HX_OS_UNLOCK();
		return res;
	}else if(cmd == IOCTL_CFG_PARAM){
		g_psam[id].ucSmart_N = va_arg(va,uint);
		HX_OS_LOCK();
		res = psam_init(&g_psam[id]);
		HX_OS_UNLOCK();
		return res;
	}else if(cmd == IOCTL_CFG_BUFFER){
		psam_buf = va_arg(va,void *);
		psam_bufzise = va_arg(va,uint);
		return 0;
	}else if(cmd == IOCTL_PSAM_RST){
		HX_OS_LOCK();
		res = psam_init(&g_psam[id]);
		HX_OS_UNLOCK();
		return res;
	}
	return -1;
}

int psam_write(HX_DEV *dev,const void *buf,int size)
{
	int id = dev->pdev->devid;
	if(id>PSAM_SLOTS_MAX)
		return -1;
	HX_OS_LOCK();
	psam_rst = psam_cmd(&g_psam[id],buf,size,psam_buf,&psam_rlen);
	HX_OS_UNLOCK();
	return size;
}
int psam_read(HX_DEV *dev,void *buf,int size)
{
	uchar *p = buf;
	if(psam_rlen){
		uint len = psam_rlen>size?size:psam_rlen;
		memcpy(p,psam_buf,len);
		p[len] = psam_rst>>8 & 0x0FFu;
		p[len+1] = psam_rst>>0 & 0x0FFu;
		return len+2;
	}
	return -1;
}

static int psam_close(HX_DEV *dev)
{
	int id = dev->pdev->devid;
	HX_OS_LOCK();
	psam_stop(&g_psam[id]);
	HX_OS_UNLOCK();
	return 0;
}
DEV_DRV_T g_psam_drv = {
	.open = psam_open,
	.ioctl = psam_ioctl,
	.close = psam_close,
	.read = psam_read,
	.write = psam_write,
};


extern unsigned int __get_sysclk(void);
#define _TCLK		(__get_sysclk())






extern void timer_init(void);
extern uint timer_getval(void);
extern void timer_setval(uint val);

#define iomode	mb_iomode
#define ioset	mb_ioset
#define ioclr	mb_ioclr
extern uint mb_ioval(uint port);
extern void iomode(uint port, uint pin, int mode);
int ioval(uint port,uint pin)
{
	return mb_ioval(port) & pin;
}

extern void ioset(uint port,uint pin);
extern void ioclr(uint port,uint pin);

static const PsamSlot_t *g_slot = NULL;

static void BcdHex2Asc(uchar *ucpIn,uchar *ucpOut,uint iInNum)
{
    uint i;
    uchar ucTmp;
    for(i=0; i<iInNum; i++)
    {
        ucTmp=(ucpIn[i] & 0xF0)>>4;
        if(ucTmp>=0x0A)
        {
            ucTmp -= 0x0A;
            ucpOut[i*2] = 'A' + ucTmp;
        }
        else
        {
            ucpOut[i*2]= ucTmp+'0';
        }
        ucTmp=(ucpIn[i] & 0x0F);
        if(ucTmp>=0x0A)
        {
            ucTmp -= 0x0A;
            ucpOut[i*2+1] = 'A' + ucTmp;
        }
        else
        {
            ucpOut[i*2+1]= ucTmp+'0';
        }
    }
}

static uchar MakeBCC(uchar * buffer,uint iBufLen)
{
    uint j;
    uchar ucXor=0;
    for(j=0; j<iBufLen; j++)
    {
        ucXor^=buffer[j];
    }
    return ucXor;
}

//=======================================================================



#define SMTCARD_PPSS		(0xFF)

static void timer_delay(uint tc)
{
	uint t;
    timer_setval(0);
	do{
		t = timer_getval();
	}while(tc>t);
}
static void delay_ms(uint ms)
{
    timer_delay(_TCLK/1000*ms);
}

static void delay_10us(uint _10us)
{
    timer_delay(_TCLK/100000*_10us);
}

//=======================================================
static void psam_vcc_ctrl(int val)
{
	IO_t p = g_slot->vcc;
	if(val)
		ioset(p.port,p.pin);
	else
		ioclr(p.port,p.pin);
}
static void psam_rst_ctrl(int val)
{
	IO_t p = g_slot->rst;
	if(val)
		ioset(p.port,p.pin);
	else
		ioclr(p.port,p.pin);
}
static void psam_clk_ctrl(int val)
{
	IO_t p = g_slot->clk;
	if(val)
		ioset(p.port,p.pin);
	else
		ioclr(p.port,p.pin);
}
static void psam_data_ctrl(int val)
{
    IO_t p = g_slot->data;
	if(val)
		ioset(p.port,p.pin);
	else
		ioclr(p.port,p.pin);
}
static int psam_data_getval(void)
{
	IO_t p = g_slot->data;
    return ioval(p.port,p.pin);
}
static void psam_data_dir(int dir)
{
	IO_t p = g_slot->data;
    iomode(p.port,p.pin,dir);
}

//======================================================

short tkSmartCardPosSet(ushort uiPos)
{
    return 0;
}

void psam_stop(PsamSt *psam)
{
    //
    //RST=L
    //CLK=L
    //VPP
    //I/O=A
    //VCC??
    psam_rst_ctrl(0);
    delay_10us(10);  //100us
    psam_clk_ctrl(0);
    delay_10us(10);  //100us
    psam_data_dir(0);
    delay_10us(10);  //100us
    //
    psam_vcc_ctrl(0);
    delay_10us(200);  //2000us
}
static uchar calc_parity(uchar _b)
{
	int i;
	uchar data = _b;
	uchar res=0;
    for(i=0; i<8; i++)
    {
		if((data>>i) & 0x01)
		{
			res++;
		}
	}
	res &= 0x01;
	return res;
}

static int psam_send_data(
	uint len,
	uchar *buf,
	uchar ucSmart_N,
	uint bps)
{
    uchar parity;   //?????

    //uchar ucTmp;
    uchar ucTrData;  //
    short i;
    short iCurrTrLen;
    uchar ucParityCnt;   //??3-5?

    //tkSmartCardTimerRst();
    psam_data_ctrl(1);
    psam_data_dir(1);
    ucParityCnt=0;
    iCurrTrLen=0;
	int tryout = 1000;
    while(iCurrTrLen<len && tryout-->0)
    {
        ucTrData=buf[iCurrTrLen];
		parity = calc_parity(ucTrData);
        psam_data_ctrl(0); // 1 start bit
        timer_delay(_TCLK/bps-_TCLK/1000000);
        for(i=0; i<8; i++)
        {
            psam_data_ctrl(ucTrData & 0x01);
            timer_delay(_TCLK/bps-_TCLK/1000000*2);
            ucTrData>>=1;
        }
        psam_data_ctrl(parity & 0x01);// 1 bit Parity
        timer_delay(_TCLK/bps-_TCLK/1000000*2);  // ???1us
        psam_data_ctrl(1);  // 1 bit stop
        timer_delay(_TCLK/bps-_TCLK/1000000);
        //??2etu?????,???????????????????
        //??I/O???Z
        psam_data_dir(0);
        //io? L ???? 5?
        if((psam_data_getval())==0)
        {
            // 3-5?????
            ucParityCnt++;
            if(ucParityCnt<5)
            {
                //?????????2?etu???
                psam_data_dir(1);
                psam_data_ctrl(1);
                timer_delay(_TCLK/bps);
            }
            else
            {
                // ?????
                psam_data_dir(1);
                psam_data_ctrl(1);
                return 1;
            }
        }
        else
        {
            iCurrTrLen++;
            ucParityCnt=0;
            psam_data_dir(1);
            psam_data_ctrl(1);
        }
        if(iCurrTrLen<len)
        {
            //?????????,????,?????
            timer_delay(_TCLK*3/2/bps);
            //????
            for(i=0; i<ucSmart_N; i++)
            {
                timer_delay(_TCLK/bps);
            }
        }

    }
    return 0;
}

static uchar psam_read_byte(uchar *ucOutData,uint bps)
{
    uchar ucSmartCardRcData=0;  // ??
    uchar ucSmartCardRcParity;  // ??
    int cLoop;
    for(cLoop=0; cLoop<8; cLoop++)
    {
        ucSmartCardRcData>>=1;
        if((psam_data_getval())!=0)
        {
            ucSmartCardRcData|=0x80;
        }
        else
        {
            ucSmartCardRcData&=(~0x80);
        }
        timer_delay(_TCLK/bps-_TCLK/800000);  // 20140804 _TCLK/1000000->_TCLK/800000
    }
    if((psam_data_getval())!=0)
    {
        ucSmartCardRcParity=1;
    }
    else
    {
        ucSmartCardRcParity=0;
    }
    *ucOutData=ucSmartCardRcData;
    return ucSmartCardRcParity;
}


static short psam_reset_head(uchar * ucpRcDataBuf,uint bps)
{
    uchar ucRcData;  // ????
    uchar parity;   //?????
    uchar ucTmp;

    //short i;
    int tryout = 10000;
    while(tryout-->0)
    {
		
        timer_setval(0);
        //ulTmp=TIME_COUNTER_D;
        do
        {
            //???????400-40000???????
            // 3.579  Fcplk=28800000 ??321828 11.17ms
            //20000=11.18ms
			//30ms???
            if(timer_getval()>(_TCLK/1000*30))//921828)   // 20140506 ?????????????
            {
                return 10;
            }

        } while((psam_data_getval())!=0);

        // ulTmp=TIME_COUNTER_D;
        timer_delay(3*_TCLK/bps/2-_TCLK/1000000);   //delay 1.5 baud time

        //??
        parity=psam_read_byte( & ucRcData,bps);

		//delay_f(bps/2);
        timer_delay(_TCLK/bps/2);   //delay 1 baud time

        //????
        ucTmp = calc_parity(ucRcData);

        //timer out 4.8us 280cycles every cycles=0.017361107us
        if(ucTmp==parity)
        {
            //????
            *ucpRcDataBuf=ucRcData;
            timer_delay(_TCLK/bps);   //delay 1 baud time
            return 0;
        }
        else
        {
            //????
            psam_data_dir(1);
            psam_data_ctrl(0);
            // 1-2?etu????
            timer_delay(_TCLK/bps);   //delay 1 baud time
            psam_data_dir(0);
        }
    }
	return -1;
}

static short psam_recv_data(short iRcLen,uchar * ucpRcDataBuf,uint bps)
{
    uchar parity;   //?????
 //   short i;
    short iCurrRcLen;

    //uint ulTmp;
    //uint ulTmp2;
    uchar ucTmp;
    uchar ucParityCnt=0;   //??3-5?
    uchar ucRcData;
    uint ulStarDelay;
    if(bps>PSAM_BPS_9600)
    {
        ulStarDelay=9600;
    }
    else
    {
        ulStarDelay=4800;
    }
    psam_data_dir(0);
    //for(iCurrRcLen=0;iCurrRcLen<iRcLen;iCurrRcLen++)
    iCurrRcLen=0;
    while(iCurrRcLen<iRcLen)
    {
//			ulTIMER3_COUNTER=0;
//			T3TCR = 1;	//enable_timer
//			do
//			{
//				//???????400-40000???????
//				//??????12-9600etu????
//				if(ulTIMER3_COUNTER>50)
//				{
//					T3TCR = 0; //disable timer
//					return 1;
//				}
//			}while((psam_data_getval)!=0);
//			T3TCR = 0; //disable timer
        //ulTmp2=401000;   //250ms  9600 ?etu
        timer_setval(0);
        do
        {
            //???????400-40000???????
            //??????12-9600etu????
            //WDTFeed();
            if(timer_getval()>_TCLK/bps*ulStarDelay)
            {
                return 10;
            }
        } while((psam_data_getval())!=0);


        timer_delay(3*_TCLK/bps/2-_TCLK/1000000);   //delay 1.5 baud time
        // ??????? 1.5? etu?? ??
        parity=psam_read_byte( & ucRcData,bps);
        // ?? 1?etu
        timer_delay(_TCLK/bps-4*_TCLK/1000000);   //delay 1 baud time
        ucTmp=calc_parity(ucRcData);

        if(ucTmp==parity)
        {
            //????
            *ucpRcDataBuf=ucRcData;
            ucpRcDataBuf++;
            iCurrRcLen++;
            //timer_delay(_TCLK/bps);   //delay 1 baud time
            if(iCurrRcLen<iRcLen)
            {
                timer_delay(_TCLK/bps);   //delay 1 baud time
            }
            else
            {
                timer_delay(2*_TCLK/3/bps);   //delay 1 baud time
            }
        }
        else
        {
            // begin by tiger 2013-07-30
            if(ucParityCnt++>100)
            {
                return 20;
            }
            // end by tiger 2013-07-30
            //???? 10.5 + - 0.2etu??????  ??1etu ??2etu
            psam_data_dir(1);
            psam_data_ctrl(0);
            // 1-2?etu????
            timer_delay(3*_TCLK/bps/2);   //delay 1 baud time
            psam_data_dir(0);
        }
    }
    psam_data_ctrl(1);
    return 0;
}


short psam_consult(PsamSt *psam,uint bps, uchar *rbuf)
{
	const uint tbl[] = {
		11,9600,
		12,19200,
		13,38400,
		18,115200,
		94,55800,
		38,57600
	};
	uchar ucFiDi = 11;
	for(int i=0;i<6;i++){
		if(bps<=tbl[i*2+1])
			ucFiDi = tbl[i*2];
	}
        //??11 12 13 18 94 38 :
		//9600 19200 38400 115200 55800 57600
    const ushort FI_Fi_Table[]= {372,372,558,744,1116,1488,1860,0
                                    ,0,512,768,1024,1536,2048,0,0
                                   };  //0=rfu
    const ushort DI_Di_Table[]= {0,1,2,4,8,16,32,0
                                    ,12,20,0,0,0,0,0,0
                                   };  //0=rfu

//	uint ulTmp;
    uint ulCurrBond;
    short i;
    short iRetVal;
    short iProcess;
    short iTmp;
    short iRcLen;
    uchar ucCurrFi;
    uchar ucCurrDi;
    uchar ucEndFi;
    uchar ucEndDi;

    uchar ucTmp;
    uchar ucTrBuf[20];
    uchar ucRcBuf[20];
    uchar ucTABCD_BITMAP;   //TD1TC1TB1TA1TD2TC2TB2TA2
    uchar ucTA1;
//    uchar ucTB1;
    uchar ucTC1;
    uchar ucTD1;
    uchar ucTA2;
//    uchar ucTB2;
//    uchar ucTC2;
//	uchar ucTD2;
//		uchar ucSmartCard_PPSS;
//		//pps0 ??b5,b6,b7????1???????PPS1,PPS2,PPS3 ?????
//		//?b4-b1????T???????????b8=0??
//		uchar ucSmartCard_PPS0;
//		//pps1 ??IFD????F?D????TA1?????????
//		uchar ucSmartCard_PPS1;
//		uchar ucSmartCard_PPS2;
//		uchar ucSmartCard_PPS3;
//		uchar ucSmartCard_PCK;
    ucTABCD_BITMAP=0;
    psam->ucSmart_N=0;
    iTmp=2;
    //T0 ucSMTCARD_RESET_DATA[1]
    if(rbuf[1]&0x10)
    {
        //TA1
        ucTABCD_BITMAP|=0x10;
        ucTA1=rbuf[iTmp];
        ucEndFi=((ucTA1&0xf0)>>4);
        ucEndDi=(ucTA1&0x0f);
        if(ucEndFi>0x0d)
        {
            return 0;
        }
        if(ucEndDi>0x09)
        {
            return 0;
        }

        iTmp++;
    }
    if(rbuf[1]&0x20)
    {
        //TB1
        ucTABCD_BITMAP|=0x20;
//        ucTB1=rbuf[iTmp];
        iTmp++;
    }
    if(rbuf[1]&0x40)
    {
        //TC1
        ucTABCD_BITMAP|=0x40;
        ucTC1=rbuf[iTmp];
        if(ucTC1<0xff)
        {
            psam->ucSmart_N=ucTC1;
        }
        iTmp++;
    }
    if(rbuf[1]&0x80)
    {
        //TD1
        ucTABCD_BITMAP|=0x80;
        ucTD1=rbuf[iTmp];
        iTmp++;
        if(ucTD1&0x10)
        {
            //TA2
            ucTABCD_BITMAP|=0x01;
            ucTA2=rbuf[iTmp];
            iTmp++;
        }
        if(ucTD1&0x20)
        {
            //TB2
            ucTABCD_BITMAP|=0x02;
//            ucTB2=rbuf[iTmp];
            iTmp++;
        }
        if(ucTD1&0x40)
        {
            //TC2
            ucTABCD_BITMAP|=0x04;
//            ucTC2=rbuf[iTmp];
            iTmp++;
        }
    }
    if(ucTABCD_BITMAP&0x01)
    {
        //TA2 ?? ????
        if(!(ucTA2&0x10))
        {
            //??????20111101
            ulCurrBond=PSAM_CLK_FREQ*DI_Di_Table[ucEndDi];
            ulCurrBond/=FI_Fi_Table[ucEndFi];
            psam->bps=ulCurrBond;
        }
        return 0;
    }
    // ??TA1 ????????
    if(!(ucTABCD_BITMAP&0x10))
    {
        return 0;
    }


    //ATR???????
    timer_delay(_TCLK/PSAM_BPS_9600);
    /* begin by tiger 2013-11-20
    	tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
    	tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
    */
    //????
    for(i=0; i<psam->ucSmart_N; i++)
    {
        timer_delay(_TCLK/PSAM_BPS_9600);
        /* begin by tiger 2013-11-20
        		tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
        		tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
        */
    }

    iProcess=100;
    ucCurrFi=1;
    ucCurrDi=1;
    while(1)
    {
        switch(iProcess)
        {
        case 100:
            ucCurrFi=ucEndFi;
            ucCurrDi=ucEndDi;
            ucCurrFi=((ucFiDi&0xf0)>>4);
            ucCurrDi=(ucFiDi&0x0f);
            iProcess=1000;
            break;
        case 200:
            if(ucCurrDi<=ucEndDi)
            {
                ucCurrDi++;
                ucCurrFi=1;
            }
            else
            {
                //????
                iProcess=9000;
            }
            break;
        case 1000:
            ucTrBuf[0]=SMTCARD_PPSS;  //ppss
            ucTrBuf[1]=0x70;   //pps0
            ucTmp=ucCurrFi<<4;
            ucTrBuf[2]=ucCurrDi&0x0f;
            ucTrBuf[2]|=ucTmp;  //pps1
            ucTrBuf[3]=0;	//pps2
            ucTrBuf[4]=0;  //pps3
            ucTrBuf[5]=MakeBCC(ucTrBuf,5);  //pck
            iRetVal=psam_send_data(6,ucTrBuf,psam->ucSmart_N,PSAM_BPS_9600);
            if(iRetVal==0)
            {
                iProcess=1100;
            }
            else
            {
                iProcess=9000;
            }
            break;
        case 1100:
            iRcLen=6;
            iRetVal=psam_recv_data(iRcLen,ucRcBuf,PSAM_BPS_9600);
            if(iRetVal==0)
            {
                iProcess=1200;
            }
            else
            {
                if(iRcLen>1)
                {
                    iProcess=1200;
                }
                else
                {
                    iProcess=9000;
                }
            }
            break;
        case 1200:

            if(ucRcBuf[0]!=SMTCARD_PPSS)
            {
                iProcess=9000;
                break;
            }
            if(ucRcBuf[1]&0x10)  //b5
            {
                if(ucRcBuf[2]!=ucTrBuf[2])
                {
                    iProcess=9000;
                    break;
                }
            }
            else
            {
                //pps1 ???,???Fd?Dd
                iProcess=10000;
                break;
            }
            iTmp=2;
            if(ucRcBuf[1]&0x20)  //b6
            {
                iTmp++;
                if(ucRcBuf[3]!=ucTrBuf[3])
                {
                    iProcess=9000;
                    break;
                }
            }
            if(ucRcBuf[1]&0x40)  //b7
            {
                iTmp++;
                if(ucRcBuf[4]!=ucTrBuf[4])
                {
                    iProcess=9000;
                    break;
                }
            }
            //??????
            if(iRcLen<iTmp)
            {
                iProcess=9000;
                break;
            }
            //?????????
            ulCurrBond=PSAM_CLK_FREQ*DI_Di_Table[ucCurrDi];
            ulCurrBond/=FI_Fi_Table[ucCurrFi];
            psam->bps=ulCurrBond;
            iProcess=10000;
            break;
        case 9000:
            return 1;
        //break;
        case 10000:
            return 0;
        //break;
        default:
            break;
        }
    }
}


static int psam_reset_info(PsamSt *psam, uchar *rbuf, uint *rlen)
{
    uchar buf[64];
    uchar *p = buf;
    int have_next = 0;
    uchar n;
    uint len;
    int res;
    memset(buf,0,sizeof(buf));
    *rlen = 0;
    res=psam_reset_head(p++,psam->bps);
    if(res)
        return res;
    res=psam_recv_data(1,p++,psam->bps);
    if(res)
        return res;
    do {
        uchar d;
        n = *(p-1);
        have_next = n&0x80;
        d =n&0xF0;
        while(d) {
            if(d&0x80) {
                res=psam_recv_data(1,p,psam->bps);
                if(res)
                    return res;
                p++;
            }
            d<<=1;
        }
    } while(have_next);
    res=psam_recv_data((n&0x0f),p,psam->bps);
    if(res)
        return res;
    p+=n&0x0f;
    len = p-buf;
    *rlen = len;
    memcpy(rbuf,buf,len);
    return 0;
}
int psam_reset_cold(PsamSt *psam, uchar *rbuf, uint *rlen)
{
    int res;
    psam_stop(psam);	//????

    psam_vcc_ctrl(1);
    delay_10us(10);  //100us
    psam_clk_ctrl(1);
    psam_data_dir(0);
    delay_10us(40);   // 400us
    psam_rst_ctrl(1);  //??clk?,rst=L??400?????
    //RST=H???400-40000??????????,?????40000?????
    //?????,?RST???????L,????

    //??ts??
    res=psam_reset_info(psam,rbuf,rlen);
    return res;
}

short psam_reset_warm(PsamSt *psam, uchar *rbuf, uint *rlen)
{
    short iRetVal;
    //
    psam_rst_ctrl(0);  //RST=L??400?????
    delay_10us(40);  //360us
    psam_rst_ctrl(1);  //??clk?,rst=L??400?????
    //RST=H???400-40000??????????,?????40000?????
    //?????,?RST???????L,????

    //??ts??
    iRetVal=psam_reset_info(psam,rbuf,rlen);
    return iRetVal;
}



static ushort _psam_cmd(PsamSt *psam,
                const uchar *cmd, uint len,
                uchar *rbuf, uint *rlen
               )
{
	int trys;
    ushort i;
    ushort res;
    uchar ucTrBuf[200];
    uchar Lc, Le;
    uchar BPrc;
    Lc=0;
    Le=0;
	*rlen = 0;
    memcpy(ucTrBuf,cmd,len);
    ucTrBuf[len]=0;
    if(len>5){
        Lc=ucTrBuf[4];
        Le=ucTrBuf[5+Lc];
    }else{
		Lc=0;
        Le=ucTrBuf[4];
    }
    //Send CLA, INS, P1, P2, 0/Le/Lc 
    if(psam_send_data(5,ucTrBuf,psam->ucSmart_N,psam->bps))
		return 0xFFFF;
	//get respons
	trys = 16;
	do{
		if(psam_recv_data(1, &BPrc,psam->bps))
			return 0xFFFE;
	}while(BPrc==0x60 && trys-->=0);		//wait
	if(trys<0)
		return 0xFFFD;
	//error return
	if(	(BPrc!=ucTrBuf[1]) && (BPrc!=(~(ucTrBuf[1])))){
		res=BPrc;
        if(psam_recv_data(1, & BPrc,psam->bps))
			return 0xFFFC;
		res<<=8;
        res|=BPrc;
		*rlen=0;
		return res;	//60 6c ??
	}
	if(Lc){
		//send data
		for(i=0; i<psam->ucSmart_N+2; i++){
			timer_delay(_TCLK/psam->bps);  // delay baud time
			/* begin by tiger 2013-11-18
							tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
							tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
			*/
		}
		if(psam_send_data(Lc,ucTrBuf+5,psam->ucSmart_N,psam->bps))
			return 0xFFFB;
	}
	//get sw
	if(psam_recv_data(Le+2,rbuf,psam->bps))
			return 0xFFFA;
	res=rbuf[Le];
	res<<=8;
	res|=rbuf[Le+1];
	*rlen=Le;
	return res;
}
ushort psam_cmd(PsamSt *psam,
                const uchar *cmd, uint len,
                uchar *rbuf, uint *rlen
               )
{
	g_slot = &PsamSlotTbl[psam->slot];
	ushort res;
	int trys = 3;
	do{
		res = _psam_cmd(psam,cmd,len,rbuf,rlen);
	}while((res&0xFF00u)==0x6C00 && trys-->=0);
	if(trys<0)
		return 0xFFF9;
	if((res&0xFF00u)==0x6100){
		uchar buf[16];
		buf[0]=0x00;
		buf[1]=0xC0;
		buf[2]=0x00;
		buf[3]=0x00;
		buf[4]=res&0x0FFu;
		buf[5]=0;
		for(int i=0; i<psam->ucSmart_N+2; i++)
		{
			timer_delay(_TCLK/psam->bps);  // delay baud time
			/* begin by tiger 2013-11-18
							tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
							tkSmartCardDelay(ulSMTCARD_DELAY3); //delay half baud time  300=27.8us
			*/
		}
		res = _psam_cmd(psam,buf,5,rbuf,rlen);
	}
	return res;
}


static void psam_init_clk_port(uint slot)
{
	timer_init();
	if(!g_slot)
		return;
	
	iomode(g_slot->data.port,g_slot->data.pin,0);
	iomode(g_slot->clk.port,g_slot->clk.pin,1);
	iomode(g_slot->vcc.port,g_slot->vcc.pin,1);
	iomode(g_slot->rst.port,g_slot->rst.pin,1);
	
	
//	while(1){
//	
//		int val = ioval(g_slot->data.port,g_slot->data.pin);
//		if(val){
//			hx_dbg(0,"H\n");
//		}else{
//			hx_dbg(0,"L\n");
//		}
//		hx_delay_ms(500);
//	}
//	
//	ioclr(g_slot->data.port,g_slot->data.pin);
//	ioclr(g_slot->clk.port,g_slot->clk.pin);
//	ioclr(g_slot->vcc.port,g_slot->vcc.pin);
//	ioclr(g_slot->rst.port,g_slot->rst.pin);
//	
//	ioset(g_slot->data.port,g_slot->data.pin);
//	ioset(g_slot->clk.port,g_slot->clk.pin);
//	ioset(g_slot->vcc.port,g_slot->vcc.pin);
//	ioset(g_slot->rst.port,g_slot->rst.pin);
	
}

int psam_init(PsamSt *psam)
{
    int res;
    uint rlen;
	uchar buf[512] = {0};
    uchar rbuf[256] = {0};
	
	g_slot = &PsamSlotTbl[psam->slot];
	
    psam_init_clk_port(psam->slot);
    psam->ucSmart_N=0;
    res = psam_reset_cold(psam,rbuf,&rlen);
    if(res)
        return -1;
	BcdHex2Asc(rbuf,buf,rlen);
    hx_dbg(0,"len=%u buf=%s\n",rlen,buf);
    if(psam->bps == 9600)
    {
		res = psam_consult(psam,38400,rbuf);
		if(res)
			return 0;
    }
    return 0;
}


void main22(void)
{
	

	uchar rbuf[512],buf[512];
	uint rlen;
    ushort rc;
    PsamSt psam = {0,9600};
    psam_init(&psam);
	hx_dbg(0,"cmd\n");
    rc = psam_cmd(&psam,"\x00\x84\x00\x00\x08",5,rbuf,&rlen);
    if(rc==0x9000) {
        BcdHex2Asc(rbuf,buf,rlen);
        hx_dbg(0,"rc=%#4X len=%u buf=%s\n",rc,rlen,buf);

    } else {

        hx_dbg(0,"error rc=%#4X\n",rc);
    }

    //while(1);

}

//ushort hx_cmd(HX_DEV *d,const uchar *cmd, uint len,
//                uchar *rbuf, uint *rlen
//               )
//{
//	if(hx_write(d,cmd,len)!=len)
//		return 0xFFEF;
//	*rlen = hx_read(d,rbuf,256+2);
//	if(*rlen>2)
//		*rlen-=2;
//	return HX_MSB_B2W(rbuf
//	
//}


