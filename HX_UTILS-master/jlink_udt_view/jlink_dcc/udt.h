#pragma once
#include "stdafx.h"
#include <windows.h>
#include <typeinfo>
#include <conio.h>
#include "tlhelp32.h" 
#include<signal.h>
#include "direct.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

#pragma pack(push)
#pragma pack(1)
struct TRAD_T
{
	UINT32 flag;
	UINT32 data;
};
struct TRAD_CB
{
	TRAD_T tx;
	TRAD_T rx;
	BYTE txbuf[128];
	BYTE rxbuf[128];
};
#pragma pack(pop)

#define TRAD_OFFSET(m)		(trad_addr+offsetof(TRAD_CB,m))