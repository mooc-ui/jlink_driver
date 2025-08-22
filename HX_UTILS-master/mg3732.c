#include "hx_utils.h"
#include "string.h"
#include "stdio.h"


static int check_zipcall(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	if(strstr(buf,"+ZIPCALL")){
		return 0;
	}
	return -1;
}

static int check_zipstat(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	int socket,state;
	sscanf(buf,"+ZIPSTAT: %d,%d",&socket,&state);
    if(socket>0 && state==1)
        return 0;
    return -1;
}

const struct ATCMD_T at_tbl[] = {
	//cmd					res			timeout		trytimes	check_res_proc
	{"AT",					"AT",		2000,		20, 		0},
	{"ATE0",				"OK",		2000,		20, 		0},
	//{"AT+CSQ",				NULL,		2000,		20, 		check_csq},
	{"AT+ZIPCFG=cmnet",		"OK",		2000,		5, 			0},
	{"AT+ZIPCALL=1",		NULL,		30000,		10,			check_zipcall,},
	//这里需要延时
	ATCMD_DELAY(2000),
	//AT+CIPCSGP=1,"cmnet","",""
	//{"AT+CIPCSGP=1,\"cmnet\",\"\",\"\"",		
	//						"OK",		2000,		5, 			0},
	//{"AT+CIPSHUT",			"SHUT OK",	2000,		5, 			0},
	//180.89.58.27:9020
//	{"AT+CIPSTART=\"TCP\",\"119.75.218.70\",\"80\"",	
//							NULL/*"CONNECT"*/,	5000,5, 		check_connect},
	//AT+CIPSTART="TCP","180.89.58.27","9020"
	{"AT+ZIPOPEN=1,0,180.89.58.27,9020,80",
							NULL,		30000,		5,			check_zipstat},	
//	{"AT+CIPSTART=\"TCP\",\"180.89.58.27\",\"9020\"",	
//							NULL/*"CONNECT"*/,	30000,5, 		check_connect},
};


static int _init(const struct HX_NIC_T *this)
{
	nic_pwr(1);
	nic_rst(1);
	hx_delay_ms(100);		
	
	nic_pwr(0);
	hx_delay_ms(4000);		//pdf say >3500ms is poweron
	nic_pwr(1);
	hx_delay_ms(500);		//pdf no say

	return 0;
}

static const struct NET_PARAM_T defprm = {
	.apn = "cmnet",
	.rm_ip = {180,89,58,27},
	.rm_port = 9020,
};
const struct HX_NIC_T nic_mg3732 = {
	.default_param = &defprm,
	.at_tbl = at_tbl,
	.at_tbl_count = sizeof(at_tbl)/sizeof(at_tbl[0]),
	.init = _init,
};


