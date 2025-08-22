#include "hx_utils.h"
#include "string.h"
#include "stdio.h"
#include "hxd_atc.h"

static HX_DEV uart = {0};

static int _init(const struct HX_NIC_T *this)
{
	hx_open("emu485_uart","57600",&uart);
	return 0;
}


/*
	@$$$$# connect
	@$$$# disconnect
	@$#	   ok
	@$$#   error
	@$$$$$...#dbginfo
*/
static int _sock_connect(const struct HX_NIC_T *this
		,uint8_t ip[4],uint16_t port, uint16_t lc_port, int *sockid)
{
	int res;
	int c;
	*sockid = 0;
	for(int i=0;i<2;i++){
		hxl_put(&uart,"@$$$$#");
		hxl_flush(&uart);
		res = hxl_getc_timeout(&uart,&c,1000);
		if(res)continue;
		if(c!='@')continue;
		res = hxl_getc_timeout(&uart,&c,100);
		if(res)continue;
		if(c!='$')continue;
		res = hxl_getc_timeout(&uart,&c,100);
		if(res)continue;
		if(c=='#'){
			hx_dbgi(0,"sock connected:\n");
			return 0;
		}else if(c=='$'){
			hx_dbge(0,"sock connect error:\n");
			return -9;
		}else{
			//recive over
			while(hxl_getc_noblock(&uart,&c)==0);
		}
		
	}
	hx_dbge(0,"sock connect timeout:\n");
	return -1;
}

static int _sock_disconnect(const struct HX_NIC_T *this, int *sockid)
{
	int res;
	int c;
	*sockid = 0;
	for(int i=0;i<2;i++){
		hxl_put(&uart,"@$$$#");
		hxl_flush(&uart);
		res = hxl_getc_timeout(&uart,&c,1000);
		if(res)continue;
		if(c!='@')continue;
		res = hxl_getc_timeout(&uart,&c,100);
		if(res)continue;
		if(c!='$')continue;
		res = hxl_getc_timeout(&uart,&c,100);
		if(res)continue;
		if(c=='#'){
			hx_dbgi(0,"sock connected:\n");
			return 0;
		}else if(c=='$'){
			hx_dbge(0,"sock connect error:\n");
			return -9;
		}else{
			//recive over
			while(hxl_getc_noblock(&uart,&c)==0);
		}
		
	}
	hx_dbge(0,"sock connect timeout:\n");
	return -1;
}
static int _nic_check_self(const struct HX_NIC_T *this)
{
	
	return -1;
}
static int _read(const struct HX_NIC_T *this,void *buf,int _size)
{
	return hx_read(&uart,buf,_size);
}
static int _write(const struct HX_NIC_T *this,const void *buf, int size)
{
	return hx_write(&uart,buf,size);
}
static const struct NET_PARAM_T defprm = {
	.apn = "cmnet",
	.rm_ip = {180,89,58,27},
	.rm_port = 9020,
};
const struct HX_NIC_T nic_485emu = {
	.default_param = &defprm,
	.at_tbl = NULL,
	.at_tbl_count = 0,
	.init = _init,
	.read = _read,
	.write = _write,

	.step_of_endinit = 0,
	.nic_check_self = _nic_check_self,
	.sock_connect = _sock_connect,
	.sock_disconnect = _sock_disconnect,
	//.net_config 
	.prv = &uart,
};

