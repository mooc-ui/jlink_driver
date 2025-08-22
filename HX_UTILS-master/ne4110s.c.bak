#include "hx_utils.h"
#include "hx_board.h"
#include "string.h"
#include "stdio.h"

/*
	NE4110 Module Config On Factory
	1. Serial Settings -> Port1
		Baud Rate : 57600
		Data Bits : 8
		Stop Bits : 1
		Parity : None
		Flow Control : None
		FIFO : Enable
	2. Serial Command Mode :
		Trigger Setting : SW Trigger
		SW Trigger Character : 23 23 23
*/

static char * ne4110_read_cmd(const char *op,
	const char *param, char *sres)
{
	int res;
	char buff[128];
	hx_trys(i,3){
		atc_printf(">R%s%s\r",op,param);
		res = atc_gets_timeout(buff,100,500);
		if(res>0){
			//<YXX<res>
			res = sscanf(buff,"<Y%*2s%s",sres);
			if(res==1)
				return sres;
		}
	}
	return "ERROR";
}
static int ne4110_write_cmd(const char *op,const char *param)
{
	int res;
	char buff[128];
	hx_trys(i,3){
		atc_printf(">W%s%s\r",op,param);
		res = atc_gets_timeout(buff,100,500);
		if(res>0){
			if((buff[0]=='<')&&(buff[1]=='Y'))
				return 0;
		}
	}
	return -1;
}

static int ne4110_leave_cmdmode(int is_save)
{
	int res;
	char buff[100];
	hx_trys(i,5){
		atc_printf(">WBR%d\r",is_save?2:1);
		res = atc_gets_timeout(buff,100,500);
		if(res>0 && strcmp(buff,"<YBR")==0){
			return 0;
		}
	}
	return -1;
}
static int ne4110_enter_cmdmode(void)
{
	int res;
	char buff[100];
	hx_trys(i,3){
		atc_put("###");
		res = atc_gets_timeout(buff,100,500);
		if(res>0){
			if(strcmp(buff,"<E")==0)
				return 0;
		}
	}
	return -1;
}


static int ne4110_reset(void)
{
	int res = ne4110_enter_cmdmode();
	if(res)
		return res;
	res = ne4110_leave_cmdmode(0);
	if(res)
		return res;
	return 0;
}
/*
	
	ne4110_write_cmd("NC","0");				// 0:static 1:dhcp
	ne4110_write_cmd("NP","192.168.60.20");	//ip
	ne4110_write_cmd("NM","255.255.255.0");	//mask
	ne4110_write_cmd("NG","192.168.60.254");//gateway
	
	ne4110_write_cmd("AS","0");	//IP Fiter 0:dis 1:en
	ne4110_write_cmd("OM","2");	//Op Mode 0:RealCom,1:TCPServer,2:TCPClient,3:UDPMode
	ne4110_write_cmd("CM","0");	//ConnectMode : 0Startup 1AnyChar
	ne4110_write_cmd("CA","180.89.58.27");	//rm ip
	ne4110_write_cmd("C1","9020");			//rm port
	ne4110_write_cmd("CT","7");	//TCP Alive Check Timeout(min)
	ne4110_write_cmd("CI","0");	//Inactivity Timeout
	ne4110_write_cmd("CX","0");	//Number of delimiters 0Dis,1,2
//	ne4110_write_cmd("CY","23");	//Delimiter1
//	ne4110_write_cmd("CZ","0");	//Delimiter2
	ne4110_write_cmd("CF","5");	//Force Tx Timeout
*/


//struct NE4110_CFG_ST
//{
//    int dhcp;
//    char ip[16];
//    char mask[16];
//    char gw[16];
//};

