#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rt_sys.h>
#include <rt_misc.h>
#include <stdarg.h>

#pragma anon_unions


/* if not retarget stdio,please call below
extern int xprintf(const char *fmt,...);
extern int xgetc_noblock(int *c);
extern int xputc(int ch,int c);
*/

/*
#include "D:/HX_UTILS/jlink_udt.h"
int main(void) 
{
	fprintf(stdin,"AAA\n");
	fprintf(stdout,"BBB\n");
	fprintf(stderr,"CCC\n");
	while(1)
	{
		int c = getchar();
		if(c!=EOF)
		{
			printf("getc: %c\n",(char)c);
		}
	}
}

*/

/*
	!!! Importent
	Dont use microlib.
	The macros _MICROLIB is not work!
	And :tt stdfile name cannot be echange
*/


/*
surpport debug interface:
	JLINK_DCC
	JLINK_DMI_TERM
*/

#ifdef __DEBUG__
#define UDT_ENABLE	
#define USE_RETARGET		/* can use stdlib printf() */
#endif

#if !defined(UDT_ENABLE) || !defined(USE_RETARGET)
#define RETARGET_DUMMY		/* use empty retarget. if not, printf() will dead */
#endif

//refrance rtx_lib.c
#if   (__TARGET_ARCH_6S_M || __TARGET_ARCH_7_M || __TARGET_ARCH_7E_M)
 #define ARCH_CORTEXM
#elif (__TARGET_ARCH_7_R)
 #define ARCH_ARMCR
#else
 #define ARCH_ARM79
#endif

//#if (__TARGET_ARCH_ARM == 0)	//refrance keil help
//#define ARCH_CORTEXM
//#else
//#define ARCH_ARM79
//#endif

#if defined(ARCH_CORTEXM)
	#define JLINK_DMI_TERM
#elif defined(ARCH_ARM79)
	#define USE_JLINK_DCC
#else
	#define USE_JLINK_DCC
#endif


#ifdef USE_JLINK_DCC
//#define STDIO_READ		jlink_dcc_read
#define STDIO_WRITE		jlink_dcc_write
#define STDIO_GETC		jlink_dcc_getc
#endif

#ifdef JLINK_DMI_TERM
//#define STDIO_READ		jlink_dmit_read
#define STDIO_WRITE		jlink_dmit_write
#define STDIO_GETC		jlink_dmit_getc
#endif

#if !defined(STDIO_WRITE) || !defined(STDIO_GETC)
//#define STDIO_READ		jlink_dcc_read
#define STDIO_WRITE		jlink_dcc_write
#define STDIO_GETC		jlink_dcc_getc
#endif

//static int STDIO_READ(int f, unsigned char *buf,unsigned len);
int STDIO_WRITE(int f, const unsigned char *buf,unsigned len);
int STDIO_GETC(int *c);


////=====================================================================

#if defined(UDT_ENABLE) && defined(USE_RETARGET)
//#ifdef _MICROLIB
//#pragma import(__use_full_stdio)
//#endif
#pragma import(__use_no_semihosting)

#define CONFIG_BUFSIZ		(128)

//#define STDIN_BUFSIZ		CONFIG_BUFSIZ
#define STDOUT_BUFSIZ		CONFIG_BUFSIZ
#define STDERR_BUFSIZ		CONFIG_BUFSIZ

#define STDIN             (0)
#define STDOUT            (1)
#define STDERR            (2)

const char __stdin_name[] =  ":stdin";
const char __stdout_name[] =  ":stdout";
const char __stderr_name[] =  ":stderr";


//static char stdin_buff[STDIN_BUFSIZ];
static char stdout_buff[STDOUT_BUFSIZ];
static char stderr_buff[STDERR_BUFSIZ];

