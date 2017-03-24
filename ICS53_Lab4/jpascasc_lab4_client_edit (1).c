#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>

/*
Lab 4 Client
Partner 1 Name: Joshua Pascascio
Partner 1 ID#: 52192782
Partner 2 Name: Tung Nguyen
Partner 2 ID#: 39291907
*/

int main(int argc, char* argv[]){
	int sockfd, port;
	port = atoi(argv[2]);
	int n = 0;
	char recvd[256];
	char toSend[256];
	char* host = argv[1];
	struct hostent* hp;
	struct sockaddr_in serv_addr;
	if(argc != 3){
		printf("\nUsage: %s <ip of server> \n",argv[0]);
		return 1;
	}
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
		printf("\n Error: Could not create socket \n");
		return 1;
	}
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    if((hp = gethostbyname(host)) == NULL){
        printf("\n DNS error occurred \n");
        return -2;
    }
     bcopy((char *)hp->h_addr_list[0],(char *)&serv_addr.sin_addr.s_addr, hp->h_length); 
	/* serv_addr.sin_addr.s_addr = inet_addr(host); */
	serv_addr.sin_port = htons((unsigned short) port);
	if(connect(sockfd,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
		printf("\n Error: Connect Failed \n");
		return 1;
	}
	int len;
	char firstByte;
	char recvByte;
	printf(">  ");
	while(fgets(toSend,256,stdin) != NULL){
        firstByte = (char) strlen(toSend);
        write(sockfd,&firstByte,1);
        write(sockfd,toSend,firstByte);
        if(strcmp("+++\n",toSend) == 0)
            break;
        bzero(toSend,256);
        bzero(recvd,256);
        read(sockfd,&recvByte,sizeof(recvByte));
        read(sockfd,recvd,recvByte);
        printf("%s\n",recvd);
        printf(">  ");
	}
	if(n < 0)
		printf("\n Read error \n");
	exit(0);
}
