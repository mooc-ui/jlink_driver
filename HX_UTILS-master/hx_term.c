/*
 * hx_term.c
 *
 *  Created on: 2016Äê7ÔÂ25ÈÕ
 *      Author: houxd
 */

#include "hx_utils.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "hx_device.h"


#define HXT_TRIM_CHAR_LIST		" \r\n\t"



/*
	extern cmd list
*/
extern const struct HXT_CMD_T g_cmd_list[];
/*
	cmd sum
*/
extern int g_cmd_count;
/*
sample :
const struct HXT_CMD_T g_cmd_list[] = {
	{"help",	hxt_help,		"ÏÔÊ¾°ïÖúÐÅÏ¢"},
	{"?",		hxt_help,		"ÏÔÊ¾°ïÖúÐÅÏ¢"},
	{"version",	hxt_version,	"ÏÔÊ¾°æ±¾"},

};
int g_cmd_count = sizeof(g_cmd_list)/sizeof(g_cmd_list[0]);
*/


struct HXT_CMD_CONTEXT_T
{
	const struct HXT_CMD_T *cmd;
	int argc;
	char **argv;
};

struct HXT_CMDL_BUF_T
{
	char buff[SIZE_OF_LINE_BUFFER];
	char *add;
	char *del;
	char *cur;
};



static int last_cmd_str(char *buf);
static int next_cmd_str(char *buf);

extern const struct HXT_CMD_T g_cmd_list[];
extern int g_cmd_count;
static char *g_argv[MAX_ARGS_COUNT];

static struct HXT_CMDL_BUF_T g_cmdl_buf;
static char g_gets_buf[MAX_CHARS_LINE];
static int g_gets_count=0;
static int g_cursor_pos=0;
static int g_ctrl_mode = 0;


int g_enable_debug_output = HX_OUTPUT_STATE_DEFAULT;



//not call ctype.h ,use self is ok
int hx_isprint(int c)	
{
	return c>=0x20 && c<127;
}
//------------------------------------------------------

int hxt_getc_noblock(int *c)
{
	return hxl_getc_noblock(&hx_stdin,c);
}
void hxt_put_char(int c) 
{
	hxl_putc(&hx_stdout,c);
}
void hxt_fput_char(HX_DEV *d,int c) 
{
	hxl_putc(d,c);
}



/*
	only two case can return :
	<0:now is no char
	>=0:has a token
	-'' : is esc got
*/
static int _hxt_gets_echo(char *buff, int buf_size);
//this func for export
int hxt_gets_echo_noblock(char **s)
{
	int res = _hxt_gets_echo(g_gets_buf,MAX_CHARS_LINE);
	if(res>=0)
		*s = g_gets_buf;
	return res;
}
static int hxt_gets_echo(char *buff, int buf_size)
{
	int res;
	do{
		res = _hxt_gets_echo(buff,buf_size);
		if(IS_CTRL_CHAR(-res)){
			int c = CTRL2CHAR(-res);
			switch(c){
				case 'D':
					#ifdef __HX_ENABLE_DEBUG__
					g_enable_debug_output = g_enable_debug_output?0:1;
					hxt_printf("Debug Output %s \r\n",g_enable_debug_output?
						"Enable":"Disable");
					#else
					hxt_printf("*** Debug Output Function Diabled!\r\n");
					#endif
					break;
				default:
					break;
			}
		}
	}while(IS_CTRL_CHAR(-res));
	return res;
}