FILEHANDLE _sys_open(const char *name, int openmode)
{
	(void)openmode;
	if(strcmp(name,__stdin_name)==0){
		//setvbuf(stdin,stdin_buff,_IOFBF,STDIN_BUFSIZ);
		setvbuf(stdin,0,_IONBF,0);
		return STDIN;
	}else if(strcmp(name,__stdout_name)==0){
		setvbuf(stdout,stdout_buff,_IOFBF,STDOUT_BUFSIZ);
		return STDOUT;
	}else if(strcmp(name,__stderr_name)==0){
		setvbuf(stderr,stderr_buff,_IOFBF,STDERR_BUFSIZ);
		return STDERR;
	}else{
		return (FILEHANDLE)name[0];
	}
}
int _sys_close(FILEHANDLE fh)
{
	(void)fh;
	return 0;
}
int _sys_write(FILEHANDLE fh, const unsigned char *buf,
               unsigned len, int mode)
{
	int res;
	(void)mode;
	if(fh<0)
		return -1;
	res = STDIO_WRITE((int)fh,buf,len);
	if(res == len)
		return 0;
	//return -1;
	return 0;
}
int _sys_read(FILEHANDLE fh, unsigned char *buf,
              unsigned len, int mode)
{
	(void)mode;
//	if(fh<0||fh>2)
//		return -1;
//	return STDIO_READ((int)fh,buf,len);
	return 0;
}
void _ttywrch(int ch)
{
  fputc(ch, stdout);
  fflush(stdout);
}
int _sys_istty(FILEHANDLE fh)
{
	return 1;
	//return 0; /* buffered output */
}
int _sys_seek(FILEHANDLE fh, long pos)
{
	(void)fh;
	(void)pos;
	return -1; /* not supported */
}
long _sys_flen(FILEHANDLE fh)
{
	(void)fh;
	return -1; /* not supported */
}
void _sys_exit(int rc) 
{
	(void)rc;
}


int fgetc_noblock(FILE *f) {
	int res;
	int c;
	(void)f;
	res = STDIO_GETC(&c);
	if(res)
		return 0xffu & c;
    return EOF;
}
int fgetc_block(FILE *f) {
	int res;
	do{
		res = fgetc_noblock(f);
	}while(res == EOF);
	return res;
}
int fgetc(FILE *f) {
	return fgetc_noblock(f);
}

#else
#ifdef RETARGET_DUMMY
const char __stdin_name[] =  ":tt";
const char __stdout_name[] =  ":tt";
const char __stderr_name[] =  ":tt";

FILEHANDLE _sys_open(const char *name, int openmode)
{
  return 1; /* everything goes to the same output */
}
int _sys_close(FILEHANDLE fh)
{
  return 0;
}
int _sys_write(FILEHANDLE fh, const unsigned char *buf,
               unsigned len, int mode)
{
	return 0;
}
int _sys_read(FILEHANDLE fh, unsigned char *buf,
              unsigned len, int mode)
{
  return -1; /* not supported */
}
void _ttywrch(int ch)
{
  (void)ch;
}
int _sys_istty(FILEHANDLE fh)
{
  return 0; /* buffered output */
}
int _sys_seek(FILEHANDLE fh, long pos)
{
  return -1; /* not supported */
}
long _sys_flen(FILEHANDLE fh)
{
  return -1; /* not supported */
}
void _sys_exit(int rc) 
{
	(void)rc;
}
#endif //RETARGET_DUMMY
#endif
//=====================================================
#if defined(UDT_ENABLE) && !defined(USE_RETARGET)
int xprintf(const char *fmt,...)
{
	int res,len;
	char buf[1024];
	va_list va;
	va_start(va,fmt);
	len = vsprintf(buf,fmt,va);
	res = STDIO_WRITE(1,(void*)buf,len);
	va_end(va);
	return res;
}
int xputc(int ch,int c)
{
	unsigned char cc = c;
	return STDIO_WRITE(ch,&cc,1);
}
int xgetc_noblock(int *pc)
{
	int res;
	int c;
	res = STDIO_GETC(&c);
	if(res){
		*pc = 0xffu & c;
		return 0;
	}
    return -1;
}

#endif

//=================================================================

#ifndef uint
#define byte	unsigned char
#define uchar	unsigned char
#define uint	unsigned int
#define ushort	unsigned short
#endif


//=================================================================
#ifdef USE_JLINK_DCC

#ifdef __ICCARM__
	#define __ARM_FUNC  __arm    
	#define ATTRIBUTE_NAKED_NO_INLINE

	#if (defined (__ARM7A__) && (__CORE__ == __ARM7A__)) || (defined (__ARM7R__) && (__CORE__ == __ARM7R__))
		#define __ARMv7A__ 1
		#undef __ARMv5__
	#else
		#define __ARMv5__ 1
		#undef __ARMv7A__
	#endif
#endif


