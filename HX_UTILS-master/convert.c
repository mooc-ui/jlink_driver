#include "convert.h"
#include "stdio.h"
#include "string.h"

int bcd2int(unsigned char bcd)
{
	unsigned int tmp = bcd;
	return (0x0Fu&(tmp>>4))*10+(0x0Fu&(tmp));
}

unsigned char int2bcd(int d)
{
	return (unsigned char)(0x0F0u&((d/10)<<4))+(0x0Fu&(d%10));
}
// "\x12\x34" => "1234"
char* hx_dumphex(const void* bin,int bin_len,void *asc)
{
	unsigned int tmp;
	const unsigned char *_bin = bin;
	unsigned char *_asc = asc;
	for(size_t i=0;i<bin_len;i++)
	{
		tmp = (0xFu&(_bin[i]>>4));
		_asc[(i<<1)] = (unsigned char) (tmp<0xA?tmp+'0':tmp+'A'-0xA);
		tmp = (0xFu&(_bin[i]>>0));
		_asc[(i<<1)+1] = (unsigned char) (tmp<0xA?tmp+'0':tmp+'A'-0xA);
	}
	return asc;
}
// "\x12\x34" => "1234"
char* hx_dumphex2str(const void* bin,int bin_len,void *asc)
{
	hx_dumphex(bin,bin_len,asc);
	((char*)asc)[bin_len<<1] = 0;
	return asc;
}
// "1234" => "\12\x34"
char* hx_hexcode2bin(const void *hexcode, int len, void *bin)
{
    unsigned int tmp;
    int c,i;
    unsigned char *t = (unsigned char*)bin;
	const unsigned char *s = (const unsigned char *)hexcode;
	for(i=0;i<len;i+=2){
		tmp = s[i];
        c = 0x0F & (tmp>='A'?tmp-'A'+0x0A:tmp-'0');
        c<<=4;
        tmp = s[i+1];
        c += 0x0F & (tmp>='A'?tmp-'A'+0x0A:tmp-'0');
        *t++ = c;
	}
    return bin;
}

char *hx_strtrim2(char *p, const char *trim_list)
{
	char *s_beg = p;
	char *s_end = s_beg + strlen(s_beg) - 1;
	while(strchr(trim_list,*s_end) && s_end > s_beg)
		*s_end-- = 0;	
	while(strchr(trim_list,*s_beg) && s_beg < s_end)
		*s_beg++ = 0;
	//*p = s_beg;
	return s_beg;
}
char *hx_strtrim(char *s)
{
	return hx_strtrim2(s,"\t\r ");
}

