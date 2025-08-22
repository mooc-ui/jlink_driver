#include "hx_utils.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#ifdef __HX_ENABLE_DEBUG__
int dbg_vprintf(HX_DEV *d,const char *fmt,va_list va)
{
	if(!g_enable_debug_output)
		return 0;
	return hxl_vprintf(d,fmt,va);
}
#else

int dbg_vprintf(HX_DEV *d,const char *fmt,va_list va)
{
	return 0;
}	

#endif


int hx_vprintf(HX_DEV *d,const char *fmt,va_list va)
{	
	return dbg_vprintf(d,fmt, va);
}

int hx_printf(const char *fmt,...)
{
	int res;
	va_list  va;
    va_start(va, fmt);
    res = dbg_vprintf(hxout,fmt, va);
    va_end(va);
	return res;
}
int hx_dprintf(HX_DEV *d,const char *fmt,...)
{
	int res;
	va_list  va;
    va_start(va, fmt);
    res = dbg_vprintf(d,fmt, va);
    va_end(va);
	return res;
}
#define SHOW_TAG(...)	//hxl_printf(__VA_ARGS__)
void hx_dbg(char ch,const char *fmt,...)
{
	if(!ch || !strchr(DISABLE_DEBUG_TAG,ch)){
		va_list  va;
		va_start(va, fmt);
		SHOW_TAG(hxout,"[%u]",(unsigned int)ch);
		dbg_vprintf(hxout,fmt,va);
		va_end(va);
	}
}
void hx_dbgi(char ch,const char *fmt,...)
{
	if(!ch || !strchr(DISABLE_DEBUG_TAG,ch)){
		va_list  va;
		va_start(va, fmt);
		SHOW_TAG(hxout,"[%u]",(unsigned int)ch);
		dbg_vprintf(hxin,fmt,va);
		va_end(va);
	}
}
void hx_dbge(char ch,const char *fmt,...)
{
	if(!ch || !strchr(DISABLE_DEBUG_TAG,ch)){
		va_list  va;
		va_start(va, fmt);
		SHOW_TAG(hxout,"[%u]",(unsigned int)ch);
		dbg_vprintf(hxerr,fmt,va);
		va_end(va);
	}
}
int hx_debug_enable(int en)
{
	int res = g_enable_debug_output;
	g_enable_debug_output = en;
	return res;
}

