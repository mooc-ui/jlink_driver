#include "hx_dep_libs.h"
#include "hx_utils.h"

#ifdef HX_LIB_DS_RTC2TIME
time_t rtc2time(const struct rtctm *rtc_time)
{
	struct tm t;
	rtc2localtime(rtc_time,&t);
	return mktime(&t);
}
int time2rtc(time_t t, struct rtctm *rtc_time)
{
	struct tm *p_tm = localtime(&t);
	rtc_time->CLKSec = int2bcd(p_tm->tm_sec);
	rtc_time->CLKMin = int2bcd(p_tm->tm_min);
	rtc_time->CLKHour = int2bcd(p_tm->tm_hour);
	rtc_time->CLKDate = int2bcd(p_tm->tm_mday);
	rtc_time->CLKMonth = int2bcd(p_tm->tm_mon+1);
	unsigned int y = p_tm->tm_year + 1900;
	rtc_time->CLKYearH = int2bcd(y/100);
	rtc_time->CLKYearL = int2bcd(y%100);
	rtc_time->CLKWeek = int2bcd(p_tm->tm_wday+1);
	return 0;
}
int rtc2localtime(const struct rtctm *rtc_time, struct tm *t)
{
	t->tm_sec = bcd2int(rtc_time->CLKSec);
	t->tm_min = bcd2int(rtc_time->CLKMin);
	t->tm_hour = bcd2int(rtc_time->CLKHour);
	t->tm_mday = bcd2int(rtc_time->CLKDate);
	t->tm_mon = bcd2int(rtc_time->CLKMonth)-1;
	t->tm_year = bcd2int(rtc_time->CLKYearH)*100
		+bcd2int(rtc_time->CLKYearL) - 1900;
	t->tm_wday = bcd2int(rtc_time->CLKWeek)-1;
	t->tm_yday = 0;
	t->tm_isdst = 0;
	return 0;
}
#endif



