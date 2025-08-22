#include "hx_utils.h"
#include "string.h"
#include "stdio.h"
#include "hxd_iopin.h"
#include "hxd_ioport.h" 

/*
========================================================
//命令模式
AT+CRESET 
AT+CSQ
AT+NETCLOSE
AT+CGDCONT=1,"IP","apn"
AT+CGSOCKCONT=1,"IP","CMNET"
AT+CSOCKSETPN=1
AT+CIPMODE=0
AT+NETOPEN
AT+CIPOPEN=3,"TCP","119.75.218.70",80
AT+CIPSEND=3,5
>
[OK]

//透传模式
AT+CRESET 
AT+CSQ
AT+NETCLOSE
AT+CGDCONT=1,"IP","apn"
AT+CGSOCKCONT=1,"IP","CMNET"
AT+CSOCKSETPN=1
AT+CIPMODE=1
AT+NETOPEN
AT+CIPOPEN=0,"TCP","119.75.218.70",80
AT+CIPSEND=3,5

AT+CIPOPEN=0,"TCP","180.89.58.27",9020


==========================================================
//ping 测试
AT+CPING="www.baidu.com",1,4,64,1000,10000,255
AT+CPING="118.186.199.151",1,4,64,1000,10000,255
AT+CPING="www.qq.com",1,4,64,1000,10000,255
AT+CPING="119.75.218.70",1,4,64,1000,10000,255
AT+CPING="www.tcps.com.cn",1,4,64,1000,10000,255



//telnet测试
telnet 118.186.199.151 9020
===============================================================
C:\Users\houxd>ping www.qq.com

正在 Ping www.qq.com [182.254.18.159] 具有 32 字节的数据:
来自 182.254.18.159 的回复: 字节=32 时间=10ms TTL=50
来自 182.254.18.159 的回复: 字节=32 时间=10ms TTL=50
来自 182.254.18.159 的回复: 字节=32 时间=9ms TTL=50
来自 182.254.18.159 的回复: 字节=32 时间=9ms TTL=50

182.254.18.159 的 Ping 统计信息:
    数据包: 已发送 = 4，已接收 = 4，丢失 = 0 (0% 丢失)，
往返行程的估计时间(以毫秒为单位):
    最短 = 9ms，最长 = 10ms，平均 = 9ms

C:\Users\houxd>ping www.baidu.com

正在 Ping www.a.shifen.com [119.75.218.70] 具有 32 字节的数据:
来自 119.75.218.70 的回复: 字节=32 时间=14ms TTL=53
来自 119.75.218.70 的回复: 字节=32 时间=7ms TTL=53
来自 119.75.218.70 的回复: 字节=32 时间=8ms TTL=53
来自 119.75.218.70 的回复: 字节=32 时间=8ms TTL=53

119.75.218.70 的 Ping 统计信息:
    数据包: 已发送 = 4，已接收 = 4，丢失 = 0 (0% 丢失)，
往返行程的估计时间(以毫秒为单位):
    最短 = 7ms，最长 = 14ms，平均 = 9ms

*/

static int on_csq(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	int res;
	//+CSQ: %d,%d
	if(buf[0] == '+'){
		int a=0,b=0;
		res = sscanf(buf,"+CSQ: %d,%d",&a,&b);
		if(res==2 &&(a>0 && a<99))
			return 0;
	}
	return -1;
}
static int on_zpas(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	if(strstr(buf,"CS_PS"))	//CONNECT FAIL
		return 0;
	if(strstr(buf,"PS_ONLY"))	//CONNECT ERROR
		return 0;			
	return -1;
}
static int on_creg(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	int res;
	//+CSQ: %d,%d
	if(buf[0] == '+'){
		int a=0,b=0;
		res = sscanf(buf,"+CREG: %d,%d",&a,&b);
		if(res==2 && (b==1||b==5))
			return 0;
	}
	return -1;
}

