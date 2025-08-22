SRCS:=
A_SRCS:=
C_SRCS:=
CXX_SRCS:=
VPATH:=

TARGET	=lpc2368_dq1200_gprs

#=====================================================
#srcs from keil mdk compile infomation
VPATH	+=./src
#SRCS+= comm_proc.c
SRCS+= comm.c
SRCS+= board.c
#SRCS+= com_module.c
#SRCS+= wcdma_net_protocol.c
SRCS+= wdt.c
SRCS+= irq.c
SRCS+= target.c
A_SRCS+= Startup.s
A_SRCS+= swi_handler.s
#SRCS+= rsa.c
SRCS+= uart.c
SRCS+= can.c
SRCS+= can_tran.c
SRCS+= TK485.c
SRCS+= key.c
SRCS+= led.c
SRCS+= timer.c
SRCS+= Recod.c
#SRCS+= GPRS.c
SRCS+= wcdma_drv.c
SRCS+= main.c
SRCS+= ComTrace.c
#SRCS+= sha1.c
SRCS+= tk_FunTool.c
SRCS+= lcd.c
SRCS+= rtc.c
SRCS+= disp.c
SRCS+= tkTransmit.c
SRCS+= SST25_flash.c
SRCS+= SST_FlashFun.c
SRCS+= wtv.c
SRCS+= tkSmartCard.c
SRCS+= card.c
#tkOperateFlash.c
SRCS+= M1_Card.c
SRCS+= cpu_card.c
SRCS+= Pboc_Card.c
SRCS+= RF24Card.c

C_SRCS	=$(SRCS)

#======================================================
# form mdk configure infomation
CFLAGS	:=\
-c --cpu ARM7TDMI -g -O0 --apcs=interwork -I..\..\DQ1200_LIB\TK_INC -I..\..\DQ1200_LIB\INC \
-I C:\Keil\ARM\RV31\INC \
-I C:\Keil\ARM\CMSIS\Include \
-I C:\Keil\ARM\Inc\Philips \
-D__DEBUG_FLASH -DLIB_TRACE_ -DDEBUG_PROGRAM -DWDT_DEBUG_OFF -D_BRIEF_TRACE_ \
--c99

#-D__HX_DEBUG__\



ASFLAGS	:=\
--cpu ARM7TDMI -g --apcs=interwork \
-I C:\Keil\ARM\RV31\INC \
-I C:\Keil\ARM\CMSIS\Include \
-I C:\Keil\ARM\Inc\Philips 



LDFLAGS :=\
--cpu ARM7TDMI \
--strict --scatter ".\Obj\release.sct" \
--entry 0x10000 --summary_stderr --info summarysizes --map --xref --callgraph --symbols \
--info sizes --info totals --info unused --info veneers \
--list "$(TARGET).map" 




#======================================================
# keil mdk tools
MDK_PATH	=C:\Keil
ARMCC_PATH	=$(MDK_PATH)\ARM\ARMCC\bin
VPATH	+= $(MDK_PATH)\ARM\inc
VPATH	+= $(MDK_PATH)\ARM\ARMCC\include

#======================================================
# toolchain
CROSS	=
CPP	=$(ARMCC_PATH)\armcc.exe -E
AS	=$(ARMCC_PATH)\armasm.exe
CC	=$(ARMCC_PATH)\armcc.exe
#CXX	=$(ARMCC_PATH)\armcc.exe
LD	=$(ARMCC_PATH)\armlink.exe
AR	=$(ARMCC_PATH)\armar.exe
OBJCOPY	=$(ARMCC_PATH)\fromelf.exe

#======================================================
#
#OBJS	:=
OBJS	+=$(CXX_SRCS:.cpp=.o)
OBJS	+=$(C_SRCS:.c=.o)
OBJS	+=$(A_SRCS:.s=.o)

DEPS	=$(OBJS:.o=.d)


ELF	=$(TARGET).axf
MDK_TARGET	:=.\Obj\debug.axf  

all:$(ELF)
	copy $(ELF) $(MDK_TARGET)
	
$(ELF):$(OBJS)
	@echo LD $^
	@$(LD)	$(LDFLAGS) $^ -o $(ELF)
	@echo Output Binary : .\obj\debug.bin
	@$(OBJCOPY) --bin -o .\obj\debug.bin $(ELF)
	

%.o:%.c	
	@echo CC $<
	@$(CC) $(CFLAGS) $< -o $@

%.o:%.s
	@echo AS $<
	@$(AS) $(ASFLAGS) $< -o $@
	
	

%.d:%.c
	@echo Check Depends : $<
	@$(CC) --mm $(CFLAGS) $< > $*.dep
	@sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" < $*.dep >$@
	@rm -f $*.dep
	
%.d:%.s
	@echo Check Depends : $<
	@$(CC) --mm $(CFLAGS) $< > $*.dep
	@sed "s,\($*\)\.o[ :]*,\1.o $@ : ,g" < $*.dep >$@
	@rm -f $*.dep
	




-include $(DEPS)

clean:
	-rm -rf $(TARGET).htm $(TARGET).map *.o *.axf $(MDK_TARGET) *.d *.dep





