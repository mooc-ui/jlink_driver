
#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero

int main(int argc,char **argv)
{

	printf("build: %s %s\n",__DATE__,__TIME__);
	printf("=======================================\n");

	//create local server sock
	struct sockaddr_in server_addr;
    	bzero(&server_addr,sizeof(server_addr)); 
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    	server_addr.sin_port = htons(4848);
	int server_socket = socket(PF_INET,SOCK_STREAM,0);
	if( server_socket < 0)
    	{
    	    printf("Create Local Socket Failed!");
    	    exit(1);
    	}
	{ 
	   int opt =1;
	   setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
	}
	//把socket和socket地址结构联系起来
	if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
	{
		printf("Local Server Bind Port Failed!"); 
		exit(1);
	}

	//server_socket用于监听
	if ( listen(server_socket, LENGTH_OF_LISTEN_QUEUE) )
	{
		printf("local Server Listen Failed!"); 
		exit(1);
	}
	
	
	return 0;
}
