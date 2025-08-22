#include "hx_utils.h"
#include "string.h"
#include "stdio.h"

/*
at+ping=192.168.1.1,1,1
at+write_config=622,wifi_mode=AP&power_mode=full&ap_channel=1&ap_ssid=RAK_00
0001&ap_secu_en=0......\r\n
<+++> >U< <U> ==>
at+read_config
OKpower_mode=full&wifi_mode=AP&sta_ssid=RAK415_STATION&sta_secu_en=1&sta_psk=1234567890&sta_dhcp_en=1&sta_ipaddr=192.168.7.1&sta_netmask=255.255.255.0&sta_gatew
ay=192.168.7.1&sta_dnssever1=0.0.0.0&sta_dnssever2=0.0.0.0&ap_channel=9&ap_ssid=RAK415_AP&ap_secu_en=0&ap_ipaddr=192.168.7.1&ap_netmask=255.255.255.0&uart_baudr
ate=115200&uart_datalen=8&uart_parity_en=0&uart_stoplen=1&uart_rtscts_en=0&uart_recvlenout=500&uart_timeout=20&socket_multi_en=0&socketA_type=ltcp&socketA_local
port=25000&socketA_tcp_timeout=0&user_name=admin&user_password=admin&module_name=RAK415&module_group=RAK415

client mode 
at+read_config
OKpower_mode=full&wifi_mode=STA&sta_ssid=whyyy&sta_secu_en=1&sta_psk=yy123456789&sta_dhcp_en=1&sta_ipaddr=192.168.7.1&sta_netmask=255.255.255.0&sta_gateway=192.
168.7.1&sta_dnssever1=0.0.0.0&sta_dnssever2=0.0.0.0&ap_channel=9&ap_ssid=RAK415_AP&ap_secu_en=0&ap_ipaddr=192.168.7.1&ap_netmask=255.255.255.0&uart_baudrate=115
200&uart_datalen=8&uart_parity_en=0&uart_stoplen=1&uart_rtscts_en=0&uart_recvlenout=500&uart_timeout=20&socket_multi_en=0&socketA_type=tcp&socketA_destport=9020
&socketA_localport=25000&socketA_destip=180.89.58.27&socketA_tcp_timeout=0&user_name=admin&user_password=admin&module_name=RAK415&module_group=RAK415

at+rssi
ERROR-2
RSSI=-47dB

*/

static const char *default_params = 
		"power_mode=full"
		"&wifi_mode=STA"
	/*
		"&sta_ssid=tplink"
		"&sta_secu_en=1"
		"&sta_psk=mmm"
		"&sta_dhcp_en=1"
	
		"&sta_ipaddr=192.168.7.1&"
		"sta_netmask=255.255.255.0"
		"&sta_gateway=192.168.7.1"
		"&sta_dnssever1=0.0.0.0"
		"&sta_dnssever2=0.0.0.0"
*/
	
	
//		"&uart_baudrate=115200"
//		"&uart_datalen=8"
//		"&uart_parity_en=0"
//		"&uart_stoplen=1"
//		"&uart_rtscts_en=0"
//		"&uart_recvlenout=500"
//		"&uart_timeout=20"
		"&socket_multi_en=0"
		"&socketA_type=tcp"
		"&socketA_destport=0"
		"&socketA_localport=9999"
		"&socketA_destip=0.0.0.0"
		"&socketA_tcp_timeout=0"
//		"&user_name=admin"
//		"&user_password=admin"
//		"&module_name=RAK415"
//		"&module_group=RAK415"
		;


#define RAK_DEF_TIMEOUT		(500)

