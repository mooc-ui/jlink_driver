#ifndef __VERSION__


#define __VERSION__		"V01"

#define __PROG_NAME__	"dq1300_boot_0x0_0x10000"

#ifdef __RELEASE__
	#define __RELTYPE__		"release"
#endif
#ifdef __DEBUG__
	#define __RELTYPE__		"debug"
#endif
#ifndef __RELTYPE__
	#define __RELTYPE__		"unknown"
#endif

#define __SEARCH_FLAG__	"\xAA\x55 THis is FlaG fOr seArch thIs.\xFF\x01"

extern const char * hx_ver_init(void);

#endif
