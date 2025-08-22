#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <rt_sys.h>
#include <stdlib.h>

#ifdef __DEBUG__
#define __DCC_ENABLE_RETARGET__
#endif


//int main(void)
//{
//	for(int i=0;i<16;i++)
//		fprintf((FILE*)i,"stdin");
//	fprintf(stdin,"\r\nstdin");
//	fprintf(stdout,"stdout");
//	fprintf(stderr,"stderr");
//	fflush(stdout);
//	while(1);
//	
//}



#ifndef uint
#define byte	unsigned char
#define uchar	unsigned char
#define uint	unsigned int
#define ushort	unsigned short
#endif

#define DCC_OP_READ_U32   0x01000000u
#define DCC_OP_READ_U16   0x02000000u
#define DCC_OP_READ_U8    0x04000000u
#define DCC_OP_GET_CAPS   0x08000000u
#define DCC_OP_WRITE_U32  0x10000000u
#define DCC_OP_WRITE_U16  0x20000000u
#define DCC_OP_WRITE_U8   0x40000000u
#define DCC_OP_ODD_ADDR   0x80000000u
#define DCC_OP_COMMAND    0x00000001u

#define DCC_SIGNATURE     0x91CA0000u
#define DCC_CONFIG        0xFFu       // All features are supported

#define DCC_CHANNEL_TERMINAL  0u
#define DCC_CHANNEL_OS        1u

#ifdef __ICCARM__
#define OS_ARM_FUNC  __arm                                          /* parasoft-suppress MISRA2004-19_4 "Compiler specific keyword." */
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
#define OS_ARM_FUNC
#define OS_INTERWORK
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
#define OS_ARM_FUNC
#define OS_INTERWORK
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
static OS_INTERWORK OS_ARM_FUNC uint _ReadDCCStat(void) ATTRIBUTE_NAKED_NO_INLINE;
static OS_INTERWORK OS_ARM_FUNC uint _ReadDCCStat(void) {
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
static OS_INTERWORK OS_ARM_FUNC uint _ReadDCC(void) ATTRIBUTE_NAKED_NO_INLINE;
static OS_INTERWORK OS_ARM_FUNC uint _ReadDCC(void) {
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
static OS_INTERWORK OS_ARM_FUNC void _WriteDCC(uint Data) ATTRIBUTE_NAKED_NO_INLINE;
static OS_INTERWORK OS_ARM_FUNC void _WriteDCC(uint Data) {
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

//static void jlink_dcc_putc(int ch, int d) {
//    uint packet;
//    uint busy;
//    packet = 0x93800000 | (ch << 16);
//    packet |= d;
//    do {
//        busy = _ReadDCCStat();
//    } while((busy & 2u));
//    _WriteDCC(packet);
//}
//static void jlink_osterm_send_char(int c)
//{
////    int i;
//    unsigned int sum = 2;
//    jlink_dcc_putc(DCC_CHANNEL_OS, 0x8C);
//    jlink_dcc_putc(DCC_CHANNEL_OS, 0xED);
//    jlink_dcc_putc(DCC_CHANNEL_OS, 2);
//    jlink_dcc_putc(DCC_CHANNEL_OS, 'C');
//    sum += 'C';
//    sum += c;
//    jlink_dcc_putc(DCC_CHANNEL_OS,c);
//    jlink_dcc_putc(DCC_CHANNEL_OS,0xFFu & sum);
//    jlink_dcc_putc(DCC_CHANNEL_OS,0x8D);
//}
//static void jlink_osterm_send(const char *s, int len)
//{
//    int i;
//    unsigned int sum = len+1;
//    jlink_dcc_putc(DCC_CHANNEL_OS, 0x8C);
//    jlink_dcc_putc(DCC_CHANNEL_OS, 0xED);
//    jlink_dcc_putc(DCC_CHANNEL_OS, len+1);
//    jlink_dcc_putc(DCC_CHANNEL_OS, 'C');
//    sum += 'C';
//    for(i=0; i<len; i++) {
//        sum += s[i];
//        jlink_dcc_putc(DCC_CHANNEL_OS,s[i]);
//    }
//    jlink_dcc_putc(DCC_CHANNEL_OS,0xFFu & sum);
//    jlink_dcc_putc(DCC_CHANNEL_OS,0x8D);
//}
//void jlink_osterm_put(const char *s)
//{
//    jlink_osterm_send(s,strlen(s));
//}
//void jlink_osterm_puts(const char *s)
//{
//    jlink_osterm_put(s);
//    jlink_osterm_put("\n");
//}
static void jlink_dcc_putc(int ch, int d) {
    uint packet;
    uint busy;
	//54 <reverse> <color8> <d8>
    packet = 0x54000000 | ((ch&0xFFu) << 8) | (d&0xFFu);
    do {
        busy = _ReadDCCStat();
    } while((busy & 2u));
    _WriteDCC(packet);
}
static int jlink_dcc_getc(int *d)
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
//==================================================
struct __FILE {
    int handle; /* Add whatever you need here */
};
FILE __stdout;
FILE __stdin;
FILE __stderr;
int fclose(FILE* f) {
    return (0);
}
int fseek (FILE *f, long nPos, int nMode)  {
    return (0);
}
int fflush (FILE *f)  {
    return (0);
}
int ferror(FILE *f) {
    return (EOF);
}

#ifdef __DCC_ENABLE_RETARGET__
//#pragma import(__use_no_semihosting_swi)
int fgetc_noblock(FILE *f) {
	int res;
	int c;
	(void)f;
	res = jlink_dcc_getc(&c);
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
int _fputc(int c, FILE *f) {
	int ch = (int)f;
	if(f==&__stdin) ch='g';		//green
	if(f==&__stdout) ch='w';	//white
	if(f==&__stderr) ch='r';	//red
	jlink_dcc_putc(ch,c);
	return c;
	
//	if(pos>=PUT_BUF_SIZE){
//		flush();
//	}
//    buff[pos] = c;
//    pos++;
//    return c;
	
//	jlink_osterm_send_char(c);
//	return c;
}
int fputc(int c, FILE *f){
//	if(c == '\t'){
//		_fputc(' ',f);
//		_fputc(' ',f);
//		_fputc(' ',f);
//		return _fputc(' ',f);
//	}else 
//	
	if(c == '\n'){
		_fputc('\r',f);
		return _fputc('\n',f);
	}else{
		return _fputc(c,f);
	}
	
	//_printf(
}

#else
int fgetc(FILE *f){(void*)f;return EOF;}
int fputc(int c, FILE *f){(void)c;(void)f;return c;}	
#endif