/*
	only two case can return :
	<0:now is no char
	>=0:has a token
*/
static int _hxt_gets_echo(char *buff, int buf_size)
{
	int res;
	int c;
	while(1){
		res = hxt_getc_noblock(&c);
		if(res)
			return -1001;
		
		if(g_ctrl_mode){	//ctrl char
			g_ctrl_mode = 0;
			switch(c){
			case '[':
				g_ctrl_mode = 2;
				continue;
			case 0x41:
				//up
				if(g_ctrl_mode == 3)
					return -333;
	//			hxt_put("[A");
				res = last_cmd_str(buff);
				if(res){
					for(int i=0;i<g_gets_count;i++){
						hxt_put_char('');
						hxt_put_char(' ');
						hxt_put_char('');
					}
					g_gets_count = res;
					g_cursor_pos = res;
					buff[g_gets_count] = 0;
					hxt_put(buff);
				}
				continue;
			case 0x42:
				if(g_ctrl_mode == 3)
					return -333;
	//			hxt_put("[B");
				res = next_cmd_str(buff);
				if(res){
					for(int i=0;i<g_gets_count;i++){
						hxt_put_char('');
						hxt_put_char(' ');
						hxt_put_char('');
					}
					g_gets_count = res;
					g_cursor_pos = res;
					buff[g_gets_count] = 0;
					hxt_put(buff);			
				}
				continue;
			case 0x43:
				if(g_ctrl_mode == 3)
					return -333;
				if(g_cursor_pos < g_gets_count){
					g_cursor_pos++;
					hxt_put("[C");
				}
				continue;
			case 0x44:
				if(g_ctrl_mode == 3)
					return -333;
				if(g_cursor_pos>0){
					g_cursor_pos --;
					hxt_put("[D");
				}
				continue;
			case 0x35:
				g_ctrl_mode = 3;
				continue;
			case 0x36:
				g_ctrl_mode = 3;
				continue;
			default:
				if(hx_isprint(c))
					return c;
				g_ctrl_mode = 0;
				return -'';
				//continue;
			}
		}
		if(c=='\n' || c=='\r'){		//endl
			hxt_put_char('\r');
			hxt_put_char('\n');
			if(g_gets_count>0){
				memcpy(buff,g_gets_buf,g_gets_count);
				buff[g_gets_count] = 0;
				g_gets_count = 0;
				g_cursor_pos = 0;
			}else{
				buff[0] = 0;
				g_gets_count = 0;
				g_cursor_pos = 0;
			}
			return 0;
		}else if(c==''){			//bs
			if(g_cursor_pos>0){
				int tmp = g_gets_count - g_cursor_pos;
				if( tmp > 0 ){
					memmove(&g_gets_buf[g_cursor_pos-1],
						&g_gets_buf[g_cursor_pos], tmp);
				}
				g_gets_count --;
				g_cursor_pos --;
				hxt_put_char('\b');
				if(tmp>0){
					for(int i=g_cursor_pos;i<g_gets_count;i++){
						hxt_put_char(g_gets_buf[i]);
					}
					hxt_put_char(' ');
					while(tmp-->0)
						hxt_put_char('\b');
					hxt_put_char('\b');
				}else{
					hxt_put_char(' ');
					hxt_put_char('\b');
				}
			}
			continue;
		}else if(c == ''){		//ctrl
			g_ctrl_mode = 1;
			//return -'';		//-0x1B
		}else if(IS_CTRL_CHAR(c)){
			return -c;			//ctrl-char
		}else{
			if(!hx_isprint(c))
				continue;
			if(g_gets_count<(MAX_CHARS_LINE-1)){
				int tmp = g_gets_count - g_cursor_pos;
				if( tmp > 0 ){
					memmove(&g_gets_buf[g_cursor_pos+1],
						&g_gets_buf[g_cursor_pos], tmp);
				}
				g_gets_buf[g_cursor_pos] = c;
				g_cursor_pos ++;
				g_gets_count ++;
		
				hxt_put_char(c);
				if(tmp>0){
					for(int i=g_cursor_pos;i<g_gets_count;i++){
						hxt_put_char(g_gets_buf[i]);
					}
					while(tmp-->0)
						hxt_put_char('\b');
				}
			}
			continue;
		}
	}
}
int hxt_putc(int c)
{
	hxt_put_char(c);
	return 0;
}
int hxt_put(const char *s)
{
	hxl_put(&hx_stdout,s);
	return 0;
}

int hxt_fput(HX_DEV *d, const char *s)
{
	hxl_put(d,s);
	return 0;
}

int hxt_puts(const char *s)
{
	hxt_put(s);
	hxt_put("\r\n");
	return 0;
}

static char *hxt_strtrim(char **p, const char *trim_list)
{
	char *s_beg = *p;
	char *s_end = s_beg + strlen(s_beg) - 1;
	while(strchr(trim_list,*s_end) && s_end > s_beg)
		*s_end-- = 0;	
	while(strchr(trim_list,*s_beg) && s_beg < s_end)
		*s_beg++ = 0;
	*p = s_beg;
	return s_beg;
}

