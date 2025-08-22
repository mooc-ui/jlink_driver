#ifndef __PSAM_H__
#define __PSAM_H__

#include "stdint.h"
#include "string.h"
#include "hx_device.h"
#include "hx_utils.h"

#ifndef uint
#define uint 	unsigned int
#define ushort	unsigned short
#define uchar	unsigned char
#endif

#define IOCTL_RW_CMD				(1)
//ioctl(HX_DEV *dev, int cmd, const uchar *buf, uint len, uchar *rbuf, *uint rlen)
#define IOCTL_CFG_BPS				(2)
#define IOCTL_CFG_PARAM				(3)
//ioctl(HX_DEV *dev, int cmd, uint );


#define IF_GPIO_PSAM_VCC		1
#define IF_GPIO_PSAM_RST		2
#define IF_GPIO_PSAM_CLK		3
#define IF_GPIO_PSAM_DATA		4

#define PSAM_CLK_FREQ				(3579545)
#define PSAM_BPS_9600		(PSAM_CLK_FREQ/372)
#define PSAM_BPS_19200		(PSAM_CLK_FREQ*2/372)
#define PSAM_BPS_38400		(PSAM_CLK_FREQ*4/372)
#define PSAM_BPS_56000		(PSAM_CLK_FREQ*8/512)
#define PSAM_BPS_57600		(PSAM_CLK_FREQ*12/744)
#define PSAM_BPS_115200		(PSAM_CLK_FREQ*12/372)


	
typedef struct  {
	uint slot;
	uint bps;
	uint ucSmart_N;
} PsamSt;

extern int psam_init(PsamSt *psam);
extern void psam_stop(PsamSt *psam);
//extern short psam_consult(PsamSt *psam,uint bps, uchar *rbuf);
//extern int psam_reset_cold(PsamSt *psam, uchar *rbuf, uint *rlen);
extern short psam_reset_warm(PsamSt *psam, uchar *rbuf, uint *rlen);
extern ushort psam_cmd(PsamSt *psam,
                const uchar *cmd, uint len,
                uchar *rbuf, uint *rlen
               );

#endif

