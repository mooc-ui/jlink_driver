  
%option yylineno
  
%{  
  
//# include"stdio.h"  

#define c_typedef()						printf("CTYPEDEF\t%u\t%s\n",yylineno,yytext);
#define cpp_define()					printf("CPPDEF\t%u\t%s\n",yylineno,yytext);
#define c_string()						printf("STRING\t%u\t%s\n",yylineno,yytext);
#define c_comma()							printf("COMMA\t%u\t%s\n",yylineno,yytext);
#define end_line()						printf("ENDL\t%u\t\n",yylineno);
#define c_line_anno()					printf("ANNO\t%u\t%s\n",yylineno,yytext);
#define c_blk_anno()					printf("ANNO_BLK\t%u\t\n",yylineno);
#define end_segment()					printf("END_SEG\t%u\t%s\n",yylineno,yytext);
#define c_b_right()						printf("BR\t%u\t%s\n",yylineno,yytext);		
#define c_b_left()						printf("BL\t%u\t%s\n",yylineno,yytext);		
#define c_m_right()						printf("MR\t%u\t%s\n",yylineno,yytext);		
#define c_m_left()						printf("ML\t%u\t%s\n",yylineno,yytext);		
#define c_l_right()						printf("LR\t%u\t%s\n",yylineno,yytext);		
#define c_l_left()						printf("LL\t%u\t%s\n",yylineno,yytext);		
#define c_struct()						printf("STRUCT\t%u\t%s\n",yylineno,yytext);		
#define c_type()							printf("TYPE\t%u\t%s\n",yylineno,yytext);			
#define c_int()								printf("INT\t%u\t%s\n",yylineno,yytext);			
#define c_word()							printf("WORD\t%u\t%s\n",yylineno,yytext);			
#define error_key_word()			printf("ERROR\t%u\tC keyword %s\n",yylineno,yytext);		
#define error_token()					printf("ERROR\t%u\tC token %s\n",yylineno,yytext);			


  
%}
  

%option noyywrap



%%

(\/\/)([^\n]*)														c_line_anno()
\/\*([^\*]|(\*)*[^\*\/])*(\*)*\*\/					c_blk_anno()

(\n)	end_line()

(\#define)([ \t]+)([^\n]*)								cpp_define()
(typedef)			c_typedef()

(struct)			c_struct()		

(auto)			c_type()									
(char)			c_type()			
(const)			c_type()			
(double)		c_type()				
(float)			c_type()			
(int)				c_type()			
(long)			c_type()			
(short)			c_type()			
(signed)			c_type()				
(unsigned)			c_type()			
(void)					c_type()			
(volatile)			c_type()			

	
(enum)			error_key_word()
(break)			error_key_word()
(case)			error_key_word()
(continue)	error_key_word()
(default)		error_key_word()
(do)				error_key_word()
(else)			error_key_word()
(extern)		error_key_word()
(for)				error_key_word()
(goto)			error_key_word()
(if)				error_key_word()
(inline)		error_key_word()
(register)	error_key_word()
(return)		error_key_word()
(sizeof)		error_key_word()
(static)		error_key_word()
(switch)		error_key_word()
(union)			error_key_word()
(while)			error_key_word()

([a-z]|[A-Z]|_)([a-z]|[A-Z]|[0-9]|_)*		c_word()

(\+|\-)?([1-9][0-9]*|0)								c_int()


[ \r\t]+	
										
(\+|\-)?([1-9][0-9]*|0)(\.[0-9]+)			error_token()
(')([a-z]|[A-Z])(')										error_token()
(")([\s\S]*)(")												error_token()


(\()			c_l_left()
(\))			c_l_right()
(\[)			c_m_left()
(\])			c_m_right()
(\{)			c_b_left()
(\})			c_b_right()
(\-\>)			error_token()
(\.)			error_token()
(!)			error_token()
(~)			error_token()
(\+\+)			error_token()
(\-\-)			error_token()
(\-)			error_token()
(\*)			
(&)				error_token()
(\/)			
(%)			error_token()
(\+)			error_token()
(\<\<)			error_token()
(\>\>)			error_token()
(\<)			error_token()
(\<=)			error_token()
(\>)			error_token()
(\>=)			error_token()
(==)			error_token()
(!=)			error_token()
(\^)			error_token()
(\|)			error_token()
(&&)			error_token()
(\|\|)			error_token()
(\?:)			error_token()
(=)				error_token()
(\+=)			error_token()
(\-=)			error_token()
(\*=)			error_token()
(\/=)			error_token()
(%=)			error_token()
(\>\>=)			error_token()
(\<\<=)			error_token()
(&=)			error_token()
(\^=)			error_token()
(\|=)			error_token()
(,)				c_comma()
(\\a)			error_token()
(\\b)			error_token()
(\\f)		error_token()
(\\n)		error_token()
(\\r)		error_token()
(\\t)		error_token()
(\\v)		error_token()
(;)		end_segment()
(:)		error_token()




%%





int main(int argc, char *argv[])  
{  
	FILE *fp;
	if(argc>1){
		fp = fopen(argv[1], "r");
		yyin = fp;
		yylex(); 
		fclose(fp);
		return 0;
	}else{
	  //printf("> ");  
	  //yyparse();  
	  //return 0;
	  return -1;
	}
}  
  