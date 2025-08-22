#ifndef HX_DEP_LIBS_H
#define HX_DEP_LIBS_H


#ifdef HX_LIB_DS_RTC2TIME
#include "time.h"
#include "rtc.h"
extern time_t rtc2time(const struct rtctm *rtc_time);
extern int time2rtc(time_t t, struct rtctm *rtc_time);
extern int rtc2localtime(const struct rtctm *rtc_time, struct tm *t);
#endif

#endif