static int rak415_leave_cmdmode(void)
{
	hx_dbg(0,"%s:%s\n",__FUNCTION__,"");
	int res;
	char buff[100];
	hx_trys(i,2){
		atc_printf("at+easy_txrx\r\n");
		atc_flush();
		const GETS_MATCH_T mtb[] = {
			{"OK",.match0_prefix1_sufix2_contains3=3,},
		};
		res = atc_gets_timeout_match(buff,100,RAK_DEF_TIMEOUT,mtb,sizeof(mtb)/sizeof(mtb[0]));
		if(res==0){
			hx_dbgi(0,"ok\n");
			return 0;
		}
	}
	hx_dbge(0,"error\n");
	return -1;
}
static int rak415_enter_cmdmode(void)
{
	hx_dbg(0,"%s:%s\n",__FUNCTION__,"");
	int res;
	char buff[100];
	
	hx_trys(i,2){
		{
			hx_trys(j,2){
				atc_put("+++");
				atc_flush();
				int c = 0;
				res = atc_getc_timeout(&c,200);
				if(res == 0){
					if(c=='U'){
						break;
					}
				}
			}
			hx_delay_ms(100);	//<3S
			atc_putc('U');
			atc_flush();
			const GETS_MATCH_T mtb[] = {
				{"OK",.match0_prefix1_sufix2_contains3=3,},
			};
			//pdf no say time, 500ms enough?
			res = atc_gets_timeout_match(buff,100,500,mtb,sizeof(mtb)/sizeof(mtb[0]));
			if(res==0){
				hx_dbgi(0,"ok\n");
				return 0;
			}
		}
		{
			//mabye is opend
			atc_puts("at+ascii=1");
			atc_flush();
			const GETS_MATCH_T mtb[] = {
				{"OK",.match0_prefix1_sufix2_contains3=3,},
			};
			res = atc_gets_timeout_match(buff,100,RAK_DEF_TIMEOUT,mtb,sizeof(mtb)/sizeof(mtb[0]));
			if(res==0){
				hx_dbgi(0,"ok\n");
				return 0;
			}
		}
	}
	hx_dbge(0,"error\n");
	return -1;
}

// normal return <0 xdB , >=0 is not connect
static int rak415_rssi(void)
{
	hx_dbg(0,"%s:%s\n",__FUNCTION__,"");
	int res;
	int ret = 1;
	char buff[50];
	atc_puts("at+rssi");
	atc_flush();
	hx_trys(i,3){
		do{
			res = atc_gets_timeout(buff,50,RAK_DEF_TIMEOUT);
			if(res>0){
				if(strstr(buff,"OK")){
					continue;
				}else if(strstr(buff,"ERROR")){
					ret = 0;
					goto end;
				}else{
					sscanf(buff,"RSSI=%ddB",&res);
					ret = res;
					goto end;
				}
			}
		}while(res);
	}
end:
	hx_dbg(0,"return %d\n",ret);
	return ret;
}
static int rak415_reset(void)
{
	hx_dbg(0,"%s:%s\n",__FUNCTION__,"");
	int res;
	res = -1;
	char buff[50];
	atc_puts("at+reset");
	atc_flush();
	const GETS_MATCH_T mtb[] = {
		{"OK",.match0_prefix1_sufix2_contains3=3,},
	};
	res = atc_gets_timeout_match(buff,100,RAK_DEF_TIMEOUT,mtb,sizeof(mtb)/sizeof(mtb[0]));
	if(res==0){
		hx_dbgi(0,"ok\n");
		return 0;
	}
	hx_dbge(0,"error\n");
	return -1;
}
static int rak415_tcp_status(int sockid)
{
	hx_dbg(0,"%s:%d\n",__FUNCTION__,sockid);
	int res;
	res = -1;
	char buff[50];
	atc_printf("at+tcp_status=%u\r\n",sockid);
	atc_flush();
	const GETS_MATCH_T mtb[] = {
		{"OK1",.match0_prefix1_sufix2_contains3=3,},
		{"OK0",.match0_prefix1_sufix2_contains3=3,},
		{"ERROR",.match0_prefix1_sufix2_contains3=3,},
	};
	res = atc_gets_timeout_match(buff,100,RAK_DEF_TIMEOUT,mtb,sizeof(mtb)/sizeof(mtb[0]));
	if(res==0){
		hx_dbgi(0,"ok\n");
		return 0;
	}else if(res==1){
		hx_dbgi(0,"tcp not connect\n");
		return -2;
	}else if(res==3){
		hx_dbgi(0,"unknow error\n");
		return -2;
	}
	hx_dbge(0,"error\n");
	return -1;
}
static int rak415_ascii(int mode)
{
	hx_dbg(0,"%s:%d\n",__FUNCTION__,mode);
	int res;
	res = -1;
	char buff[50];
	hx_trys(i,2){
		atc_printf("at+ascii=1\r\n");
		atc_flush();
		const GETS_MATCH_T mtb[] = {
			{"OK",.match0_prefix1_sufix2_contains3=3,},
		};
		res = atc_gets_timeout_match(buff,100,RAK_DEF_TIMEOUT,mtb,sizeof(mtb)/sizeof(mtb[0]));
		if(res==0){
			hx_dbgi(0,"ok\n");
			return 0;
		}
	}
	hx_dbge(0,"error\n");
	return -1;
}
static int rak415_con_status(void)
{
	hx_dbg(0,"%s:%s\n",__FUNCTION__,"");
	int res;
	res = -1;
	char buff[50];
	atc_printf("at+con_status\r\n");
	atc_flush();
	const GETS_MATCH_T mtb[] = {
		{"OK1",.match0_prefix1_sufix2_contains3=3,},
		{"OK0",.match0_prefix1_sufix2_contains3=3,},
		{"ERROR",.match0_prefix1_sufix2_contains3=3,},
	};
	res = atc_gets_timeout_match(buff,100,RAK_DEF_TIMEOUT,mtb,sizeof(mtb)/sizeof(mtb[0]));
	if(res==0){
		hx_dbgi(0,"ok\n");
		return 0;
	}else if(res==1){
		hx_dbgi(0,"wifi not connect\n");
		return -2;
	}else if(res==3){
		hx_dbgi(0,"unknow error\n");
		return -2;
	}
	hx_dbge(0,"error\n");
	return -1;
}
static int rak415_write_config(const char *params)
{
	hx_dbg(0,"%s:%s\n",__FUNCTION__,params);
	int res;
	char buff[100];
	hx_trys(i,5){
		atc_printf("at+write_config=%d,%s\r\n",(int)(strlen(params)),params);
		atc_flush();
		hx_trys(j,3){
			res = atc_gets_timeout(buff,100,500);
			if(res>0 ){
				if(strcmp(buff,"OK")==0){
					hx_dbgi(0,"ok\n");
					return 0;
				}
			}
		}
	} 
	hx_dbge(0,"error\n");
	return -1;
}


