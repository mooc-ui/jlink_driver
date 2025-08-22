///*
// * hx_atcmd.h
// *
// *  Created on: 2016Äê8ÔÂ15ÈÕ
// *      Author: houxd
// */

//#ifndef __AT_PORT_H__
//#define __AT_PORT_H__

//#include "int.h"
//#include "hx_device.h"
//#include "hxd_atc.h"

//#ifdef MULTY_NIC_CARD
//typedef struct __HX_ATARG_T {
//    char 	rm_ip[16]; 
//	uint16_t 	rm_port;
//	union {
//		struct __GPRS_WCDMA_ETC{
//			char apn[16];
//			char user[16];
//			char passwd[32];
//		};
//		struct __WIFI {
//			char w_ssid[32];
//			char w_passwd[32];
//			
//			struct __LAN {
//				char lc_ip[16];
//				char mask[16];
//				char gateway[16];
//				int dhcp_en;
//			}
//		}
//	}
//} HX_ATARG_T;

//typedef struct __HX_NIC_INFO {
//	char sn[10];
//	char firm_ver[10];
//	char module_name[20];
//	char mac[20];
//} HX_NIC_INFO_T;
//#else
//typedef struct __HX_ATARG_T {
//    char 	rm_ip[16]; 
//	uint16_t 	rm_port;
//	
//    char apn[16];
//    char user[16];
//    char passwd[32];

//    char w_ssid[32];
//    char w_passwd[32];

//    char lc_ip[16];
//    char mask[16];
//    char gateway[16];
//    int dhcp_en;
//} HX_ATARG_T;

//typedef struct __HX_NIC_INFO {
//	char sn[10];
//	char firm_ver[10];
//	char module_name[20];
//	char mac[20];
//} HX_NIC_INFO_T;
//#endif

///*
//	usefor call back function, indicate a event.
//*/
//enum ATEVENT_T {AT_PUT,AT_GET};

///*
//	define cmd list item struct
//	if cmd set NULL, this case will call callback fuction,
//		and send AT_PUT event.
//	if want_res set NULL, send AT_GET event.
//	callback :
//		step current step
//		event
//		buf when event==AT_PUT,that use for return a cmd
//			when event==AT_GET,that use for get res of at revc.
//		msg use for user defined data struct . set NULL. if no use
//		return value 0 will complete current step. others are not.
//*/
//struct ATCMD_T
//{
//    const char *cmd;
//    const char *want_res;
//    int tmout_at_once;
//    int trytimes;
//    int (*callback)(
//        int step,
//        enum ATEVENT_T event,
//        char *buf,
//        void *msg);
//};

//struct HX_NIC_T
//{
//	const struct NET_PARAM_T *default_param;
//	const struct ATCMD_T *at_tbl;
//	const int at_tbl_count;
//	int (*init)(const struct HX_NIC_T *this);
//	int (*reset)(const struct HX_NIC_T *this);
//};

/////=====================================
//// AT Inner APIs
//#define ATCMD_DELAY(ms)	 	{"DELAY",NULL,ms,-1,NULL,}
//extern 
//int atc_default_init(
//	const struct HX_NIC_T *this, 
//	int *pstep, 
//	HX_ATARG_T *arg
//);
//extern HX_NIC_INFO_T g_nic_info;

/////=====================================
//// AT Export APIs
///*
//	arguements:
//	nic	:	net card struct
//	pstep : instans of current step
//	arg : nic configuration
//	return : errno ,0 is success, others are bad
//*/
//extern int atc_init(const struct HX_NIC_T *nic, int *pstep, HX_ATARG_T *arg);
//extern int atc_init2(const struct HX_NIC_T *nic, int *pstep, HX_DEV *dev);
///*
//	arguements:
//	nic	:	net card struct
//	pstep : instans of current step
//	msg : private data here
//	return : current step

//	note:
//	normal return number is the current step.
//	when all step continued , return -1
//*/
//extern int atc_poll(const struct HX_NIC_T *nic, int *pstep, struct NET_PARAM_T *param);

//extern void atc_reset(const struct HX_NIC_T *nic);


//extern int atc_getc_noblock(int *pc);
//extern int atc_getc_timeout(int *pc,int t);
//extern int atc_getc_block(int *pc);
//extern int atc_gets_noblock(char *bf,int bl);
//extern int atc_gets_timeout(char *bf,int bl,int t);
//extern void atc_gets_block(char *bf, int bl);
//extern void atc_rxclr(void);
//extern void atc_putc(int c);
//extern void atc_send(const char *d,int l);
//extern void atc_put(const  char *s);
//extern void atc_puts(const  char *s);
//extern int atc_printf(const char *format, ...);


///////=====================================
/////// NICs
////extern const struct HX_NIC_T nic_sim800c;
////extern const struct HX_NIC_T nic_sim7100c;
////extern const struct HX_NIC_T nic_me3630;
////extern const struct HX_NIC_T nic_mg3732;
////extern const struct HX_NIC_T nic_ne4110s;
////extern const struct HX_NIC_T nic_sim7600c;


//#endif

