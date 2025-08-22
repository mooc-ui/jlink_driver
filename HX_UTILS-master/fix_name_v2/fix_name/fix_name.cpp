// fix_name.cpp : 定义控制台应用程序的入口点。
//



#include "stdafx.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


#include "..\..\..\hx_ver_def.h"


// #define __HOUXDs_TRICK__	"This Is Trick By Houxd Use For Find Version Str :"
const char *trick = __SEARCH_FLAG__;// __HOUXDs_TRICK__;
const int trick_size = strlen(trick);// sizeof(__HOUXDs_TRICK__) - 1;
size_t hex_to_asc(const void* hex, size_t hex_len, void *asc)
{
	unsigned int tmp;
	const unsigned char *_hex = (const unsigned char *)hex;
	unsigned char *_asc = (unsigned char *)asc;
	for (size_t i = 0; i<hex_len; i++)
	{
		tmp = (0xFu & (_hex[i] >> 4));
		_asc[(i << 1)] = (unsigned char)(tmp<0xA ? tmp + '0' : tmp + 'A' - 0xA);
		tmp = (0xFu & (_hex[i] >> 0));
		_asc[(i << 1) + 1] = (unsigned char)(tmp<0xA ? tmp + '0' : tmp + 'A' - 0xA);
	}
	return (hex_len << 1);
}
int main(int argc, char *argv[])
{
	int res;
	if (argc < 2) {
		printf("error: need input\r\n");
		goto end;
	}
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL) {
		printf("error: file open failure : %s\r\n",argv[1]);
		goto end;
	}
	fseek(f, 0, SEEK_END);
	int l = (int)ftell(f);
	if (l <= 0) {
		printf("error: file size<=0\r\n");
		goto end;
	}
	char *buff = (char*)malloc(l);	//文件数据
	fseek(f, 0 , SEEK_SET);
	int trys = l / 100;
	int n = 0;
	do {
		if (trys-- < 0)
			break;
		n += fread(buff, 1, l, f);
	} while (n < l);
	if (n != l) {
		printf("error: read %d need %d\r\n",n,l);
		goto end;
	}

	char *buff2 = (char*)malloc(l);	//这里拷贝一份用于操作,可能会修改
	memcpy(buff2, buff, l);

	char *pbuff = buff2;
	int i;
	for ( i = 0; i < l; i++) {
		if (memcmp(trick, pbuff, trick_size-10) == 0)
			break;
		pbuff++;
	}
	if (i == l) {
		printf("error: Cannot Find Flag String In This File !\r\n");
		printf("File Size : %d \r\n",l);
		goto end;
	}
	pbuff += trick_size;
	pbuff += 2;	//"$$"
	//char version_str[32];
	//memset(version_str, 0, 32);
	//hex_to_asc(pbuff, 4, version_str);
	//pbuff += 4;	//ver
	//pbuff += 2;	//"$$"

	char *ver_str = strtok(pbuff, "$$");
	char *rel_type = strtok(NULL, "$$");
	char *build_time = strtok(NULL, "$$");
	char *prog_name = strtok(NULL, "$$");
	char *p = build_time;
	while (*p) {
		if (*p == ':')
			*p = '.';
		if (*p == ' ')
			*p = '_';
		p++;
	}

	fclose(f);

	char cmd[512];
	sprintf(cmd, "copy \"%s\" \"./%s_%s_%s_%s.bin\"", argv[1], prog_name, ver_str, rel_type, build_time);
	res = system(cmd);
	if (res) {
		printf("error: when exec: %s !\r\n",cmd);
		goto end;
	}

	//写文件信息文件
	char tmp[256];
	sprintf(tmp,
		"program: %s\r\n"
		"version: %s\r\n"
		"build type: %s\r\n"
		"build time: %s\r\n",
		prog_name , ver_str, rel_type, build_time
	);
	FILE *f2 = fopen("./bin_info.txt", "w");
	fwrite(tmp, 1, strlen(tmp), f2);
	fclose(f2);

	////转换为公司的文件
	//sprintf(cmd, "VD7172_GPRS_Bin.exe %s", argv[1]);
	//res = system(cmd);
	//if (res) {
	//	printf("error: when exec: %s !\r\n", cmd);
	//	goto end;
	//}
	//sprintf(cmd, "move output.bin ./20%s00000000.bin", version_str);
	//res = system(cmd);
	//if (res) {
	//	printf("error: when exec: %s !\r\n", cmd);
	//	goto end;
	//}
	//FILE *f3 = fopen(cmd, "w");
	//fwrite(buff, 1, l, f3);
	////fwrite() 这里写公司头
	//fclose(f3);

	delete buff,buff2;

	//sprintf(cmd, "echo houxd > \"./prog_%s_%s_%s.bin\"", version_str, rel_type, build_time);
	////system(cmd);

	
	printf(tmp);

	printf("\r\nOperate Success!\r\n");
	getchar();
    return 0;
end:
	printf("any key to exit.\r\n");
	getchar();
	return -1;
}