//if param_file set NULL,that use deault.
static int ne4110_config(char *param_file)
{
	int res;
	char buff[100];
	res = ne4110_enter_cmdmode();
	if(res){
		ne4110_leave_cmdmode(0);
		hx_delay_ms(3000);		//after reset, need some time .
		res = ne4110_enter_cmdmode();
		if(res)
			return res;
	}
#define NE_DINFO(...)	HX_DBG_PRINTLN(__VA_ARGS__)	
	HX_NIC_INFO_T *ne_info = &g_nic_info;
	if(ne_info) memset(ne_info, 0, sizeof(HX_NIC_INFO_T));
	NE_DINFO("SerialNumber:%s", ne4110_read_cmd("BS","",buff));
	if(ne_info) strncpy(ne_info->sn,buff,9);
	NE_DINFO("FirmwareVersion:%s", ne4110_read_cmd("BV","",buff));
	if(ne_info) strncpy(ne_info->firm_ver,buff,9);
	NE_DINFO("ModuleName:%s", ne4110_read_cmd("BN","",buff));
	if(ne_info) strncpy(ne_info->module_name,buff,19);
	NE_DINFO("MAC:%s", ne4110_read_cmd("NA","",buff));
	if(ne_info) strncpy(ne_info->mac,buff,19);
	
	char default_params[] = 
		"NC=0"		// 0:static 1:dhcp
		"&NP=192.168.60.20"	//ip
		"&NM=255.255.255.0"	//mask
		"&NG=192.168.60.254"	//gateway
		"&AS=0"	//IP Fiter 0:dis 1:en
		"&OM=2"	//Op Mode 0:RealCom,1:TCPServer,2:TCPClient,3:UDPMode
		"&CM=0"	//ConnectMode : 0Startup 1AnyChar
		"&CA=180.89.58.27"	//rm ip
		"&C1=9020"			//rm port
		"&CT=7"	//TCP Alive Check Timeout(min)
		"&CI=0"	//Inactivity Timeout
		"&CX=0"	//Number of delimiters 0Dis,1,2
	//	"&CY=23"	//Delimiter1
	//	"&CZ=0"	//Delimiter2
		"&CF=5"	//Force Tx Timeout
	;
	char *paramfs[2] = {default_params, param_file};
	
	for(int i=0;i<2;i++){
		char *p = paramfs[i];
		if(p==NULL)
			break;
		p = strtok(p,"&");
		do{
			if(p){
				char cmd[3];
				strncpy(cmd,p,2);
				cmd[2]=0;
				char *op = p+3;
				res = ne4110_write_cmd(cmd,op);
				if(res)
					return -999;
				p = strtok(NULL,"&");
			}else{
				break;
			}
		}while(p);
	}
	res = ne4110_leave_cmdmode(1);
	if(res){
		hx_delay_ms(200);
		ne4110_leave_cmdmode(1);
	}
	
	return 0;
}
//struct NE4110_CFG_ST g_ne4110_cfg = {
//	0,
//	{192,168,60,20},
//	{255,255,255,255},
//	{192,168,60,254},
//};

static const HX_ATARG_T defarg = {
	.rm_ip = "180.89.58.27",
	.rm_port = 9020,
	.lc_ip = "0.0.0.0",
	.mask = "0.0.0.0",
	.gateway = "0.0.0.0",
	.dhcp_en = 1,
};

static int _init(const struct HX_NIC_T *this, int *pstep, HX_ATARG_T *arg)
{
	int res;
	char ne_params[512];
	char *p = ne_params;
	p += sprintf(p,
		"NC=%u"				// 0:static 1:dhcp
		"&NP=%s"	//ip
		"&NM=%s"	//mask
		"&NG=%s"	//gateway
		"&CA=180.89.58.27"	//rm ip
		"&C1=9020"			//rm port
	,this->default_arg->dhcp_en?1:0
	,this->default_arg->lc_ip
	,this->default_arg->mask
	,this->default_arg->gateway
	);
	do{
		res = ne4110_config(ne_params);
		if(res){
			HX_DBG_PRINTLN("NE4110S INIT Fail, Error Code(%d) . Retry in 3S.",res);
			return -1;
		}
	}while(res);
	HX_DBG_PRINTLN("NE4110S INIT OK.");
	return 0;
}


static void _reset(const struct HX_NIC_T *this) 
{
	  ne4110_reset();
}

const struct HX_NIC_T nic_ne4110s = {
	.default_arg = &defarg,
	.init = _init,
	.reset = _reset,
};



