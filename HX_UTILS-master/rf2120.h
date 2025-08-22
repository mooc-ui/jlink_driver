#ifndef RF2120_H
#define RF2120_H

#ifndef IOCTL_RW_CMD
#define IOCTL_RW_CMD				(1)
#endif

#define IOCRL_RC_IccPowerOn			(2)
#define IOCRL_RC_IccPowerOff		(3)


extern DEV_DRV_T g_carddev_drv;


#endif
