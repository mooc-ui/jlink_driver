#include "hx_utils.h"
#include "string.h"
#include "stdio.h"
#include "hxd_atc.h"

static int config_ap(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	struct NET_PARAM_T *p = msg;
	sprintf(buf,"AT+CIPCSGP=1,\"%s\",\"\",\"\"",p->apn);
	return 0;
}

static int check_csq(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	//+CSQ: %d,%d
	if(buf[0] == '+'){
		int a=0,b=0;
		sscanf(buf,"+CSQ: %d,%d",&a,&b);
		if(a>0)
			return 0;
	}
	return -1;
}
static int check_connect(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	if(event==AT_PUT){
		struct NET_PARAM_T *p = msg;
		sprintf(buf,"AT+CIPSTART=\"TCP\",\"%u.%u.%u.%u\",\"%u\"",
			(unsigned int)(p->rm_ip[0]),
			(unsigned int)(p->rm_ip[1]),
			(unsigned int)(p->rm_ip[2]),
			(unsigned int)(p->rm_ip[3]),
			(unsigned int)(p->rm_port));
		return 0;
	}else if(event == AT_GET){
		if(strstr(buf,"FAIL"))	//CONNECT FAIL
			return -1;
		if(strstr(buf,"ERROR"))	//CONNECT ERROR
			return -2;			
		if(strstr(buf,"CONNECT"))	//CONNECT 115200 / ALREADY CONNECT etc..
			return 0;
		return -1;
	}
	return -1;
}

static const struct ATCMD_T at_tbl[] = {
	//cmd					res			timeout		trytimes	check_res_proc
	{"AT",					"OK",		2000,		20, 		0,	"NIC Fault.(No Response)"},
	{"ATE0",				"OK",		2000,		20, 		0,	"NIC Fault.(Cfg No Resp.)"},
	{"AT+CSQ",				NULL,		2000,		20, 		check_csq,	"No Signal."},
	{"AT+CIPMODE=1",		"OK",		2000,		5, 			0,	"NIC Fault.(Mode Config Error)"},
	//AT+CIPCSGP=1,"cmnet","",""
	//{"AT+CIPCSGP=1,\"CMIOTDLMZK.LN\",\"\",\"\"",		
	//						"OK",		2000,		5, 			config_ap, "Config APN Error."},
	{NULL,					"OK",		2000,		5, 			config_ap, "Config APN Error."},
							
	{"AT+CIPSHUT",			"SHUT OK",	2000,		5, 			0, "NIC Fault.(Mode Shut Error)"},
	//180.89.58.27:9020
	//{"AT+CIPSTART=\"TCP\",\"10.10.10.45\",\"700\"",	
	//						"CONNECT",	5000,5, 		check_connect, "Cannot Connect."},
	{NULL,					"CONNECT",	5000,5, 		check_connect, "Cannot Connect."},
							
	//AT+CIPSTART="TCP","180.89.58.27","9020"
	//{"AT+CIPSTART=\"TCP\",\"119.75.218.70\",\"80\"",	
	//{NULL/*"AT+CIPSTART=\"TCP\",\"180.89.58.27\",\"9020\""*/,	
	//						NULL/*"CONNECT"*/,	30000,5, 		check_connect},
};
//AT+CSTT="cmnet","yqcxgj@yqsys.wzyd","654321"
//AT+CIPSTART="TCP","172.32.253.29","700"
//static const struct ATCMD_T at_tbl[] = {
//	//cmd					res			timeout		trytimes	check_res_proc
//	{"AT",					"AT",		2000,		20, 		0},
//	{"ATE0",				"OK",		2000,		20, 		0},
//	{"AT+CSQ",				NULL,		2000,		20, 		check_csq},
//	{"AT+CIPMODE=1",		"OK",		2000,		5, 			0},
//	//AT+CIPCSGP=1,"cmnet","",""
//	{NULL/*"AT+CIPCSGP=1,\"cmnet\",\"\",\"\""*/,		
//							"OK",		2000,		5, 			config_ap},
//	{"AT+CIPSHUT",			"SHUT OK",	2000,		5, 			0},
//	//180.89.58.27:9020
////	{"AT+CIPSTART=\"TCP\",\"119.75.218.70\",\"80\"",	
////							NULL/*"CONNECT"*/,	5000,5, 		check_connect},
//	//AT+CIPSTART="TCP","180.89.58.27","9020"
//	//{"AT+CIPSTART=\"TCP\",\"119.75.218.70\",\"80\"",	
//	{NULL/*"AT+CIPSTART=\"TCP\",\"180.89.58.27\",\"9020\""*/,	
//							NULL/*"CONNECT"*/,	30000,5, 		check_connect},
//};


