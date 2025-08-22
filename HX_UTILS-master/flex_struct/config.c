/*
   Config File Format:
   struct <struct_name> // <table_name>
   {
      <type_name> <val_name>;	// <var_title> , <format_dec>
   }

   <struct_name> : struct_name for self defined
   <type_name>   : type_name surpport C compiler types, also can use
                   #define or typedef to define a alias.
   <val_name>    : self defined
   
 Belows item are use for disp after convert.
   <table_name>  : self defined table title.
   <var_title>   : self defined variable title. 
   <format_dec>  : describ the variable is what format ,like data or bcd etc.

 Sampale:
 #define u8 unsigned char
 typedef u8 byte;
 struct Record_St //记录表
 {
   u8 data [ 7 ] ;	//时间,yyyymmddHHMMSS
   u8 data[4];		//金额,msb_u32
 }
*/

#define u8 unsigned char
typedef u8 byte;
struct Record_St //记录表
{
   u8 data [ 7 ] ;	//时间,yyyymmddHHMMSS
   u8 data[4];		//金额,msb_u32
}