static int check_zipsetprt(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	return 0;
}
static int check_zipcall(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	int id=0;
	int res = sscanf(buf,"+ZIPCALL: %d,",&id);
	if(res==1 && id>0){
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
	int res;
	int socket,state;
	res = sscanf(buf,"+ZIPSTAT: %d,%d",&socket,&state);
    if(res ==2 && socket>0 && state==1)
        return 0;
    return -1;
}

static int on_netact(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	int res;
	int socket,state;
	int ip[4] = {0};
	res = sscanf(buf,"$MYURCACT: %d,%d,\"%u.%u.%u.%u\"",&socket,&state,
		&ip[0],&ip[1],&ip[2],&ip[3]
	);
    if(res==6 && state==1){
        hx_dbgi(0,"lc_ip: %u.%u.%u.%u\n",ip[0],ip[1],ip[2],ip[3]);
		return 0;
	}
    return -1;
}


static int on_connect(
	int step, 
	enum ATEVENT_T event,
	char *buf, 
	void *msg)
{
	if(strstr(buf,"FAIL"))	//CONNECT FAIL
		return -1;
	if(strstr(buf,"ERROR"))	//CONNECT ERROR
		return -2;			
	if(strstr(buf,"CONNECT"))	//CONNECT 115200 / ALREADY CONNECT etc..
		return 0;
	return -1;
}
/*
AT+CPIN?		//返回 "+CPIN: READY"
AT+CSQ			//返回 "+CSQ: a,b"    a>15 && a!=99
AT+CREG?		//返回 "+CREG: a,b"	  b==1 || b==5
AT+ZPAS?		//返回 "CS_PS" 或者 "PS_ONLY"
AT$MYNETINFO=1	//返回 "OK"		设置模式见下表
AT$MYNETCON=0,"APN","cmnet"		//返回	"OK"
//AT$MYNETCON=0,"USERPWD",","	//这条可以不用
AT$MYNETACT=0,1	//返回 $MYURCACT: a,b,"ip地址" 其中 b==1
AT$MYNETCREATE=0,0,0,"180.89.58.27",9020,9020	//返回 "CONNECT"

mode:
1:auto
2:2G( GSM, EDGE,CDMA)
3:3G( WCDMA, TD-SCDMA, EVDO)
4:4G( FDD-LTE, TD-LTE)
5:2G+3G
6:2G+4G
7:3G+4G



*/
static const struct ATCMD_T at_tbl_for_c1b[] = {
	//cmd					res			timeout		trytimes	event_proc
	//{"ATE1",				"OK",		2000,		20, 		0},
	//{"ATE0",				"OK",		2000,		20, 		0},
	{"AT+CPIN?",			"+CPIN: READY",		2000,		20, 		on_csq},
	{"AT+CSQ",				NULL,		2000,		20, 		on_csq},
	{"AT+CREG?",			NULL,		2000,		20,			on_creg},
	{"AT+ZPAS?",				NULL,		2000,		20,			on_zpas},
//	{"AT$MYNETINFO=1",		"OK",		2000,		5,			0},
	//{"AT$MYNETCON=0,\"APN\",\"CMIOTDLMZK.LN\"",		
	//						"OK",		2000,		5,			0},
	//{"AT$MYNETCON=0,\"USERPWD\",\",\"",		
	//						"OK",		2000,		5,			0},
	{"AT$MYNETCON=0,\"USERPWD\",\"CARD\",\"CARD\"",		
							"OK",		2000,		5,			0},
	
	{"AT$MYNETACT=0,1",		NULL,		2000,		5,			on_netact},
	{"AT$MYNETCREATE=0,0,0,\"180.89.58.27\",9020,9020",	
							NULL,		3000,		5,			on_connect},
};

static const struct ATCMD_T at_tbl[] = {
	//cmd					res			timeout		trytimes	event_proc
	{"ATE1",				"OK",		2000,		20, 		0},
	{"ATE0",				"OK",		2000,		20, 		0},
	{"AT+CSQ",				NULL,		2000,		20, 		on_csq},
	//{"AT+CREG?",			NULL,		2000,		20,			on_creg},
	{"AT+ZSNT=0,0,0",		"OK",		2000,		5,			0},
	//{"AT+CEREG?",			NULL,		2000,		10,			on_cereg},
	//{"AT+ZPAS?",			NULL,		2000,		5,			on_zpas},
	//AT$MYNETCREATE=0,0,0,"180.89.58.27",9020,9020
	//AT$MYNETCREATE=0,0,0,"127.0.0.1",9020
	//{"AT+CGDCONT=1,\"IP\",\"CMNET\"",		
	//						"OK",		2000,		5, 			0},
	//AT$MYNETCREATE=1,2,2,"172.22.44.123",5300,3000
	
	{"AT+ZIPCFG=cmnet",		"OK",		2000,		5,			0},
	{"AT+ZIPCALL=1",		NULL,		3000,		10,			check_zipcall,},
	{"AT+ZIPSETRPT=0",		NULL,		3000,		5,			check_zipsetprt},
	ATCMD_DELAY(3000),
	{"AT+ZIPOPEN=1,0,180.89.58.27,9020",
							NULL,		3000,		5,			check_zipstat},	
};

static int _init_me3630_pid_c1a(const struct HX_NIC_T *this)
{	
	nic_pwr(1);
	nic_rst(1);
	hx_delay_ms(100);		
	
	nic_pwr(0);
	hx_delay_ms(500);		//pdf say 0.2s normal
	nic_pwr(1);
	hx_delay_ms(500);		//pdf no say
	
	nic_rst(0);
	hx_delay_ms(1500);		//pdf say 1s normal
	nic_rst(1);
	hx_delay_ms(3000);		//pdf say 32s normal
	return 0;
}

static int _init_me3630_pid_c1b(const struct HX_NIC_T *this)
{
	HX_DEV d;
	hx_open("at_io_pp","1",&d);
	hx_write(&d,"\x00",1);
	
	nic_pwr(1);
	nic_rst(1);
	hx_delay_ms(100);		
	
	nic_pwr(0);
	hx_delay_ms(500);		//pdf say 0.2s normal
	nic_pwr(1);
	hx_delay_ms(500);		//pdf no say
	
	nic_rst(0);
	hx_delay_ms(1500);		//pdf say 1s normal
	nic_rst(1);
	hx_delay_ms(3000);		//pdf say 32s normal
	return 0;
}
static const struct NET_PARAM_T defprm = {
	.apn = "cmnet",
	.rm_ip = {180,89,58,27},
	.rm_port = 9020,
};
const struct HX_NIC_T nic_me3630_pid_c1b = {
	.default_param = &defprm,
	.at_tbl = at_tbl_for_c1b,
	.at_tbl_count = sizeof(at_tbl_for_c1b)/sizeof(at_tbl_for_c1b[0]),
	.init = _init_me3630_pid_c1b,
};

static int read_c1a(const struct HX_NIC_T *this,void *buf,int _size)
{
	char buff[4096],buff2[2048];
	int res = atc_gets_noblock(buff,2047);
	if(res>0){
		hx_dbg(0,"AT_CMD_RX: [%s]\n",buff);
		int id,ip[4],port,len;
		res = sscanf(buff,"+ZIPRECV: %u,%u.%u.%u.%u,%u,%u,%s",
			&id,ip,ip+1,ip+2,ip+3,&port,&len,buff2);
		if(res==8){
			if(len>0 && len<1024){
				if(len>_size)
					len = _size;
				hx_hexcode2bin(buff2,len*2,buf);
				return len;
			}
		}
	}
	return -1;
}
static int write_c1a(const struct HX_NIC_T *this,const void *buf, int _size)
{
	char buff[2048] = {0};
	char *p = buff;
	p += sprintf(p,"AT+ZIPSEND=1,");
	hx_dumphex2str(buf,_size,p);
	p += _size*2;
	p += sprintf(p,"\r\n");
	atc_put(buff);
	hx_dbg(0,"AT_CMD_TX: [%s]\n",buff);
	return _size;
}

const struct HX_NIC_T nic_me3630_pid_c1a = {
	.default_param = &defprm,
	.at_tbl = at_tbl,
	.at_tbl_count = sizeof(at_tbl)/sizeof(at_tbl[0]),
	.init = _init_me3630_pid_c1a,
	.read = read_c1a,
	.write = write_c1a,
};

