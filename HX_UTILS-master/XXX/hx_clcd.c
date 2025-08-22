#include "hx_utils.h"
#include "hx_board.h"
#include "string.h"
#include "stdio.h"

#define CLCD_PXL_COLS			(240)		/*��������*/
#define CLCD_PXL_ROWS			(128)		/*��������*/
#define CLCD_CHAR_WIDTH			(8)			/*�ַ����*/
#define CLCD_CHAR_HEIGHT		(16)		/*�ַ��߶�*/
#define CLCD_CHAR_COLS			(CLCD_PXL_COLS/CLCD_CHAR_WIDTH)
#define CLCD_CHAR_ROWS			(CLCD_PXL_ROWS/CLCD_CHAR_HEIGHT)

void hx_clcd_disp_char(int x,int y,char c,int inverse)
{
	unsigned char s[2];
	s[0] = c;
	s[1] = 0;
	dprintf(x*CLCD_CHAR_WIDTH,y,s,inverse,0,30);
}
int hx_clcd_disp_chars(
	int x,			//��
	int y,			//��
	const char *s,	//����
	int len,		//���ݳ���
	int inverse		//0����,1��ɫ
)
{
	int i;
	for(i=0;i<len;i++){
		hx_clcd_disp_char(x+i,y,s[i],inverse);
	}
	return 0;
}
int hx_clcd_disp_str(
	int x,			//��
	int y,			//��
	const char *s,	//����
	int align,		//N=0�����, N>0�Ҷ��밴N���ַ���, N<0�м���밴��-N���ַ���
	int mode,		//0��������,1������
	int inverse		//0����,1��ɫ
)
{
	int res;
	int l,p;
	char buff[CLCD_CHAR_COLS];
	memset(buff,' ',CLCD_CHAR_COLS);
	l = strlen(s);
	if(l>(CLCD_CHAR_COLS-x))
		l= CLCD_CHAR_COLS-x;
	if(align==0)
		p=x;
	else if(align>0)
		p=x+align-l;
	else if(align<0){
		int hl = l/2;
		hl += (hl*2==l)?0:1;
		if(x-hl<0 || x+hl>CLCD_CHAR_COLS)
			return -1;
		p = x-hl;
	}	
	if(mode){
		memcpy(buff+p,s,l);
		res = hx_clcd_disp_chars(0,y,buff,CLCD_CHAR_COLS,inverse);
	}else{
		res = hx_clcd_disp_chars(p,y,buff,CLCD_CHAR_COLS,inverse);
	}
	return res;
}
void hx_clcd_disp_str2(int x,int y,const char *s,int align,int mode)
{
    hx_clcd_disp_str(x,y,s,align,mode,0);
}
