#ifndef __VERSION__


#define __VERSION__		"\x00\x00\x00\x00"


#ifdef __RELEASE__
	#define __RELTYPE__		"release"
#endif
#ifdef __DEBUG__
	#define __RELTYPE__		"debug"
#endif
#ifndef __RELTYPE__
	#define __RELTYPE__		"unknown"
#endif

#endif
