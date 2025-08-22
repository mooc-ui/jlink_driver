/*
 * hxd_atc.h
 *
 *  Created on: 2016Äê11ÔÂ14ÈÕ
 *      Author: houxd
 */

#ifndef HXD_ATC_H__
#define HXD_ATC_H__


#include "hx_device.h"
#include "stdint.h"
#include "hxl_serial.h"

#define IOCTL_AT_POLL					(1)
#define IOCTL_AT_NIC_CFG				(2)
//ioctl(int ip[4],int port,int lcport,int *sockid)
#define IOCTL_AT_SOCK_CONNECT			(3)
//ioctl(int *sockid)
#define IOCTL_AT_SOCK_DISCONNECT		(4)
#define IOCTL_AT_NIC_CHECKSELF			(5)

//ioctl(NIC_CB_T *cb);
#define IOCTL_AT_CFG_CALLBACK			(6)


//ioctl(HX_DEV *dev,int cmd)

/*
	usefor call back function, indicate a event.
*/
enum ATEVENT_T {AT_PUT,AT_GET};

/*
	define cmd list item struct
	if cmd set NULL, this case will call callback fuction,
		and send AT_PUT event.
	if want_res set NULL, send AT_GET event.
	callback :
		step current step
		event
		buf when event==AT_PUT,that use for return a cmd
			when event==AT_GET,that use for get res of at revc.
		msg use for user defined data struct . set NULL. if no use
		return value 0 will complete current step. others are not.
*/
struct ATCMD_T
{
    const char *cmd;
    const char *want_res;
    int tmout_at_once;
    int trytimes;
    int (*callback)(
        int step,
        enum ATEVENT_T event,
        char *buf,
        void *msg);
	const char *err_inf;
};


struct HX_NIC_T
{
	const struct NET_PARAM_T *default_param;
	const struct ATCMD_T *at_tbl;
	const int at_tbl_count;
	int (*init)(const struct HX_NIC_T *this);
	int (*reset)(const struct HX_NIC_T *this);
	int (*read)(const struct HX_NIC_T *this,void *buf,int _size);
	int (*write)(const struct HX_NIC_T *this,const void *buf, int size);

	/*
	 * belows proc is blocked, use for rtos
	 */
	int step_of_endinit;
	int (*nic_check_self)(const struct HX_NIC_T *this);
	int (*sock_connect)(const struct HX_NIC_T *this
			,uint8_t ip[4],uint16_t port, uint16_t lc_port, int *sockid);
	int (*sock_disconnect)(const struct HX_NIC_T *this, int *sockid);
	
	int (*net_config)(const struct HX_NIC_T *this);
	
	void *prv;
};

struct NET_PARAM_T {
	int step;
	char apn[32];
	uint8_t rm_ip[4];
	uint16_t rm_port;
	uint8_t lc_ip[4];
	
	int dhcp_en;
	uint8_t mask[4];
	uint8_t gateway[4];
	
	//wifi
	char w_ssid[33];
	int w_secu_en;
	char w_psk[33];
	int w_dhcp_en;
	uint8_t w_lc_ip[4]; 
	uint8_t w_lc_mask[4]; 
	uint8_t w_lc_gw[4]; 
	uint8_t w_lc_dsn1[4]; 
	uint8_t w_lc_dsn2[4]; 
	
	
};

/*
	atc device instance
*/
typedef struct __ATC_DEV_T{
	/*
		must be extends DEV_T
	*/
	DEV_T dev;
	/*
		private propertys
	*/
	struct NET_PARAM_T *param;
	const struct HX_NIC_T *nic;
	
} ATC_DEV_T;

typedef void NIC_CB_T(const struct ATCMD_T *pat);


///=====================================
// AT Export APIs
/*
	arguements:
	nic	:	net card struct
	pstep : instans of current step
	arg : nic configuration
	return : errno ,0 is success, others are bad
*/
extern int atc_poll(const struct HX_NIC_T *nic, int *pstep, struct NET_PARAM_T *param);
extern int atc_getc_noblock(int *pc);
extern int atc_getc_timeout(int *pc,int t);
extern int atc_getc_block(int *pc);
extern int atc_gets_noblock(char *bf,int bl);
extern int atc_gets_timeout(char *bf,int bl,int t);
extern int atc_gets_timeout_prefix(char *bf,int bl,int t,char *pfx);
extern int atc_gets_timeout_match(char *bf,int bl,int t,
		const GETS_MATCH_T *tbl, int tblsize);
extern void atc_gets_block(char *bf, int bl);
extern void atc_rxclr(void);
extern void atc_putc(int c);
extern void atc_send(const char *d,int l);
extern void atc_put(const  char *s);
extern void atc_puts(const  char *s);
extern int atc_printf(const char *format, ...);
extern void atc_flush(void);

#define ATCMD_DELAY(ms)	 	{"DELAY",NULL,ms,-1,NULL,}

/*
*/
extern void nic_pwr(int level);
extern void nic_rst(int level);

///=====================================
/// NICs
extern const struct HX_NIC_T nic_sim800c;
extern const struct HX_NIC_T nic_sim7100c;
extern const struct HX_NIC_T nic_me3630_pid_c1a;
extern const struct HX_NIC_T nic_me3630_pid_c1b;
extern const struct HX_NIC_T nic_mg3732;
extern const struct HX_NIC_T nic_ne4110s;
extern const struct HX_NIC_T nic_sim7600c;
extern const struct HX_NIC_T nic_rak415;
extern const struct HX_NIC_T nic_485emu;

//common param instance, 
//use self define to support multy nic in one process
extern struct NET_PARAM_T g_net_param;
//driver for use
extern const DEV_DRV_T hx_atc_drv;


extern HX_DEV g_at_uart;

#endif



