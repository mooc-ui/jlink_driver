#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rt_sys.h>
#include <rt_misc.h>
#include <stdarg.h>
#pragma anon_unions

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
#endif


#if (__TARGET_ARCH_ARM == 0)	//refrance keil help
#define ARCH_CORTEXM
#else
#define ARCH_ARM79
#endif

#if defined(ARCH_CORTEXM)
	#define JLINK_DMI_TERM
#elif defined(ARCH_ARM79)
	#define USE_JLINK_DCC
#else
	#define USE_JLINK_DCC
#endif


#ifdef USE_JLINK_DCC
#define STDIO_PUTC		jlink_dcc_putc
#define STDIO_GETC		jlink_dcc_getc
#endif

#ifdef JLINK_DMI_TERM
#define STDIO_PUTC		jlink_dmit_putc
#define STDIO_GETC		jlink_dmit_getc
#endif

#if !defined(STDIO_PUTC) || !defined(STDIO_GETC)
#define STDIO_PUTC		jlink_dcc_putc
#define STDIO_GETC		jlink_dcc_getc
#endif


int STDIO_PUTC(int ch, int c);
int STDIO_GETC(int *c);


//=====================================================================

int fflush (FILE *f)  {
    return (0);
}
int ferror(FILE *f) {
    return (EOF);
}

#if defined(UDT_ENABLE)
int fgetc(FILE *f)
{
	return STDIO_GETC();
}
int fputc(int c, FILE *f)
{
	(void)c;
	int ch;
	if(f==stdin) ch='g';	//green
	else if(f==stdout) ch='w';	//white
	else if(f==stderr) ch='r';	//red
	else ch=(int)f;
	return STDIO_PUTC(ch,c);
}	


#else
int fgetc(FILE *f){(void*)f;return EOF;}
int fputc(int c, FILE *f){(void)c;(void)f;return c;}	

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
#define TRAD_CTRLBLK_ADDR			(0x10001000)		

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

int jlink_dmit_getc(void)
{
	int res;
	res = trad_cb.rx.flag;
	if(res){
		res = trad_cb.rx.data;
		if((res&0xFF000000u) == 0x55000000u ){
			trad_cb.rx.data = 0x00000000;
			return res & 0x0FFu;
		}
		trad_cb.rx.flag = 0x00000000;
	}
	return EOF;
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





