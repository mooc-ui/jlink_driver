#include "hx_utils.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "hx_device.h"
#include "hxd_uart.h"

#define VSPRINTF_BUFF_SIZE		(800)

//not call ctype.h ,use self is ok
//static int hx_isprint(int c)
//{
//	return c>=0x20 && c<127;
//}
/*
 * return 0 is got,others are not
 */
int hxl_getc_noblock(HX_DEV *d,int *c)
{
	int res;
	unsigned char cc;
	res = hx_read(d,&cc,1);
	if(res==1)
		*c = cc;
	return res==1?0:-1;
}
void hxl_rxclr(HX_DEV *d)
{
/*	int res;
	do{
		res = hxl_getc_noblock(d,NULL);
	}while(res==0);*/
}
int hxl_getc_block(HX_DEV *d)
{
	int res;
	int c;
	for(;;){
		res = hxl_getc_noblock(d,&c);
		if(res>=0)
			break;
	}
	return c;
}
int hxl_getc_timeout(HX_DEV *d, int *c, int timeout)
{
	int res;
	hx_do_timeout(timeout){
		res = hxl_getc_noblock(d,c);
		if(res>=0)
			break;
	}
	return res;
}
/*
	retrun >=0 is has \r\n
	<0 is null
*/
int hxl_gets_noblock(HX_DEV *d, char *buff, int buff_size)
{
	return hx_ioctl(d,IOCTL_GETS_NOBLOCK,buff,buff_size);
}
void hxl_gets_block(HX_DEV *d, char *buff, int buff_size)
{
	int res;
	for(;;){
		res = hxl_gets_noblock(d,buff,buff_size);
		if(res>=0)
			break;
	}
}
int hxl_gets_timeout(HX_DEV *d, char *buff, int buff_size,int timeout)
{
	int res;
	if(timeout<0 ||buff_size<=0 ||buff==NULL || d==NULL)
		return -1;
	hx_do_timeout(timeout){
		res = hxl_gets_noblock(d,buff,buff_size);
		if(res>=0)
			break;
	}
	return res;
}
int hxl_gets_timeout2(HX_DEV *d, char *buff, int buff_size,int timeout,int minchars)
{
	int res;
	if(timeout<0 ||buff_size<=0 ||buff==NULL || d==NULL)
		return -1;
	hx_do_timeout(timeout){
		res = hxl_gets_noblock(d,buff,buff_size);
		if(res>=minchars)
			break;
	}
	return res;
}
/*
	return >=0 is get line
	return <0 is nothing
*/
int hxl_gets_timeout3(HX_DEV *d, char *buff, int buff_size,int timeout,char *prefix)
{
	int res;
	if(prefix==NULL ||timeout<0 ||buff_size<=0 ||buff==NULL || d==NULL)
		return -1;
	int l = strlen(prefix);
	hx_do_timeout(timeout){
		res = hxl_gets_noblock(d,buff,buff_size);
		if(res>=l && (memcmp(prefix,buff,l)==0)){
			return res;
		}
	}
	return -1;
}
int hxl_gets_timeout_match(HX_DEV *d, char *buff,int buff_size,int timeout,
		const GETS_MATCH_T *tbl, int tblsize)
{
	int res;
	if(tbl==NULL ||timeout<0 ||buff_size<=0 ||buff==NULL || d==NULL)
		return -1;
	hx_do_timeout(timeout){
		res = hxl_gets_noblock(d,buff,buff_size);
		if(res>=0){
			for(int i=0;i<tblsize;i++){
				const char *str = tbl[i].str;
				int match0_prefix1_sufix2_contains3 = tbl[i].match0_prefix1_sufix2_contains3;
				if(match0_prefix1_sufix2_contains3 == 0){
					if(strcmp(str,buff)==0)
						return i;
				}else if(match0_prefix1_sufix2_contains3 == 1){
					int n = strlen(str);
					if(memcmp(str,buff,n)==0)
						return i;
				}else if(match0_prefix1_sufix2_contains3 == 2){
					int n = strlen(str);
					int m = strlen(buff);
					if(memcmp(str,&buff[m-n],n)==0)
						return i;
				}else if(match0_prefix1_sufix2_contains3 == 3){
					if(strstr(buff,str))
						return i;
				}else if(match0_prefix1_sufix2_contains3 == -1){
					return i;
				}
			}
		}
	}
	return -1;
}

/*
 * send
 */
int hxl_send(HX_DEV *d, const char *data, int len)
{
	return hx_write(d,data,len);
}
void hxl_putc(HX_DEV *d, int c)
{
	char _c = (char)c;
	hxl_send(d,&_c,1);
}
void hxl_put(HX_DEV *d, const char *s)
{
	hxl_send(d,s,strlen((const char *)s));
}
void hxl_puts(HX_DEV *d, const char *s)
{
	hxl_put(d,s);
	hxl_put(d,"\r\n");
}
void hxl_flush(HX_DEV *d)
{
	hxl_rxclr(d);
	hxl_send(d,NULL,0);
}



int hxl_vprintf(HX_DEV *d, const char *fmt, va_list va)
{
	int res;
	//use data or stack!
    //static  	
	char  buffer[VSPRINTF_BUFF_SIZE];
    res = vsprintf(buffer,fmt, va);
    hxl_put(d, buffer);
	hxl_send(d,NULL,0);
	return res;
}

int hxl_printf(HX_DEV *d, const char *fmt, ...)
{
	int res;
	va_list  va;
    va_start(va, fmt);
	res = hxl_vprintf(d,fmt, va);
    va_end(va);
	return res;
}

int hxl_open_uart(const char *name, int bps, HX_DEV *d)
{
	char param[20];
	sprintf(param,"%u,8N1",bps);
	return hx_open(name,param,d);
}
int hxl_open_stdio(int stdfile, HX_DEV *d)
{
	char param[2] = {0};
	param[0] = stdfile;
	return hx_open("stdio",param,d);
}