static int find_cmd(char *buff, struct HXT_CMD_CONTEXT_T *cmd_info)
{
	g_argv[0] = strtok(buff," ");
	hxt_strtrim(&g_argv[0],HXT_TRIM_CHAR_LIST);
	const struct HXT_CMD_T *cmd;
	foreach(g_cmd_list,g_cmd_count,cmd)
	{
		//cmd = &g_cmd_list[i];
		if(strcmp(g_argv[0],cmd->cmd_str)==0){
			int i;
			for(i=1;i<MAX_ARGS_COUNT;i++){
				char *tmp = strtok(NULL," ");
				if(tmp == NULL)
					break;
				hxt_strtrim(&tmp,HXT_TRIM_CHAR_LIST);
				g_argv[i] = tmp;
			}
			if(i==MAX_ARGS_COUNT){
				if(strtok(NULL," ") != NULL){
					hxt_puts("ERROR: too many args can not be execute!");
					return -2;
				}
			}
			cmd_info->cmd = cmd;
			cmd_info->argv = g_argv;
			cmd_info->argc = i;
			return 0;
		}
	}
	hxt_puts("ERROR: bad commad !");
	return -1;
}

static int calc_empty(void)
{
	if(g_cmdl_buf.add >= g_cmdl_buf.del){
		return SIZE_OF_LINE_BUFFER -(g_cmdl_buf.add - g_cmdl_buf.del);
	}else{
		return g_cmdl_buf.del - g_cmdl_buf.add;
	}
}
static char *find_zero_forward(
	char *buf_beg,
	char *buf_end,
	char *find_beg,
	char *find_end)
{
	if(find_beg==find_end)
		return NULL;
	if(find_beg<find_end){
		char * p =find_beg;
		while(p<find_end){
			if(*p ==0)
				break;
			p++;
		}
		if(p<find_end)
			return p;
		return NULL;
	}else{
		char * p =find_beg;
		while(p<buf_end){
			if(*p ==0)
				break;
			p++;
		}
		if(p<buf_end)
			return p;
		p =buf_beg;
		while(p<find_end){
			if(*p ==0)
				break;
			p++;
		}
		if(p<find_end)
			return p;
		return NULL;
	}
}
/*static char *find_zero_backword(
	char *buf_beg,
	char *buf_end,
	char *find_beg,
	char *find_end)
{
	if(find_beg==find_end)
		return NULL;
	if(find_beg>find_end){
		char * p =find_beg;
		while(p>=find_end){
			if(*p ==0)
				break;
			p--;
		}
		if(p>=find_end)
			return p;
		if(find_end == buf_beg)
			return buf_beg;
		return NULL;
	}else{
		char * p =find_beg;
		while(p>=buf_beg){
			if(*p ==0)
				break;
			p--;
		}
		if(p>=buf_end)
			return p;
		p =buf_end;
		while(p>=find_end){
			if(*p ==0)
				break;
			p--;
		}
		if(p>=find_end)
			return p;
		if(find_end == buf_beg)
			return buf_beg;
		return NULL;
	}
}*/
static int del_a_cmd_str(void)
{
	if(g_cmdl_buf.del < g_cmdl_buf.add){
		for(char *p=g_cmdl_buf.del;
				p<&g_cmdl_buf.buff[SIZE_OF_LINE_BUFFER];p++){
			if(*p == 0){
				if(p == &g_cmdl_buf.buff[SIZE_OF_LINE_BUFFER])
					g_cmdl_buf.del = g_cmdl_buf.buff;
				else
					g_cmdl_buf.del = p+1;
				return 0;
			}
		}
	}
	for(char *p=g_cmdl_buf.buff;p<g_cmdl_buf.add;p++){
		if(*p == 0){
			if(p == g_cmdl_buf.add)
				g_cmdl_buf.del = g_cmdl_buf.add;
			else
				g_cmdl_buf.del = p+1;
			return 0;
		}
	}
	return -1;
}

static int add_a_cmd_str(const char *cmd_str,int cmd_len)
{
	char *p = g_cmdl_buf.add;
	for(int i=0;i<cmd_len;i++){
		if(p == &g_cmdl_buf.buff[SIZE_OF_LINE_BUFFER])
			p = g_cmdl_buf.buff;
		if(p == g_cmdl_buf.del-1)
			return -1;
		*p++ = cmd_str[i];
	}
	*p++ = 0;
	g_cmdl_buf.add = p;
	g_cmdl_buf.cur = g_cmdl_buf.add;
	return 0;
}