//==========================================================
// convert str <=> value with fmt
/*
	<fmt>,count,split-chars
	"bcd n ."		"\x20\x16"		<=>	"2016"		this not surpport 
												count ,split-chars .
	"bin"											like before
	"asc 5"			'houxd123'		<=>	"houxd"		
	"%i 4 ."		[127][0][0][1]	<=> "127.0.0.1"
	"%d"			100	(DWORD)			<=>	"100"
	"%hd/%hu/%hx/%hX"		WORD
	"%hhd/%hhu/%hhx/%hhX"		BYTE
	"%lld/%llu/%hhx/%hhX"		QWORD
	"%c"
	"msb 2/4/8"
	"lsb 2/4/8"
*/
int hx_str2value(const char *s, const char *value_type, void *vres)
{
    char fmt[10] = {0};
    int count = -1;
    char split_chars[5] = {0};
	if(!s)
		return -1;
    char str[128] = {0};
	strncpy(str,s,127);
    int res = sscanf(value_type,"%s %d %s",fmt,&count,split_chars);
    if(res<=0)
        return -1;
    if(strcmp(fmt,"bcd")==0 || strcmp(fmt,"bin")==0) {
		if(str[0]==0)
			return -2;
        if(count<=0)
            return -3;
		int l = strlen(str);
		if(l>count){
			l=count;
		}
        hx_hexcode2bin(str,l,vres);
        return 0;
    } else if(strcmp(fmt,"asc")==0) {
        if(count<0)
            return -3;
        if(count>0){
			if(str[0]==0)
				return -2;
			memcpy(vres,str,count);
        }else
            strcpy(vres,str);
        return 0;
    } else if(strcmp(fmt,"msb")==0) {
		if(str[0]==0)
			return -2;
        uint64_t v;
        sscanf(str,"%llu",&v);
        if(count==2)
            HX_MSB_W2B(v,vres);
        else if(count==4)
            HX_MSB_DW2B(v,vres);
        else if(count==8)
            HX_MSB_QW2B(v,vres);
        else
            return -3;
        return 0;
    } else if(strcmp(fmt,"lsb")==0) {
		if(str[0]==0)
			return -2;
        uint64_t v;
        sscanf(str,"%llu",&v);
        if(count==2)
            HX_LSB_W2B(v,vres);
        else if(count==4)
            HX_LSB_DW2B(v,vres);
        else if(count==8)
            HX_LSB_QW2B(v,vres);
        else
            return -3;
        return 0;
    } else if(fmt[0]=='%') {
        //use printf/scan format
        if(res==1) {
            count = 1;
            split_chars[0] = 0;
            //sscanf(str,fmt,vres);
        } else if(res >= 2) {
            if(count<=0)
                return -3;
            if(count==1)	//count ==1, no need split char
                split_chars[0] = 0;
            else if(split_chars[0]==0) {
                //array big than 0, neet split char, default is '.'
                strcpy(split_chars,".");
            }
        } else {
            return -5;
        }
        int size = 0;
        if((strcmp(fmt,"%s")==0)){
			count = 1;
            size = 0;
		}else if((strcmp(fmt,"%c")==0)){
			count = 1;
            size = 1;
		}else if(strstr(fmt,"d")||strstr(fmt,"i")||strstr(fmt,"u")||strstr(fmt,"X")||strstr(fmt,"x")) {
            if(strstr(fmt,"hh"))
                size = 1;
            else if(strstr(fmt,"h"))
                size = 2;
            else if(strstr(fmt,"ll"))
                size = 8;
            else
                size = 4;
        } else {
            //...
        }
        if(count>1) {
            char *cvres = vres;
            char *p = strtok(str,split_chars);
            int i = 0;
            for(i=0; i<count-1; i++) {
                sscanf(p,fmt,cvres);
                if(size==0) {
                    int ll = strlen(cvres);
                    cvres[ll] = ' ';
                    cvres += ll+1;
                } else {
                    cvres += size;
                }
                p = strtok(NULL,split_chars);
            }
            sscanf(p,fmt,cvres);
        } else {
            sscanf(str,fmt,vres);
        }
        return 0;

    } else {
        return -9;
    }
}
int hx_value2str(const void* value,const char *format,
                 char *sres)
{
    strcpy(sres,"error");
    char fmt[10] = {0};
    int count = -1;
    char split_chars[5] = {0};
    if(!value)
        return -2;
    int res = sscanf(format,"%s %d %s",fmt,&count,split_chars);
    if(res<=0)
        return -1;
    if(strcmp(fmt,"bcd")==0||strcmp(fmt,"bin")==0) {
        if(count<=0)
            return -3;
        hx_dumphex2str(value,count,sres);
        return 0;
    } else if(strcmp(fmt,"asc")==0) {
        if(count<0)
            count = 0;
        if(count>0) {
            memcpy(sres,value,count);
            sres[count] = 0;
        } else
            strcpy(sres,value);
        return 0;
    } else if(strcmp(fmt,"msb")==0) {
        uint64_t v;
        if(count==2)
            v = HX_MSB_B2W(value);
        else if(count==4)
            v = HX_MSB_B2DW(value);
        else if(count==8)
            v = HX_MSB_B2QW(value);
        else
            return -3;
        sprintf(sres,"%llu",v);
        return 0;
    } else if(strcmp(fmt,"lsb")==0) {
        uint64_t v;
        if(count==2)
            v = HX_LSB_B2W(value);
        else if(count==4)
            v = HX_LSB_B2DW(value);
        else if(count==8)
            v = HX_LSB_B2QW(value);
        else
            return -3;
        sprintf(sres,"%llu",v);
        return 0;
    } else if(fmt[0]=='%') {
        //use printf/scan format
        if(res==1) {
            count = 1;
            split_chars[0] = 0;
            //sprintf(sres,fmt,*(uint32_t*)value);
        } else if(res >= 2) {
            if(count<=0)
                return -3;
            if(count==1)	//count ==1, no need split char
                split_chars[0] = 0;
            else if(split_chars[0]==0) {
                //array big than 0, neet split char, default is '.'
                strcpy(split_chars,".");
            }
        } else {
            return -5;
        }
        int size = 0;
        if((strcmp(fmt,"%s")==0))
            size = 0;
        else if((strcmp(fmt,"%c")==0))
            size = 1;
        else if(strstr(fmt,"d")||strstr(fmt,"i")||strstr(fmt,"u")||strstr(fmt,"X")||strstr(fmt,"x")) {
            if(strstr(fmt,"hh"))
                size = 1;
            else if(strstr(fmt,"h"))
                size = 2;
            else if(strstr(fmt,"ll"))
                size = 8;
            else
                size = 4;
        } else {
            //...
        }
        char *p = sres;
        const void *v = value;
        for(int i=0; i<count; i++) {
            if(size==0)
                p += sprintf(p,fmt,v);
            else if(size==1)
                p += sprintf(p,fmt,*(uint8_t*)v);
            else if(size==2)
                p += sprintf(p,fmt,*(uint16_t*)v);
            else if(size==4)
                p += sprintf(p,fmt,*(uint32_t*)v);
            else if(size==4)
                p += sprintf(p,fmt,*(uint64_t*)v);
            else
                return -3;
            if(i<count-1)
                p += sprintf(p,split_chars);

            v = (char*)v + size;
        }
        return 0;
    } else {
        return -9;
    }
}