#ifdef __GNUC__
	#define __ARM_FUNC
	#define __INTERWORK
	#define ATTRIBUTE_NAKED_NO_INLINE __attribute__ ((naked, noinline)) /* parasoft-suppress MISRA2004-19_4 "Compiler specific keyword." */

	#if (defined (__ARM_ARCH_7A__) && (__ARM_ARCH_7A__ == 1)) || (defined (__ARM_ARCH_7R__) && (__ARM_ARCH_7R__ == 1))
		#define __ARMv7A__ 1
		#undef __ARMv5__
	#else
		#define __ARMv5__ 1
		#undef __ARMv7A__
	#endif

	#pragma GCC diagnostic ignored "-Wreturn-type"      // Avoid warning for missing return value with GCC
	#pragma GCC diagnostic ignored "-Wunused-parameter" // Avoid warning for unused parameter with GCC
#endif

#ifdef __CC_ARM
	#define __ARM_FUNC
	#define __INTERWORK
	#define ATTRIBUTE_NAKED_NO_INLINE __attribute__ ((noinline))        /* parasoft-suppress MISRA2004-19_4 "Compiler specific keyword." */
	#pragma arm

	#if (defined (__TARGET_ARCH_ARM) && (__TARGET_ARCH_ARM == 7))
		#define __ARMv7A__ 1
		#undef __ARMv5__
	#else
		#define __ARMv5__ 1
		#undef __ARMv7A__
	#endif
#endif

//
// The bit positions for DCC RX and TX ready are different for ARMv5 and ARMv7,
// so we have to distinguish here.
//
#if (defined (__ARMv7A__) && (__ARMv7A__ == 1))
	#define RX_FULL_FLAG_SHIFT 30
	#define TX_FULL_FLAG_SHIFT 29
#else
	#define RX_FULL_FLAG_SHIFT 0
	#define TX_FULL_FLAG_SHIFT 1
#endif
/*********************************************************************
*
*       _ReadDCCStat()
*
*  IAR macro:     unsigned long __MRC(coproc, opcode_1, CRn, CRm, opcode_2);
*  Inst:          MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move from coproc to ARM reg
*  ARMv4 / ARMv5: MRC p14,0,<Rt>,c0,c0,0
*  ARMv7-AR:      MRC p14,0,<Rt>,c0,c1,0 DBGDSCR
*/
static __INTERWORK __ARM_FUNC uint _ReadDCCStat(void) ATTRIBUTE_NAKED_NO_INLINE;
static __INTERWORK __ARM_FUNC uint _ReadDCCStat(void) {
#if (defined (__ARMv7A__) && (__ARMv7A__ == 1))
#ifdef __ICCARM__
    return __MRC(14u, 0u, 0u, 1u, 0u);
#elif defined (__GNUC__)
    __asm(".arm\n\
           mrc p14, 0, r0, c0, c1, 0\n\
           bx  lr\n");
#elif defined (__CC_ARM)
    register unsigned int r;
    r = 0;  // Set initial value to avoid compiler warning
    __asm
    {
        mrc p14, 0, r, c0, c1, 0
    }
    return r;
#endif
#else
#ifdef __ICCARM__
    return __MRC(14u, 0u, 0u, 0u, 0u);
#elif defined (__GNUC__)
    __asm(".arm\n\
           mrc p14, 0, r0, c0, c0, 0\n\
           bx  lr\n");
#elif defined (__CC_ARM)
    register unsigned int r;
    r = 0;  // Set initial value to avoid compiler warning
    __asm
    {
        mrc p14, 0, r, c0, c0, 0
    }
    return r;
#endif
#endif
}

