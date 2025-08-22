/* Companionsource code for "flex & bison", published by O'Reilly 
 * Media, ISBN 978-0-596-15597-1 
 * Copyright (c) 2009, Taughannock Networks.All rights reserved. 
 * See the README file for license conditionsand contact info. 
 * $Header: /home/johnl/flnb/code/RCS/fb1-5.l,v2.1 2009/11/08 02:53:18 johnl Exp $ 
 */  
  
/* recognizetokens for the calculator and print them out */  
  
%{  
  
# include"a.tab.h"  
  
%}  

%option noyywrap
  
%%  

"+"  { return ADD; }  
"-"   { return SUB; }  
"*"  { return MUL; }  
"/"   { return DIV; }  
"|"     { return ABS; }  
"("     { return OP; }  
")"     { return CP; }  
  
[0-9]+    { yylval = atoi(yytext); return NUMBER; }  
  
\n      { return EOL; }  
"//".*   
[ \t]   { /* ignore white space */ }  
.      { yyerror("Mystery character%c\n", *yytext); }  
  
%%