static int check_cmd_exsit(const char *cmd)
{
	return 0;
}

static int add_cmdl_buff(const char *cmd_str)
{
	int res;
	int cmd_len = strlen(cmd_str);
	if(cmd_len>=SIZE_OF_LINE_BUFFER)
		return -2;
	res = check_cmd_exsit(cmd_str);
	if(res)
		return 0;
	int empty = calc_empty();
	while(empty<=cmd_len)
	{
		res = del_a_cmd_str();
		if(res)
			break;	//have no data to delete
		empty = calc_empty();
	}
	if(empty<=cmd_len)
		return -1;
	return add_a_cmd_str(cmd_str,cmd_len);
}
static int last_cmd_str(char *buf)
{
	// find str form add inverse
	if(g_cmdl_buf.cur == g_cmdl_buf.del){
		return 0;
	}else if(g_cmdl_buf.cur > g_cmdl_buf.del){
		char *p = g_cmdl_buf.cur-2;	//end has space 
		while(p>g_cmdl_buf.del){
			if(*p==0)
				break;
			p--;
		}
		if(p>=g_cmdl_buf.del){
			if(*p==0)p++;
			if(*p==NULL)
				return 0;
			memcpy(buf,p,g_cmdl_buf.cur-p);
			g_cmdl_buf.cur = p;
			return strlen(buf);
		}
	}else{
		char *p = g_cmdl_buf.cur-2;	//end has space 
		while(p>g_cmdl_buf.buff){
			if(*p==0)
				break;
			p--;
		}
		if(p>=g_cmdl_buf.buff){
			if(*p==0)p++;
			if(*p==NULL)
				return 0;
			int n = g_cmdl_buf.cur-p;
			memcpy(buf,p,n);
			buf[n] = 0;
			g_cmdl_buf.cur = p;
			return strlen(buf);
		}
		
		p = g_cmdl_buf.buff+SIZE_OF_LINE_BUFFER-2;	//end has space 
		while(p>g_cmdl_buf.del){
			if(*p==0)
				break;
			p--;
		}
		if(p>=g_cmdl_buf.del){
			if(*p==0)p++;
			if(*p==NULL)
				return NULL;
			int tmp = g_cmdl_buf.buff+SIZE_OF_LINE_BUFFER-p;
			memcpy(buf,p,tmp);
			g_cmdl_buf.cur = p;
			int tmp2 = g_cmdl_buf.cur - g_cmdl_buf.buff;
			memcpy(buf+tmp,g_cmdl_buf.buff,tmp2);
			buf[tmp+tmp2] = 0;
			return strlen(buf);
		}
		return 0;
	}
	return 0;//g_cmdl_buf.cur
}

static int next_cmd_str(char *buf)
{
	// find str form add inverse
	char *p = find_zero_forward(
		g_cmdl_buf.buff,g_cmdl_buf.buff+SIZE_OF_LINE_BUFFER,
		g_cmdl_buf.cur,g_cmdl_buf.add);
	if(p==NULL)
		return 0;
	p++;
	char *q = find_zero_forward(
		g_cmdl_buf.buff,g_cmdl_buf.buff+SIZE_OF_LINE_BUFFER,
		p,g_cmdl_buf.add);
	if(q==NULL)
		return 0;
	if(p<q){
		int n = q-p;
		memcpy(buf,p,n);
		buf[n] = 0;
		g_cmdl_buf.cur = p;
		return strlen(buf);
	}else{
		int n = g_cmdl_buf.buff+SIZE_OF_LINE_BUFFER-p;
		memcpy(buf,p,n);
		g_cmdl_buf.cur = p;
		int m = q-g_cmdl_buf.buff;
		memcpy(buf+n,g_cmdl_buf.buff,m);
		buf[m+n] = 0;
		return strlen(buf);
	}
}
int hxt_term_init(void)
{
	g_cmdl_buf.cur = g_cmdl_buf.add =
		g_cmdl_buf.del = g_cmdl_buf.buff;
	for(int i=0;i<MAX_ARGS_COUNT;i++)
		g_argv[i] = NULL;
	hxt_exec_cmd("version");
	//show prompt
	hxt_put(HXT_PROMPT);
	return 0;
}