static int _reset(const struct HX_NIC_T *this) 
{
	return rak415_reset();
}


static int _sock_connect(const struct HX_NIC_T *this
		,uint8_t ip[4],uint16_t port, uint16_t lc_port, int *sockid)
{	
	int res;
	
	res = rak415_enter_cmdmode();
	if(res)
		return res;
	
	char buff[512];
	char *p = buff;
	p += sprintf(buff,
		"socketA_type=tcp"
		"&socketA_localport=%hu"
		"&socketA_destport=%hu"
		"&socketA_destip=%hhu.%hhu.%hhu.%hhu"
		"&socketA_tcp_timeout=0",
	
		lc_port,
		port,
		ip[0],ip[1],ip[2],ip[3]
	);
	
	res = rak415_write_config(buff);
	if(res)
		return -1;
	
	int i;
	for(i=0;i<8 && rak415_tcp_status(0) ;i++)
		hx_delay_ms(1000);
	if(i>=8){
		hx_dbge(0,"Socket Connect Connect:%hhu.%hhu.%hhu.%hhu:%hu\n",
			ip[0],ip[1],ip[2],ip[3],port
		);
		return -2;
	}
	res = rak415_leave_cmdmode();
	if(res){
		hx_delay_ms(200);
		rak415_leave_cmdmode();
	}
	hx_dbgi(0,"Socket Connected\n");
	return 0;
}
//socketA_type=tcp&socketA_destport=9020&socketA_destip=192.168.1.115
static int _sock_disconnect(const struct HX_NIC_T *this, int *sockid)
{
	int res;
	
	res = rak415_enter_cmdmode();
	if(res){	
		return res;
	}
	
	char buff[512];
	char *p = buff;
	p += sprintf(buff,
		"socketA_type=ltcp"
		"&socketA_localport=1"
		"&socketA_destport=1"
		"&socketA_destip=0.0.0.0"
		"&socketA_tcp_timeout=0"
	);
	
	res = rak415_write_config(buff);
	if(res)
		return -1;
	

	res = rak415_leave_cmdmode();
	if(res){
		hx_delay_ms(200);
		rak415_leave_cmdmode();
	}
	return 0;
}
static int _nic_check_self(const struct HX_NIC_T *this)
{
	return -1;
}

