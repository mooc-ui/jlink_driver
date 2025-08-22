
#include "stdio.h"


int main(void)
{
	printf("this is dcc \n");
	
	fprintf(stdin,"this is stdin \n");
	fprintf(stdout,"this is stdout \n");
	fprintf(stderr,"this is stderr \n");
	
	fprintf((void*)'r',"this is red \n");
	fprintf((void*)'g',"this is green \n");
	fprintf((void*)'b',"this is blue \n");
	
	fprintf((void*)'R',"this is inverse red \n");
	fprintf((void*)'G',"this is inverse green \n");
	fprintf((void*)'B',"this is inverse blue \n");
	
	printf("«Î ‰»Î:\n");
	
	while(1){
		int c = getchar();
		if(c!=EOF){
			printf("getc: %c\n",(char)c);
		}
	}
}

