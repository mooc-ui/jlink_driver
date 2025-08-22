//#define HXUTILS_RTOS_SURPPORT
#ifdef HXUTILS_RTOS_SURPPORT

#include <RTL.H>

/*
	OS TickCount
*/
#define HX_OS_TICKCOUNT()	os_time_get()

/*
	OS Delay
*/
#define HX_OS_DELAY(ms)		os_dly_wait(ms)

/*
	OS Lock defines
*/
#define HX_OS_LOCK()		rt_tsk_lock()
#define HX_OS_UNLOCK()		rt_tsk_unlock()


/*
	Mutex defines
*/
#define HX_MUTEX_T	OS_MUT
#define HX_MUTEX_INIT(m)		os_mut_init(&(m))
#define HX_MUTEX_UNINIT(m)		
#define HX_MUTEX_WAIT(m)		os_mut_wait(&(m),0xFFFF)
#define HX_MUTEX_RELEASE(m)		os_mut_release(&(m))

#else
#define HX_OS_TICKCOUNT()
#define HX_OS_DELAY(ms)
#define HX_OS_LOCK()
#define HX_OS_UNLOCK()
#define HX_MUTEX_T			int
#define HX_MUTEX_INIT(m)
#define HX_MUTEX_UNINIT(m)				
#define HX_MUTEX_WAIT(m)		
#define HX_MUTEX_RELEASE(m)		

#endif


