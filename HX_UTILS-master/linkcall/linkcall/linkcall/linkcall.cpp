// linkcall.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "string.h"
#include "stdio.h"
#include "windows.h"

int main(int argc, const char **argv)
{
	char buff[1024] = { 0 };
	strcat(buff, argv[0]);
	const char *p = strrchr(argv[0], '\\');
	if (p) {
		buff[p - argv[0]+1] = 0;
	}
	strcat(buff, "conf.cmd");
	for (int i = 1; i < argc; i++) {
		strcat(buff, " ");
		strcat(buff, argv[i]);
	}
	return system(buff);
}