/*********************************************************************
*
*       _ReadDCC()
*
*  IAR macro:     unsigned long __MRC(coproc, opcode_1, CRn, CRm, opcode_2);
*  Inst:          MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move from coproc to ARM reg
*  ARMv4 / ARMv5: MRC p14,0,<Rt>,c1,c0,0
*  ARMv7-AR:      MRC p14,0,<Rt>,c0,c5,0 DTRRX
*/
static __INTERWORK __ARM_FUNC uint _ReadDCC(void) ATTRIBUTE_NAKED_NO_INLINE;
static __INTERWORK __ARM_FUNC uint _ReadDCC(void) {
#if (defined (__ARMv7A__) && (__ARMv7A__ == 1))
#ifdef __ICCARM__
    return __MRC(14u, 0u, 0u, 5u, 0u);
#elif defined (__GNUC__)
    __asm(".arm\n\
           mrc p14, 0, r0, c0, c5, 0\n\
           bx  lr\n");
#elif defined (__CC_ARM)
    register unsigned int r;
    r = 0;  // Set initial value to avoid compiler warning
    __asm
    {
        mrc p14, 0, r, c0, c5, 0
    }
    return r;
#endif
#else
#ifdef __ICCARM__
    return __MRC(14u, 0u, 1u, 0u, 0u);
#elif defined (__GNUC__)
    __asm(".arm\n\
           mrc p14, 0, r0, c1, c0, 0\n\
           bx  lr\n");
#elif defined (__CC_ARM)
    register unsigned int r;
    r = 0;  // Set initial value to avoid compiler warning
    __asm
    {
        mrc p14, 0, r, c1, c0, 0
    }
    return r;
#endif
#endif
}

/*********************************************************************
*
*       _WriteDCC()
*
*  IAR macro:     void __MCR(coproc, opcode_1, Data, CRn, CRm, opcode_2);
*  Inst:          MCR<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}  Move to coproc
*  ARMv4 / ARMv5: MCR P14,0,<Rt>,C1,C0,0
*  ARMv7-AR:      MCR p14,0,<Rt>,c0,c5,0 DTRTX
*/
static __INTERWORK __ARM_FUNC void _WriteDCC(uint Data) ATTRIBUTE_NAKED_NO_INLINE;
static __INTERWORK __ARM_FUNC void _WriteDCC(uint Data) {
#if (defined (__ARMv7A__) && (__ARMv7A__ == 1))
#ifdef __ICCARM__
    __MCR(14u, 0u, Data, 0u, 5u, 0u);
#elif defined (__GNUC__)
    __asm(".arm\n\
           mcr p14, 0, r0, c0, c5, 0\n\
           bx  lr\n");
#elif defined (__CC_ARM)
    __asm
    {
        mcr p14, 0, Data, c0, c5, 0
    }
#endif
#else
#ifdef __ICCARM__
    __MCR(14u, 0u, Data, 1u, 0u, 0u);
#elif defined (__GNUC__)
    __asm(".arm\n\
           mcr p14, 0, r0, c1, c0, 0\n\
           bx  lr\n");
#elif defined (__CC_ARM)
    __asm
    {
        mcr p14, 0, Data, c1, c0, 0
    }
#endif
#endif
}

//=============================================================================
static void dcc_write_u32(uint d) {
    uint busy;
	do {
        busy = _ReadDCCStat();
    } while((busy & 2u));
    _WriteDCC(d);
}
//static int dcc_read_u32(uint *d)
//{
// uint Data;

