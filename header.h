#ifndef HEADER_H_  /* Include guard */
#define HEADER_H_

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
//#include <sys/time.h>
#include <time.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>

/* Constants */
#define MSS 1000
#define LOCALPORT 9999
#define LOCALADDRESS "127.0.0.1"
#define ETA "164.107.113.23"
#define BETA "164.107.113.18"
#define CLIENTTROLLPORT 10001
#define SERVERTROLLPORT 10003
#define TCPDSERVERPORT 10002
#define CBUFFERSIZE 64000 /* 64 KB buffer per spec */
#define ALPHA 0.875
#define RHO 0.25

/* Structs used */

/* tcpd to tcpd packet */
/*typedef struct Packet {
	char body[MSS];
	int bytes_to_read;
	int chksum;
	int packNo;
	int startNo;
	//add additional vars here
} Packet;*/

//TCP Header structure as per RFC 793
typedef struct Packet {
 u_short th_sport;  /* source port */
 u_short th_dport;  /* destination port */
 int seq;   /* sequence number */
 int ack;   /* acknowledgement number */
#if BYTE_ORDER == LITTLE_ENDIAN
 u_int th_x2:4,  /* (unused) */
  th_off:4;  /* data offset */
#endif
#if BYTE_ORDER == BIG_ENDIAN
 u_int th_off:4,  /* data offset */
  th_x2:4;  /* (unused) */
#endif
 u_char th_flags;
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
#define TH_ECE 0x40
#define TH_CWR 0x80
#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
 
 u_short th_win;   /* window */
 u_short th_sum;   /* checksum */
 u_short th_urp;   /* urgent pointer */
 //data
 char body[MSS];
 int bytes_to_read;
 int chksum;
 int packNo;
 int startNo;
} Packet;

/* tcpd to troll packet */
typedef struct MyMessage {
	struct sockaddr_in msg_header;
	struct Packet msg_pack;
	//int flag;	
} MyMessage;

/* aux List node */
struct node
{
	int start; /* Consider this the node start. Corresponds with MSS chunk in cBuffer 0-64000 */		
	int nextB; /* Next chunk index, optional */
	int pack;  /* Packet No. */
	int bytes; /* Num of bytes read in from client */
	int seq;   /* Sequence number */
	int ack;   /* acknowledgement flag (0 = no, 1 = yes) */
	struct timespec time;/* Time */
	
	struct node *next;
};

/* aux list prototypes */
void insertNode(struct node *ptr, int start, int nextB, int pack, int bytes, int seq, struct timespec time);
void deleteNode(struct node *ptr, int start);
void printList(struct node *ptr);
struct node *findNode(struct node *ptr, int start);
double calculate_rto(double sample_rtt);

/* circular buffer properties */
static char *cBuffer[CBUFFERSIZE];
static int start = 0;
static int end = 0;
static int active = 0;

char * GetFromBuffer();
void AddToBuffer(char *p);
int getStart();
int getEnd();

#endif

#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif
