/*
 * hx_utils.h
 *
 *  Created on: 2016Äê7ÔÂ25ÈÕ
 *      Author: houxd
 */

#ifndef __HX_UTILS_H__
#define	__HX_UTILS_H__

#define __HX_UTILS__

#include "int.h"

#include "hx_device.h"
#include "hx_term.h"
#include "hxl_serial.h"
#include "hxl_poll.h"
#include "hx_debug.h"
#include "hx_ver_def.h"
#include "hx_clcd.h"
#include "hx_debug.h"
#include "hxd_atc.h"
#include "hx_rtos_surpport.h"
#include "convert.h"

extern int hx_utils_init(void);
extern uint32_t hx_get_gpclk(void);

#define hx_do_timeout(tm_in_ms)						\
	for(int __t__ = hx_get_tick_count() + tm_in_ms;	\
		hx_get_tick_count()<__t__;)
#define hx_do_timeout1(tm_in_ms)						\
	for(int __t__1 = hx_get_tick_count() + tm_in_ms;	\
		hx_get_tick_count()<__t__1;)
#define hx_do_timeout2(tm_in_ms)						\
	for(int __t__2 = hx_get_tick_count() + tm_in_ms;	\
		hx_get_tick_count()<__t__2;)
		
#define hx_do_timeout_r(__t__,tm_in_ms)						\
	for(int __t__ = hx_get_tick_count() + tm_in_ms;	\
		hx_get_tick_count()<__t__;)

#define hx_trys(id,times)							\
	for(int id=times;id>0;id--)
		
#define foreach(array,count,pointer) \
	for((pointer)=(array);(pointer)-(array)<(count);(pointer)++)

extern uint32_t hx_get_tick_count(void);
extern void hx_init_tickcount(int pclk);

extern int hx_delay_ms(int ms);
extern int hx_check_timeout3(int *tlist, int tid, uint32_t timeout);
extern int hx_check_timeout2(int *last,uint32_t timeout);
extern int hx_check_timeout(int last,uint32_t timeout);
extern void check_key_to_menu(void);


#endif


