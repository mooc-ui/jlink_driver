#include "hx_utils.h"
//#include "hx_board.h"
#include "string.h"
#include "stdio.h"

/*
	CLCD 排列
	col->
page||||....|||
  |	||||....|||
  V	....
	||||....|||

地址增长方向: 先col后page
*/

#define CLCD_PXL_COLS			(240)		/* colume count in pixel*/
#define CLCD_PXL_ROWS			(128)		/* row count in pixel*/
#define CLCD_PAGE_SIZE			(8)			/* page size in pixel */

#define CLCD_CHAR_WIDTH			(8)			/* ascii char width in pixel , wchar is double ascii*/
#define CLCD_CHAR_HEIGHT		(16)		/* char height in pixel */
#define CLCD_CHAR_COLS			(CLCD_PXL_COLS/CLCD_CHAR_WIDTH)
#define CLCD_CHAR_ROWS			(CLCD_PXL_ROWS/CLCD_CHAR_HEIGHT)


extern void clcd_set_pos(int col,int page);
extern void clcd_wr_data(unsigned char data);
extern void *font_ascii(int c);
extern void *font_wchar(int wc, void *buf32byte);
extern void lcd_init(void);
extern void lcd_clear(void);
/*
void clcd_set_pos(int col,int page)
{
	SetPageAddress(page);
    SetColumnAddress(col);
	SetRAMAddress(1); //SET RAM ADDRESS CONTROL
}
void clcd_wr_data(unsigned char data)
{
	write_data(data);
}
void *font_ascii(int c)
{
    unsigned char j;
    unsigned int i;
    WDTFeed();        //喂狗
    SelectSSTFlash(tkFLASH_NO_2);
    i=(c-0x20);
    for(j=0; j<16; j++)
    {
        sstFlashRead(ASCstartpage+(i/(tkFLASH_PAGE_SIZE/16)),(16*(i%(tkFLASH_PAGE_SIZE/16)))+j,&LCD_tmp[j],1);
    }
    //draw_bmp(col,page,8,2,LCD_tmp,mode,ifmenu);
    SelectSSTFlash(tkFLASH_NO_1);
	return LCD_tmp;
}
void *font_wchar(int wc)
{
	QueryHzk(wc);
    return LCD_tmp;
}
*/

/*
void *font_wchar(int wc, void *buf32byte)
{
	char buf[32];
	unsigned code = (unsigned)wc - 0xA0A0u ;
    unsigned section = (unsigned char)(code>>8) ;
    unsigned position=(unsigned char)(code) ;
    unsigned ofs=((section-1)*94+(position-1)) * 32;
	int blk = ofs/1024;
	int pos = ofs%1024;
	flash_read(0,1+blk,0+pos,32,buf32byte);
	return buf32byte;
}

*/


void hx_clcd_init(void)
{
	HX_OS_LOCK();
	lcd_init();
	HX_OS_UNLOCK();
}
void hx_clcd_clear(void)
{
	HX_OS_LOCK();
	lcd_clear();
	HX_OS_UNLOCK();
}

void hx_clcd_draw_bmp(
	int col,	//colume
	int page,	//page
	int cols,	//colume count
	int pages,	//page count
	const void *bmpdata,	//data
	int inverse)	//0:normal,1:inverse
{
	const unsigned char *d = bmpdata;
	int x=0,y=0;
	int page_end = page+pages;
	int col_end = col+cols;
	HX_OS_LOCK();
	for(int p=page;p<page_end;p++){
		clcd_set_pos(col,p);
		for(int c=col;c<col_end;c++){
			char dd = d[y*cols+x];
			if(inverse)
				dd = ~dd;
			clcd_wr_data(dd);
			x++;
		}
		x=0;
		y++;
	}
	HX_OS_UNLOCK();
}
static void hx_clcd_disp_ascii(int x,int y,char c,int inverse)
{
	int page = y * (CLCD_CHAR_HEIGHT/CLCD_PAGE_SIZE);
	int pages = CLCD_CHAR_HEIGHT/CLCD_PAGE_SIZE;
	void *font = font_ascii(c);
	hx_clcd_draw_bmp(x*CLCD_CHAR_WIDTH,page,CLCD_CHAR_WIDTH,pages,
		font,inverse);
}
static void hx_clcd_disp_wchar(int x,int y,unsigned short c,int inverse)
{
	char buff[32];
	int page = y * (CLCD_CHAR_HEIGHT/CLCD_PAGE_SIZE);
	int pages = CLCD_CHAR_HEIGHT/CLCD_PAGE_SIZE;
	void *font = font_wchar(c,buff);
	hx_clcd_draw_bmp(x*CLCD_CHAR_WIDTH,page,CLCD_CHAR_WIDTH*2,pages,
		font,inverse);
}
static int hx_clcd_disp_chars(
	int x,			//colume
	int y,			//row
	const char *s,	//data
	int len,		//data length
	int inverse		//0:normal,1:inverse
)
{
	int i;
	for(i=0;i<len;i++){
		if(s[i]>128){
			unsigned short wc = (unsigned char)s[i];
			wc<<=8;
			wc += (unsigned char)s[i+1];
			hx_clcd_disp_wchar(x+i,y,wc,inverse);
			i++;
		}else{
			hx_clcd_disp_ascii(x+i,y,s[i],inverse);
		}
	}
	return 0;
}
static int _abs(int n)
{
	return n<0?-n:n;
}
int hx_clcd_disp_str(
	int x,			//colume
	int y,			//row
	const char *s,	//data string
	int align,		//N=0 align left, 
					//N>0 align right with endian N of a line,
					//N<0 align middle with endian N of a line
	int mode,		//0:erase all disp area([x,abs(align)]),1:nothing be erased
	int inverse		//0:normal,1:inverse
)
{
	int res;
	int l,p;
	char buff[CLCD_CHAR_COLS];
	memset(buff,' ',CLCD_CHAR_COLS);
	l = strlen(s);
	int endx = _abs(align);
	if(align==0){	//left align end with lcd width
		endx = CLCD_CHAR_COLS;
	}else {				//right align endx must less than lcd width
						//middle align endx must less than lcd width 
						//and strlen must less than disp area
		if(endx>CLCD_CHAR_COLS)
			endx = CLCD_CHAR_COLS;
	}
	int dl = endx-x;	//show area length
	//check data len
	if(l>dl)
		l=dl;
	//calc pos of first disp char
	if(align==0)
		p=x;
	else if(align>0)	//right align
		p=endx-l;
	else if(align<0)	//middle align
		p = (dl-l)/2;
	
	if(mode==0){
		memcpy(buff+p,s,l);	
		res = hx_clcd_disp_chars(0,y,buff,dl,inverse);
	}else{
		res = hx_clcd_disp_chars(p,y,s,l,inverse);
	}
	return res;
}

void hx_clcd_disp_text(char *dbuf)
{
	//lcd_clear();
	char *e;
	char *p = strtok_r(dbuf,"\n",&e);
	p = hx_strtrim2(p,"\r");
	int i=0;
	while(p && i<8){
		hx_clcd_disp_str(0,i++,p, 0, 0, 0);
		p = strtok_r(NULL,"\n",&e);
		p = hx_strtrim2(p,"\r");
	}
}
