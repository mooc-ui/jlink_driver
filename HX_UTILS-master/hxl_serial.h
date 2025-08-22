#ifndef HXL_SERIAL_H__
#define HXL_SERIAL_H__

#include "hx_utils.h"
#include "hx_device.h"
#include "stdarg.h"

/*
 * recv
 */
extern int hxl_getc_noblock(HX_DEV *d,int *c);
extern void hxl_rxclr(HX_DEV *d);
extern int hxl_getc_block(HX_DEV *d);
extern int hxl_getc_timeout(HX_DEV *d, int *c, int timeout);
/*
	retrun >=0 is has \r\n
	<0 is null
*/
extern int hxl_gets_noblock(HX_DEV *d, char *buff, int buff_size);
extern void hxl_gets_block(HX_DEV *d, char *buff, int buff_size);
extern int hxl_gets_timeout(HX_DEV *d, char *buff, int buff_size,int timeout);
extern int hxl_gets_timeout2(HX_DEV *d, char *buff, int buff_size,int timeout,int minchars);
extern int hxl_gets_timeout3(HX_DEV *d, char *buff, int buff_size,int timeout,char *prefix);
typedef struct {
	const char *str;
	/*
	 * -1 is for default
	 */
	int match0_prefix1_sufix2_contains3;
} GETS_MATCH_T;
/*
 * return <0 is timeout, >=0 is index of match
 */
int hxl_gets_timeout_match(HX_DEV *d, char *buff,int buff_size,int timeout,
		const GETS_MATCH_T *tbl, int tblsize);
/*
 * send
 */
extern int hxl_send(HX_DEV *d, const char *data, int len);
extern void hxl_putc(HX_DEV *d, int c);
extern void hxl_put(HX_DEV *d, const char *s);
extern void hxl_puts(HX_DEV *d, const char *s);
extern int hxl_vprintf(HX_DEV *d, const char *fmt, va_list va);
extern int hxl_printf(HX_DEV *d, const char *format, ...);
extern void hxl_flush(HX_DEV *d);
extern int hxl_open_uart(const char *name, int bps, HX_DEV *d);
extern int hxl_open_stdio(int stdfile, HX_DEV *d);


#endif