static int _init(const struct HX_NIC_T *this)
{
	nic_pwr(0);
	hx_delay_ms(500);
	nic_pwr(1);
	hx_delay_ms(1000);
	
	nic_rst(0);
	hx_delay_ms(500);
	nic_rst(1);
	hx_delay_ms(1000);
	return 0;
}



static int _sock_connect(const struct HX_NIC_T *this
		,uint8_t ip[4],uint16_t port, uint16_t lc_port, int *sockid)
{
	*sockid = 0;
	char buf[128];
	int n = sprintf(buf,"AT+CIPSTART=\"TCP\",\"%hhu.%hhu.%hhu.%hhu\",\"%hu\",\"%hu\"\r\n",
			ip[0],ip[1],ip[2],ip[3],port,lc_port);
	atc_send(buf,n);
	hx_dbgi(0,"sock connect:%s\n",buf);
	const GETS_MATCH_T mtb[] = {
			{"CONNECT",.match0_prefix1_sufix2_contains3 = 1},
			{"DISCONNECT",3},
	};
	memset(buf,0,128);
	int res = atc_gets_timeout_match(buf,128,5000,mtb,2);
	if(res==0 && strcmp(buf,"CONNECT")==0)
		return 0;
	hx_dbge(0,"sock connect error:%s\n",buf);
	return -1;
}
static int _sock_disconnect(const struct HX_NIC_T *this, int *sockid)
{
	char buf[128];
	atc_send("+++",3);
	hx_dbgi(0,"sock disconnect:%s\n",buf);
	const GETS_MATCH_T mtb[] = {
			{"OK",.match0_prefix1_sufix2_contains3 = 0},
	};
	memset(buf,0,128);
	int res = atc_gets_timeout_match(buf,128,5000,mtb,2);
	if(res==0)
		return 0;
	hx_dbge(0,"sock connect error:%s\n",buf);
	return -1;
}
static int _nic_check_self(const struct HX_NIC_T *this)
{
/*	char buf[128];
	atc_send("AT+...");
	hx_dbgi("sock disconnect:%s\n",buf);
	const GETS_MATCH_T mtb[] = {
			{"OK",.match0_prefix1_sufix2_contains3 = 0},
	};
	memset(buff,0,128);
	int res = atc_gets_timeout_match(buf,128,30000,mtb,2);
	if(res==0)
		return 0;
	hx_dbge("sock connect error:%s\n",buf); */
	return -1;
}

static const struct NET_PARAM_T defprm = {
	.apn = "cmnet",
	.rm_ip = {180,89,58,27},
	.rm_port = 9020,
};
const struct HX_NIC_T nic_sim800c = {
	.default_param = &defprm,
	.at_tbl = at_tbl,
	.at_tbl_count = sizeof(at_tbl)/sizeof(at_tbl[0]),
	.init = _init,		/* on open*/

	.step_of_endinit = (sizeof(at_tbl)/sizeof(at_tbl[0])) -1,
	.nic_check_self = _nic_check_self,
	.sock_connect = _sock_connect,
	.sock_disconnect = _sock_disconnect,
};