static int _net_config(const struct HX_NIC_T *this)
{
	int res;
	char buff[512];
	char *p = buff;
	p += sprintf(buff,
		"power_mode=full"
		"&wifi_mode=STA"
		
		"&socket_multi_en=0"
		"&socketA_type=tcp"
		"&socketA_destport=1"
		"&socketA_localport=1"
		"&socketA_destip=0.0.0.0"
		"&socketA_tcp_timeout=0"
	
		"&sta_ssid=%s"
		"&sta_secu_en=%u"	//1 0
		"&sta_psk=%s"
		"&sta_dhcp_en=%u"	//1 0
		"&sta_ipaddr=%hhu.%hhu.%hhu.%hhu"
		"&sta_netmask=%hhu.%hhu.%hhu.%hhu"
		"&sta_gateway=%hhu.%hhu.%hhu.%hhu"
		"&sta_dnssever1=%hhu.%hhu.%hhu.%hhu"
		"&sta_dnssever2=%hhu.%hhu.%hhu.%hhu"
		,
	
		g_net_param.w_ssid,
		g_net_param.w_secu_en,
		g_net_param.w_psk,
		g_net_param.w_dhcp_en,
		g_net_param.w_lc_ip[0],g_net_param.w_lc_ip[1],g_net_param.w_lc_ip[2],g_net_param.w_lc_ip[3],
		g_net_param.w_lc_mask[0],g_net_param.w_lc_mask[1],g_net_param.w_lc_mask[2],g_net_param.w_lc_mask[3],
		g_net_param.w_lc_gw[0],g_net_param.w_lc_gw[1],g_net_param.w_lc_gw[2],g_net_param.w_lc_gw[3],
		g_net_param.w_lc_dsn1[0],g_net_param.w_lc_dsn1[1],g_net_param.w_lc_dsn1[2],g_net_param.w_lc_dsn1[3],
		g_net_param.w_lc_dsn2[0],g_net_param.w_lc_dsn2[1],g_net_param.w_lc_dsn2[2],g_net_param.w_lc_dsn2[3]
		);
	res = rak415_write_config(buff);
	if(res)
		return -1;
	
	do{
		hx_delay_ms(1000);
		hx_dbg(0,"Wifi Connecting: %s %s\n",g_net_param.w_ssid,g_net_param.w_psk);
		res = rak415_con_status();
	}while(res);
	hx_dbgi(0,"Wifi Connected: %s %s\n",g_net_param.w_ssid,g_net_param.w_psk);

	res = rak415_leave_cmdmode();
	if(res){
		hx_delay_ms(200);
		rak415_leave_cmdmode();
	}
	
	return 0;
}

static int _init(const struct HX_NIC_T *this)
{
	HX_DEV rak415_pwr,rak415_rst;
	hx_open("io_pin7","1",&rak415_pwr);
	hx_open("io_pin8","1",&rak415_rst);
	
	hx_write(&rak415_rst,"\x01",1);
	
	hx_write(&rak415_pwr,"\x00",1);
	hx_delay_ms(1000);
	hx_write(&rak415_pwr,"\x01",1);
	
	int res;
	//char buff[100];
	hx_dbg(0,"======================================\n");
	while(rak415_enter_cmdmode()){
		hx_delay_ms(500);
		hx_dbg(0,"rak415_init: into cmd mode...\n");
	}
	res = rak415_ascii(1);
	return res;
}
static const struct NET_PARAM_T defprm = {
	.rm_ip = {180,89,58,27},
	.rm_port = 9020,
	.w_ssid = "TP-LINK_AA18C8",
};

const struct HX_NIC_T nic_rak415 = {
	.default_param = &defprm,
	.at_tbl = NULL,
	.at_tbl_count = 0,
	.init = _init,
	.reset = _reset,
	//.state = _state,
	
	.nic_check_self = _nic_check_self,
	.sock_connect = _sock_connect,
	.sock_disconnect = _sock_disconnect,
	.net_config = _net_config,
};