//  if (((_ReadDCCStat() >> RX_FULL_FLAG_SHIFT)  & 1u) == 0u) {       // Data or command received ?
//    return 0;      // Nothing received
//  }
//  Data = _ReadDCC();
//  if((Data&0xFF000000u) == 0x55000000u){
//	*d = Data & 0xFFu;
//	return 1;
//  }
//  return 0;
//}
static uchar make_sum(uchar init,const uchar *buf,uint len)
{
	int i;
	uchar res = init;
	for(i=0;i<len;i++){
		res += buf[i];
	}
	return res;
}
static uint array2uint(const uchar *_data, uint len)
{
	const uchar *d = _data;
    uint res = 0L;
	if(len==1){
		res = ((uint)d[0])<<24;
	}else if(len==2){
		res = ((uint)d[0])<<24 | ((uint)d[1])<<16;
	}else if(len==3){
		res = ((uint)d[0])<<24 | ((uint)d[1])<<16 | ((uint)d[2])<<8;
	}else {//if(len==4){
		res = ((uint)d[0])<<24 | ((uint)d[1])<<16 | ((uint)d[2])<<8 | ((uint)d[3]);
	}
    return res;
}
static uchar jlink_dcc_send_frame(uchar color, const uchar *buf, uchar len)
{
	/*
	proctol:
	StartFrame              DataFrame        EndFrame
	---------------------   -------------    ------------------  
	sof cmd1  cmd2   dlen   [data...] ...    eof sum1 rfs1 rfs2
	FE  C1/C2 color  <256                    F0  
	*/
	int i;
	uint dlen = len & 0x0FFu;
	uint StartFrame = 0xFEC10000u | (color<<8) | dlen;
	uint sum = make_sum(dlen,buf,len);
	uint EndFrame = 0xF0000000u | (sum<<16);
	uint l;
	if(buf==NULL || len>128)
		return 0;
	dcc_write_u32(StartFrame);
	l = len>>2<<2;
	for(i=0;i<l;i+=4){
		uint d = array2uint(&buf[i],4);
		dcc_write_u32(d);
	}
	if(len>l){
		dcc_write_u32(array2uint(&buf[l],len-l));
	}
	dcc_write_u32(EndFrame);
	return len;
}
static int jlink_dcc_putc(int ch, int d) {
    uint packet;
    uint busy;
	//54 <reverse> <color8> <d8>
    packet = 0x54000000 | ((ch&0xFFu) << 8) | (d&0xFFu);
    do {
        busy = _ReadDCCStat();
    } while((busy & 2u));
    _WriteDCC(packet);
	return d;
}
int jlink_dcc_write(int f, const unsigned char *buf, unsigned len)
{
	uint l;
	uint i;
	uint ch = 'r';
	if(f==0) ch='g';	//green
	else if(f==1) ch='w';	//white
	else if(f==2) ch='r';	//red
	else ch = f;
	if(len==1){
		int c = *buf;
		if(c == '\n'){
		jlink_dcc_putc(ch,'\r');
			return jlink_dcc_putc(ch,'\n');
		}else{
			return jlink_dcc_putc(ch,c);
		}
	}
	l = len>>7<<7;
	for(i=0;i<l;i+=128){
		jlink_dcc_send_frame(ch,&buf[i],128);
	}
	if(len>l){
		jlink_dcc_send_frame(ch,&buf[l],len-l);
	}
	return len;
}
int jlink_dcc_getc(int *d)
{
  uint Data;

  if (((_ReadDCCStat() >> RX_FULL_FLAG_SHIFT)  & 1u) == 0u) {       // Data or command received ?
    return 0;      // Nothing received
  }
  Data = _ReadDCC();
  if((Data&0xFF000000u) == 0x55000000u){
	*d = Data & 0xFFu;
	return 1;
  }
  return 0;
}
#endif
//=======================================================================
#ifdef JLINK_DMI_TERM

//must be in ram
#define TRAD_CTRLBLK_ADDR			(0x10008000 - 512)		

//terminal use ram access directlly  
typedef __packed struct __TRAD
{
	unsigned int flag;
	unsigned int data;
} TRAD_T;

typedef __packed struct __TRAD_CB
{
	TRAD_T tx;
	TRAD_T rx;
	uchar txbuf[128];
	uchar rxbuf[128];
} TRAD_CB;

static TRAD_CB trad_cb __attribute__((at(TRAD_CTRLBLK_ADDR))) = {0};

static int jlink_dmit_putc(int ch,int c)
{
	uint res;
		
	trad_cb.tx.data = c | 0x54000000|(ch<<8);
	trad_cb.tx.flag = 1;
	do{
		res = trad_cb.tx.flag;
	}while(res);
	return 0;
}

static int jlink_dmit_send(int ch,const uchar *s,int len)
{
	uint res;	
	memcpy(trad_cb.txbuf,s,len);
	trad_cb.tx.data = len | 0x5F000000|(ch<<8);
	trad_cb.tx.flag = 1;
	do{
		res = trad_cb.tx.flag;
	}while(res);
	return 0;
}

int jlink_dmit_getc(int *c)
{
	int res;
	res = trad_cb.rx.flag;
	if(res){
		res = trad_cb.rx.data;
		if((res&0xFF000000u) == 0x55000000u ){
			trad_cb.rx.data = 0x00000000;
			*c = res & 0x0FFu;
			return 1;
		}
		trad_cb.rx.flag = 0x00000000;
	}
	return 0;
}
int jlink_dmit_write(int f, const unsigned char *buf, unsigned len)
{
	uint ch = 'r';
	if(f==0) ch='g';	//green
	else if(f==1) ch='w';	//white
	else if(f==2) ch='r';	//red
	else ch=f;
	if(len==1){
		jlink_dmit_putc(ch,buf[0]);
	}else{
		int i=0;
		for(i=0;i<len;i+=128){
			int l = len-i;
			if(l>=128)
				l = 128;
			jlink_dmit_send(ch,buf,len);
		}
	}
	return len;
}

#endif