int hxt_exec_cmd(const char *cmd)
{
	char buff[MAX_CHARS_LINE];
	strncpy(buff,cmd,MAX_CHARS_LINE);
	int res;
	struct HXT_CMD_CONTEXT_T cmd_info;
	res = find_cmd(buff,&cmd_info);
	if(res)
		return -2;

	//excute this cmd
	res = cmd_info.cmd->p_main(cmd_info.argc,cmd_info.argv);
	
	g_gets_count=0;
	g_cursor_pos=0;
	g_ctrl_mode = 0;	
	
	return res;
}

int hxt_term_poll(void)
{
	int res;
	/*
		here is a loop , it no error !!!
	*/
	for(;;){
		//get line string
		char buff[MAX_CHARS_LINE];
		res = hxt_gets_echo(buff,MAX_CHARS_LINE);
		if(res)
			return -1;
		char *p = buff;
		hxt_strtrim(&p," \r\n\t");
		if(*buff){
			//add cmd to buffer list
			add_cmdl_buff(buff);

			res = hxt_exec_cmd(buff);
		}
		//show prompt
		hxt_put(HXT_PROMPT);
	}
}

int hxt_term_run(void)
{
	hxt_term_init();

	for(;;){
		hxt_term_poll();
	}
}
int hxt_printf(const char *fmt, ...)
{
	int res;
	va_list ap;
    char buff[MAX_CHARS_LINE];
    va_start (ap, fmt);
    res = vsprintf(buff,fmt,ap);
    va_end(ap);
    hxt_put(buff);
	return res;
}
int hxt_fprintf(HX_DEV *d,const char *fmt, ...)
{
	int res;
	va_list ap;
    char buff[MAX_CHARS_LINE];
    va_start (ap, fmt);
    res = vsprintf(buff,fmt,ap);
    va_end(ap);
	
    hxt_fput(d,buff);
	return res;
}

int hx_get_arg(int argc,char *argv[],const char *arg, char **res)
{
	if(argc<=0)
		return -2;
	for(int i=0;i<argc;i++){
		if(argv[i]==NULL)
			continue;
		char *s = hx_strtrim(argv[i]);
		int n = strlen(arg);
		if(memcmp(s,arg,n)==0 && s[n]=='='){
			*res = &s[n+1];
			return i;
		}
	}
	*res = NULL;
	return -1;
}
int hx_get_arg_bool(int argc,char *argv[],const char *arg,int* res)
{
	if(argc<=0)
		return -2;
	for(int i=0;i<argc;i++){
		if(argv[i]==NULL)
			continue;
		char *s = hx_strtrim(argv[i]);
		if(strcmp(s,arg)==0){
			*res = 1;
			return i;
		}
	}
	*res = 0;
	return -1;
}
int hx_get_arg_int(int argc,char *argv[],const char *arg, int *res)
{
	if(argc<=0)
		return -2;
	for(int i=0;i<argc;i++){
		if(argv[i]==NULL)
			continue;
		char *s = hx_strtrim(argv[i]);
		int n = strlen(arg);
		if(memcmp(s,arg,n)==0 && s[n]=='='){
			s = &s[n+1];
			if(s[n]==0 && (s[n+1]=='x' || s[n+1]=='X')){
				int n = sscanf(s,"0%*[xX]%x",res);
				if(n==1)
					return i;
				else
					return -2;
			}else{
				int n = sscanf(s,"%d",res);
				if(n==1)
					return i;
				else
					return -2;
			}
		}
	}
	*res = NULL;
	return -1;
}


HXT_DEF_PROC(version)
{
	hxt_puts("HX Utils R01 @16/8/17");
	hxt_printf("Build Time: %s %s\r\n",__DATE__,__TIME__);
	return 0;
}

HXT_DEF_PROC(help)
{
	hxt_puts("HX Term V0.1 (just for test)");
	hxt_puts("command list :");
	const struct HXT_CMD_T *cmd;
	foreach(g_cmd_list,g_cmd_count,cmd)
	{
		hxt_printf("\t%s\t\t%s\r\n",cmd->cmd_str,cmd->cmd_info);
	}
	return 0;
}

