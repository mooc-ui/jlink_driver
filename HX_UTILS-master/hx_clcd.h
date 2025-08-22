#ifndef HX_CLCD_H__
#define HX_CLCD_H__

extern void hx_clcd_init(void);
extern void hx_clcd_clear(void);

extern void hx_clcd_draw_bmp(
	int col,	//colume
	int page,	//page
	int cols,	//colume count
	int pages,	//page count
	const void *bmpdata,	//data
	int inverse);	//0:normal,1:inverse
		
extern void hx_clcd_disp_char(int x,int y,char c,int inverse);
extern int hx_clcd_disp_chars(
	int x,		//列
	int y,		//行
	const char *s,	//数据
	int len,		//数据长度
	int inverse		//0正常,1反色
);
extern int hx_clcd_disp_str(
	int x,			//colume
	int y,			//row
	const char *s,	//data string
	int align,		//N=0 align left, 
					//N>0 align right with endian N of a line,
					//N<0 align middle with endian N of a line
	int mode,		//0:erase all disp area([x,abs(align)]),1:nothing be erased
	int inverse		//0:normal,1:inverse
);
extern void hx_clcd_disp_str2(int x,int y,const char *s,int align,int mode);

#endif
