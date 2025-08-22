#include "hx_utils.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#define atport		(&g_at_uart)
static HX_ATARG_T g_atarg = {0};
HX_NIC_INFO_T g_nic_info = {0};



void atc_get_arg(HX_ATARG_T *parg)
{
	*parg = g_atarg;
}
void atc_set_arg(const HX_ATARG_T *parg)
{
	g_atarg = *parg;
}

int atc_default_init(const struct HX_NIC_T *this, int *pstep, HX_ATARG_T *arg)
{
	if(pstep)
		*pstep = 0;
	if(arg)
		g_atarg = *(HX_ATARG_T*)arg;
	else
		g_atarg = *(this->default_arg);
	return 0;
}


int atc_init(const struct HX_NIC_T *nic, int *pstep, HX_ATARG_T *arg)
{
	if(nic->init)
		return nic->init(nic,pstep,arg);
	return 0;
}

void atc_reset(const struct HX_NIC_T *nic)
{
	if(nic->reset)
		nic->reset(nic);
	else
		nic->init(nic,0,NULL);
}


#define ATC_POLL_BUFSIZE		(4096)

/*
	arguements:
	nic	:	net card struct
	pstep : instans of current step
	msg : private data here
	return : current step

	return:
	normal return number is the current step.
	when all step continued , return sum of item of at_tbl.
*/
int atc_poll(const struct HX_NIC_T *nic, int *pstep, struct NET_PARAM_T *param)
{
	if(nic == NULL | pstep == NULL)
		return -1;
	char buff[ATC_POLL_BUFSIZE];
	const struct ATCMD_T *at_tbl = nic->at_tbl;
    int tbl_len = nic->at_tbl_count;
    int buff_size = ATC_POLL_BUFSIZE;
    int res;
	int step = *pstep;
    if(step >= tbl_len)
        return tbl_len;
    static int is_send = 0;
    int tmout_at_once = at_tbl[step].tmout_at_once;
    int try_times = at_tbl[step].trytimes;
    static int tmof_last_state_change = -1;
    if(tmof_last_state_change == -1)
        tmof_last_state_change = hx_get_tick_count();
    if(!is_send) {
        if(at_tbl[step].cmd) {	//if has cmd than send it
            //this is delay cmd ???
            if(try_times == -1 && tmout_at_once>0) {
                if(hx_check_timeout2(&tmof_last_state_change,tmout_at_once)) {
                    step ++ ;
                    is_send = 0;
                }
				*pstep = step;
                return step;
            } else {
                hxl_printf(&g_at_uart,"%s\r\n",at_tbl[step].cmd);
                is_send = 1;
                HX_DBG_PRINT("ATCMD_TX: [%s]\r\n",at_tbl[step].cmd);
            }
        } else if(at_tbl[step].callback) {	//has none, than callback
            memset(buff,0,buff_size);
            at_tbl[step].callback(step,AT_PUT,buff,param);
            hxl_printf(&g_at_uart,"%s\r\n",buff);
            is_send = 1;
            HX_DBG_PRINT("ATCMD_TX: [%s]\r\n",buff);
        } else {
            //no this stuation
            HX_DBG_PRINT("**error: ATCMD bad line:\r\n");
            HX_DBG_PRINT("\t step: %s\r\n", step);
            HX_DBG_PRINT("\t cmd: %s\r\n", at_tbl[step].cmd);
            HX_DBG_PRINT("\t want_res: %s\r\n", at_tbl[step].want_res);
            HX_DBG_PRINT("\t timeout_At_once: %d\r\n", at_tbl[step].tmout_at_once);
            HX_DBG_PRINT("\t trytimes: %d\r\n", at_tbl[step].trytimes);
            HX_DBG_PRINT("\t cmd: %d\r\n", at_tbl[step].callback);
        }
    }
again:
    res = hxl_gets_noblock(&g_at_uart,buff,buff_size);
    if(res>=0) {
        int get_ok = 0;
        HX_DBG_PRINT("\tRX: [%s]\r\n",buff);
        char *s = strtrim(buff,"\r\n ");
        if(at_tbl[step].want_res &&
                strcmp(buff,at_tbl[step].want_res)==0) {
            get_ok = 1;
        } else if(at_tbl[step].callback) {
            res = at_tbl[step].callback(step,AT_GET,s,param);
            if(res == 0)
                get_ok = 1;
        }
        if(get_ok) {
            step ++ ;
            is_send = 0;
            tmof_last_state_change = hx_get_tick_count();
            if(step == tbl_len) {
                HX_DBG_PRINT("\tModule Init Complete\r\n");
				*pstep = step;
                return -1;		//到头了
            } else {
				*pstep = step;
                return step;
            }
        } else {
            goto again;
        }
    } else {
        res = hx_check_timeout(tmof_last_state_change,tmout_at_once);
        if(res) {
            tmof_last_state_change = hx_get_tick_count();
            if(--try_times<0) {
                step = 0;
                is_send = 0;	//复位
				*pstep = step;
                return 0;
            } else {
                is_send = 0;	//重发
            }
        }
    }
	
	//if(step < nic->at_tbl_count){
	*pstep = step;
	return step;
	//}else{
	//	return -1;
	//}
}

int atc_getc_noblock(int *pc)
{
    return hxl_getc_noblock(&g_at_uart,pc);
}
int atc_getc_timeout(int *pc,int t)
{
    return hxl_getc_timeout(&g_at_uart,pc,t);
}
int atc_getc_block(int *pc)
{
    return hxl_getc_block(&g_at_uart);
}
int atc_gets_noblock(char *bf,int bl)
{
    return hxl_gets_noblock(&g_at_uart,bf,bl);
}
int atc_gets_timeout(char *bf,int bl,int t)
{  
	return hxl_gets_timeout(&g_at_uart,bf,bl,t);
}
void atc_gets_block(char *bf, int bl)
{   
	 hxl_gets_block(&g_at_uart,bf,bl);
}
void atc_rxclr(void)
{
     hxl_rxclr(&g_at_uart);
}
void atc_putc(int c)
{
     hxl_putc(&g_at_uart,c);
}
void atc_send(const char *d,int l)
{
     hxl_send(&g_at_uart,d,l);
}
void atc_put(const  char *s)
{
     hxl_put(&g_at_uart,s);
}
void atc_puts(const  char *s)
{
     hxl_puts(&g_at_uart,s);
}
int atc_printf(const char *format, ...)
{
    int res;
    //use data or stack!
    //static
    char  buffer[VSPRINTF_BUFF_SIZE];

    va_list  vArgs;
    va_start(vArgs, format);
    res = vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);
    hxl_put(&g_at_uart, buffer);
    hxl_send(&g_at_uart,NULL,0);
    return res;
}


