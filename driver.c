/*
/ Eric Olson and James Baker
/ CSE 5462 Project 1, Checkpoint 1
/ September 29, 2016
/ This file is meant to interact with the timer-process
*/

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#define TIMER_PORT 9090
#define DRIVER_PORT 8908

/* These are the prototypes for the functions described in this file */
void starttimer(double timeout, int seq_num, int sock, int ret_port, struct sockaddr_in server_addr);
void canceltimer(int seq_num, int sock, struct sockaddr_in server_addr);


/*This is the definition of a message that is passed from the driver to the timer */
typedef struct message {
	int type;
	int p_num;
	double time;
	int ret_port;
} message_t;


int main(int argc, char* argv[]) {
	printf("%s\n", "The driver is beginning...");

	int sock;
	struct sockaddr_in sin_addr;

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    	perror("Error opening socket");
    	exit(1);
    } 

    struct sockaddr_in server_addr; 

 //    struct hostent *hp;
 //    hp = gethostbyname(argv[1]);
 //    if(hp == 0) {
	// fprintf(stderr, "%s:unknown host\n", argv[1]);
	// exit(3);
 //    }
 //    bcopy((char *)hp->h_addr, (char *)&server_addr.sin_addr, hp->h_length);
	
	sock = socket(AF_INET, SOCK_DGRAM, 0); 

	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(TIMER_PORT); // short, network byte order 
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); 
	memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct 

	struct sockaddr_in driver_addr; 
	driver_addr.sin_family = AF_INET; 
	driver_addr.sin_port = htons(DRIVER_PORT); // short, network byte order 
	driver_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	memset(&(server_addr.sin_zero), '\0', 8); // zero the rest of the struct 
	if(bind(sock, (struct sockaddr *)&driver_addr, sizeof(struct sockaddr_in)) < 0) {
      	perror("Error binding stream socket");
      	exit(1);
    }

	// canceltimer(2, sock, server_addr);
	// canceltimer(2, sock, server_addr);
	// starttimer(7.0,1, sock, server_addr);

	typedef struct send {
    	int flag;
    	int seq_num;
    } send_msg_t;

	starttimer(2.1234, 1, sock, DRIVER_PORT, server_addr);

	send_msg_t buffer;
	bzero((char*)&buffer, sizeof(buffer));

	recvfrom(sock, (char*)&buffer, sizeof buffer, MSG_WAITALL, NULL, NULL);
	printf("After the response. Sequence number: %d\n", buffer.seq_num);

	// starttimer(20.0,1, sock, server_addr);
	// printf("%s\n", "Sent One");

	// starttimer(10.0,2, sock, server_addr);
	// printf("%s\n", "Sent Two");

	// starttimer(30.0,3, sock, server_addr);
	// printf("%s\n", "Sent Three");

	// sleep(5);
	// printf("%s\n", "Just slept");

	// canceltimer(2, sock, server_addr);
	// printf("%s\n", "canceled Two");

	// starttimer(20.0,4, sock, server_addr);
	// printf("%s\n", "Sent Four");

	// sleep(5);
	// printf("%s\n", "Just slept");

	// starttimer(18.0,5, sock, server_addr);
	// printf("%s\n", "Sent Five");

	// canceltimer(4, sock, server_addr);
	// printf("%s\n", "Canceled Four");

	// canceltimer(8, sock, server_addr);
	// printf("%s\n", "Canceled 8");

	close(sock);

	printf("%s\n", "The driver is ending...");
}


/*This function creates a start timer message and sends it to the timer process */
void starttimer(double timeout, int seq_num, int sock, int ret_port, struct sockaddr_in server_addr) {
	message_t send_msg;
	bzero((char*)&send_msg, sizeof(send_msg));
	send_msg.type = 0;
	send_msg.p_num = seq_num;
	send_msg.time = timeout;
	send_msg.ret_port = ret_port;

	if(sendto(sock, &send_msg, sizeof(send_msg), 0,
		(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {

		perror("There was an error sending to the socket in the driver "
			"(starttimer)");
		exit(1);
	}
}


/* This function creates a cancel timer message and sends it to the timer process */
void canceltimer(int seq_num, int sock, struct sockaddr_in server_addr) {
	message_t send_msg;
	bzero((char*)&send_msg, sizeof(send_msg));
	send_msg.type = 1;
	send_msg.p_num = seq_num;
	send_msg.time = 0;

	if(sendto(sock, &send_msg, sizeof(send_msg), 0, 
		(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {

		perror("There was an error sending to the socket in the driver " 
			"(canceltimer)");
		exit(1);
	}
}