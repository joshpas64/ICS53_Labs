#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
/*
Partner 1 Name: Joshua Pascascio
Partner 1 ID#: 52192782
Partner 2 Name: Tung Nguyen
Partner 2 ID#: 39291907

// Socket Server Base
// My Written Code havent tested this is just a layout im thinking of from reading the book
// and online resources. The same can be said for the client part I wrote
*/
struct email_db {
	char* user_name;
	char* email_address;
} email_db;

void init_server(struct email_db** startDB);
void free_server(struct email_db** startDB,int len);

void init_server(struct email_db** startDB){
	startDB[0]->user_name = malloc(strlen("Ian G. Harris"));
	strcpy(startDB[0]->user_name,"Ian G. Harris");
	startDB[0]->email_address = malloc(strlen("harris@ics.uci.edu"));
	strcpy(startDB[0]->email_address,"harris@ics.uci.edu");
	startDB[1]->user_name = malloc(strlen("Joe Smith"));
	strcpy(startDB[1]->user_name,"Joe Smith");
	startDB[1]->email_address = malloc(strlen("joe@cnn.com"));
	strcpy(startDB[1]->email_address,"joe@cnn.com");
	startDB[2]->user_name = malloc(strlen("Jane Smith"));
	strcpy(startDB[2]->user_name,"Jane Smith");
	startDB[2]->email_address = malloc(strlen("jane@slashdot.com"));
	strcpy(startDB[2]->email_address,"jane@slashdot.com");
}

void free_server(struct email_db** startDB,int len){
	int i;
	if(len < 3)
		len = 3;
	for(i = 0; i < len; i++){
		free(startDB[i]->user_name); /*// Might night need to explicitly free or deallocate the strings since they are turned into */
		free(startDB[i]->email_address); /* // strings at instantiation (might have built in destructors) */
		free(startDB[i]);
	}
}

void putintoDB(struct email_db** startDB,int* currentLen,char* user, char* email){ /* If no bult in email or db data structure might just change email DB into a linked list implementation */
	startDB = realloc(startDB,(*currentLen + 1) * sizeof(struct email_db*));
	(*(currentLen))++;
	(*(startDB + (*currentLen) - 1))->user_name = malloc(strlen(user));
	strcpy((*(startDB + (*currentLen) - 1))->user_name,user);
	(*(startDB + (*currentLen) - 1))->email_address = malloc(strlen(email));
	strcpy((*(startDB + (*currentLen) - 1))->email_address,email);
}

int parseCommand(struct email_db** startDB,int* currentLen,char* inputs);

int parseCommand(struct email_db** startDB,int* currentLen,char* inputs){
	int i;
	char* replaced;
	if((replaced = strchr(inputs,'\n')) != NULL)
        *replaced = '\0';
	if(strcmp(inputs,"+++") == 0)
		return -2;
	for(i = 0; i < (*currentLen); i++){
		if(strcmp(startDB[i]->email_address,inputs) == 0){
			return i;
		}
	}
	return -1;
}

int main(int argc, char* argv[]){
	struct email_db** startDB = malloc(3 * sizeof(struct email_db*));
	int i;
	for(i = 0; i < 3; i++){
        startDB[i] = malloc(sizeof(struct email_db));
	}
	init_server(startDB);
	int currentLen = 3;
	int port = atoi(argv[1]);
	struct hostent* client_info;
	int listenfd;
	int connfd = 0;
	int optval = 1;
	struct sockaddr_in serv_addr, clientaddr; /*//Internet socket info */
	int clientLength;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd < 0){
		printf("Could not create initial listening socket");
		return -1;
	}
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void*) &optval, sizeof(int)) < 0)
	{
		printf("Could not set socket option to reuse ip address of server");
		return -1;
	}
	/*// get size of the server socket, in bytes so make it a (char *) and formatted in network byte
	/// order with bzero function */
	bzero((char *)&serv_addr, sizeof(serv_addr));
	/*// Have the networking information for the socket be stored in serv_addr (contains information about a socket)
	/// so it can be bound as a listening socket later in the program */
	serv_addr.sin_family = AF_INET; /*// Set it as an internet socket */
	serv_addr.sin_addr.s_addr = INADDR_ANY; /*Set one of its IP addresses */
	/*// could also use localhost maybe ?????
	///serv_addr.sin_addr.s_addr = htonl(gethostbyname("localhost")->h_addr_list[0]);
	/// serv_addr.sin_addr.s_addr = htonl("127.0.0.1"); */
	serv_addr.sin_port = htons((unsigned short) port);
	/*//bind the listening socket fd to the information in the sockaddr_in struct ser_addr */
	if((bind(listenfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)){
		printf("Could not successfully bind the socket with the specified information\nIP,domain name, or port may be invalid\n");
		printf("Cannot bind socket: [%d] [%s]\n",errno,strerror(errno));
		return -1;
	}
	/*//Now the socket is now successfully bound to the right IP and port
	/// Make it a listening socket */
	if(listen(listenfd,5) < 0){
		printf("Error setting the socket as a listening socket\n");
		return -1;
	}
	char buffer[256];
	char clientBuffer;
	char firstByte;
	/*// Socket is now up and ready and listening for connections */
	int result;
	while(1){
        printf("Address server has started\n");
        clientLength = sizeof(clientaddr);
		/*
		//Accept a connection request and store its IP and TCP info in clientaddr
		// The connections socket itself will be connfd and represent an API between
		// the client and the listening socket for which they can exchange information
		// in bytes through TCP, and HTTP connections and requests
		*/
		connfd = accept(listenfd,(struct sockaddr *) &clientaddr, &clientLength);
		client_info = gethostbyaddr((struct sockaddr *)&clientaddr.sin_addr.s_addr,
			sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        /*
		// Perform operations and exchanges use buffer as input and output buffer
		// Also note keep TCP connection alive during exchange until termination
		// input +++ is recvd
		// Once +++ command received close connection
		// use rio_read rio_write
		*/
		int n;
		do{
		n = read(connfd,&firstByte,1);
		if(n == 0 || n == -1)
            break;
		bzero(buffer,256);
		read(connfd,buffer,firstByte);
        printf("%s",buffer);
		int result = parseCommand(startDB,&currentLen,buffer);
		bzero(buffer,256);
		if(result >= 0){
		    strcpy(buffer,startDB[result]->user_name);
		}
		else if(result == -1){
            strcpy(buffer,"unknown");
		}
		else if(result == -2)
            break;
		clientBuffer = (char) strlen(buffer);
		write(connfd,&clientBuffer,1);
		write(connfd,buffer,clientBuffer);
		bzero(buffer,256);
		}
		while(result != -2);
		close(connfd);
	}
	free_server(startDB,3); /*/ initial at least 3 */
	free(startDB);
	return 0;
}