//==========================================================
// uint8_t ARRAY <==>  uint16_t(2Bytes) uint32_t(4Bytes) QWORD(8Bytes)
// MSB = Big Endian, LSB = Littie Endian

uint16_t HX_LSB_B2W(const void *data)
{
	const uint8_t *d = data;
    uint16_t res;
    res = (uint16_t)(d[1]);
    res <<= 8;
    res += (uint16_t)(d[0]);
    return res;
}
uint16_t HX_MSB_B2W(const void *data)
{
	const uint8_t *d = data;
    uint16_t res;
    res = (uint16_t)(d[0]);
    res <<= 8;
    res += (uint16_t)(d[1]);
    return res;
}
uint64_t HX_MSB_B2QW(const void *data)
{
    const uint8_t *d = data;
    uint64_t res;
	res = (uint64_t)d[0];
    res <<=8;
    res += (uint64_t) d[1];
    res <<=8;
    res += (uint64_t) d[2];
    res <<=8;
    res += (uint64_t) d[3];
	res <<=8;
    res += (uint64_t) d[4];
    res <<=8;
    res += (uint64_t) d[5];
    res <<=8;
    res += (uint64_t) d[6];
    res <<=8;
    res += (uint64_t) d[7];
    return res;
}
uint32_t HX_MSB_B2DW(const void *data)
{
	const uint8_t *d = data;
    uint32_t res;
    res = (uint32_t)d[0];
    res <<=8;
    res += (uint32_t) d[1];
    res <<=8;
    res += (uint32_t) d[2];
    res <<=8;
    res += (uint32_t) d[3];
    return res;
}
uint64_t HX_LSB_B2QW(const void *data)
{
	const uint8_t *d = data;
    uint64_t res;
	res = (uint64_t)d[7];
    res <<=8;
    res += (uint64_t) d[6];
    res <<=8;
    res += (uint64_t) d[5];
    res <<=8;
    res += (uint64_t) d[4];
	res <<=8;
    res += (uint64_t) d[3];
    res <<=8;
    res += (uint64_t) d[2];
    res <<=8;
    res += (uint64_t) d[1];
    res <<=8;
    res += (uint64_t) d[0];
    return res;
}
uint32_t HX_LSB_B2DW(const void *data)
{
	const uint8_t *d = data;
    uint32_t res;
    res = (uint32_t)d[3];
    res <<=8;
    res += (uint32_t) d[2];
    res <<=8;
    res += (uint32_t) d[1];
    res <<=8;
    res += (uint32_t) d[0];
    return res;
}
uint8_t * HX_LSB_W2B(uint16_t v,void *p)
{
	uint8_t *_p = p;
    _p[0] =  0xFF & (v>>0);
    _p[1] =  0xFF & (v>>8);
	return _p;
}
uint8_t * HX_MSB_W2B(uint16_t v,void *p)
{
    uint8_t *_p = p;
    _p[0] =  0xFF & (v>>8);
    _p[1] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_MSB_DW2B(uint32_t v,void *p)
{
    uint8_t *_p = p;
    _p[0] =  0xFF & (v>>24);
    _p[1] =  0xFF & (v>>16);
    _p[2] =  0xFF & (v>>8);
    _p[3] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_MSB_QW2B(uint64_t v,void *p)
{
	uint8_t *_p = p;
    _p[0] =  0xFF & (v>>56);
    _p[1] =  0xFF & (v>>48);
    _p[2] =  0xFF & (v>>40);
    _p[3] =  0xFF & (v>>32);
    _p[4] =  0xFF & (v>>24);
    _p[5] =  0xFF & (v>>16);
    _p[6] =  0xFF & (v>>8);
    _p[7] =  0xFF & (v>>0);
	return _p;
}
uint8_t * HX_LSB_DW2B(uint32_t v,void *p)
{
    uint8_t *_p = p;
    _p[3] =  0xFF & (v>>24);
    _p[2] =  0xFF & (v>>16);
    _p[1] =  0xFF & (v>>8);
    _p[0] =  0xFF & (v>>0);
	return _p;
}
uint8_t *HX_LSB_QW2B(uint64_t v,void *p)
{
	uint8_t *_p = p;
    _p[7] =  0xFF & (v>>56);
    _p[6] =  0xFF & (v>>48);
    _p[5] =  0xFF & (v>>40);
    _p[4] =  0xFF & (v>>32);
    _p[3] =  0xFF & (v>>24);
    _p[2] =  0xFF & (v>>16);
    _p[1] =  0xFF & (v>>8);
    _p[0] =  0xFF & (v>>0);
	return _p;
}
void HX_W_REV(__packed uint16_t *v)
{
	uint8_t buf[2];
	*v = HX_LSB_B2W(HX_MSB_W2B(*v,buf));
}
void HX_DW_REV(__packed uint32_t *v)
{
	uint8_t buf[4];
	*v = HX_LSB_B2DW(HX_MSB_DW2B(*v,buf));
}
void HX_QW_REV(__packed uint64_t *v)
{
	uint8_t buf[8];
	*v = HX_LSB_B2QW(HX_MSB_QW2B(*v,buf));
}
uint64_t HX_W_REV2(uint16_t v)
{
	uint8_t buf[2];
	return HX_LSB_B2W(HX_MSB_W2B(v,buf));
}
uint64_t HX_DW_REV2(uint32_t v)
{
	uint8_t buf[4];
	return HX_LSB_B2DW(HX_MSB_DW2B(v,buf));
}
uint64_t HX_QW_REV2(uint64_t v)
{
	uint8_t buf[8];
	return HX_LSB_B2QW(HX_MSB_QW2B(v,buf));
}
unsigned char make_bcc(unsigned char init, const void *data, int len)
{
	unsigned char res = init;
	const unsigned char *p = (const unsigned char*)data;
	for(int i=0;i<len;i++){
		res ^= p[i];
	}
	return res;
}
unsigned char make_bcc2(const void *data, int len)
{
	return make_bcc(0,data,len);
}
unsigned char make_sum(const void *buf, int len)
{
	const unsigned char *p = buf;
	unsigned char sum = 0;
	for(int i=0;i<len;i++){
		sum += p[i];
	}
	return sum;
}
unsigned int make_sum2(unsigned int init,const char *buf, int len)
{
	unsigned int res = 0;
	for(int i=0;i<len;i++){
		res += (unsigned int) buf[i];
	}
	return res;
}

void* pk_fill(void *to, int len, int d)
{
    char *res = memset(to,d,len);
    return res + len;
}
void* pk_add(void *to, int len, const void *from)
{
    char *res = (char*)memcpy(to,from,len);
    return res + len;
}
void* pk_get(void*from,int len,void* to)
{
    char *res = (char*)from;
    memcpy(to,from,len);
    return  res + len;
}


//------------------------------------------------------------------------------
int ymd2days(int y,int m,int d)
{
	const int tbl[] = {
		31,
		31+28,
		31+28+31,
		31+28+31+30,
		31+28+31+30+31,
		31+28+31+30+31+30,
		31+28+31+30+31+30+31,
		31+28+31+30+31+30+31+31,
		31+28+31+30+31+30+31+31+30,
		31+28+31+30+31+30+31+31+30+31,
		31+28+31+30+31+30+31+31+30+31+30,
		31+28+31+30+31+30+31+31+30+31+30+31,
	};
	//if(y>1970)
		y-=1970;
	int f = y%4?0:1;
	return (y * (365+f)) + (tbl[m]+(m>2?f:0)) + d;

}
long long ymdhms2sec(int y,int m,int d,int H,int M,int S)
{
	int ds = ymd2days(y,m,d);
	return ds*3600*24+H*3600+M*60+S;
}

int ymdbcd2days(uint8_t *yyyymmdd)
{
	return ymd2days(
			bcd2int(yyyymmdd[0])*100+bcd2int(yyyymmdd[1]),
			bcd2int(yyyymmdd[2]),
			bcd2int(yyyymmdd[3])
	);
}
long long ymdhmsbcd2sec(uint8_t *yyyymmddHHMMSS)
{
	long long ds = ymdbcd2days(yyyymmddHHMMSS);
	return ds*3600*24+ymdhms2sec(0,0,0,
			bcd2int(yyyymmddHHMMSS[4]),
			bcd2int(yyyymmddHHMMSS[5]),
			bcd2int(yyyymmddHHMMSS[6])
	);
}
