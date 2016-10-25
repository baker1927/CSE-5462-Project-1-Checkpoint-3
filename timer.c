/*
/ Eric Olson and James Baker
/ CSE 5462 Project1, Checkpoint 1
/ September 29, 2016
/ This file contains the delta timer description.
*/


#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/select.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define WAIT_TIME 1
#define LISTENING_PORT 9090



/* This is the definition for each node within our delta timer linked list */
typedef struct timer_node {
	float time;
	//int port;
	int p_num;
	struct timer_node* next;
	struct timer_node* prev;
} timer_node_t;



/* These are the prototypes for the functions defined within this file */
void delete_node(int packet);
void add_node(float seconds, int packet);
int insert_node(timer_node_t** head, timer_node_t* node);
int remove_node(int del_val, timer_node_t** head);
void print_full_list(timer_node_t* head);
timer_node_t* create_node(float time, int port/*, int p_num*/);
void destroy_node(timer_node_t* node);



/* This pointer points to the head of the delta list */
timer_node_t* head = NULL;



/* This method deallocates the memory held by a node */
void destroy_node(timer_node_t* node) {
	free(node);
}



/* This method creates a new node for our linked list with the supplied criteria.
   It returns a pointer to the newly created node */
timer_node_t* create_node(float time, int p_num/*, int port*/) {
	timer_node_t* node = (timer_node_t*)malloc(sizeof(timer_node_t));
	if (NULL == node) {
		printf("%s\n", "ERROR: Failed to allocate memory for a new timer node");
	} else {
		node->next = NULL;
		node->prev = NULL;
		node->time = time;
		node->p_num = p_num;
		//node->port = port;
	}
	return node;
}



/* This method prints out the linked list in its entirety */
void print_full_list(timer_node_t* list) {
	if (NULL == list) {
		printf("%s\n", "The delta timer list is empty");
	} else {
		while(NULL != list) {
			printf("%s%.2f%s%d%s\n", "NODE:: time: ", list->time, " p_num: ", 
				list->p_num," ---> ");
			list = list->next;
		}
	}
}



/* This function inserts the supplied node into the linked list. It returns 1
   for a successful insertion and 0 otherwise */
int insert_node(timer_node_t** head, timer_node_t* node) {
	//If the list is empty
	if (NULL == *head) {
		*head = node;
		return 1;
	} 

	timer_node_t* curr_node = *head;
	timer_node_t* prev_node = NULL;

	while (NULL != curr_node && node->time > curr_node->time) {
		node->time -= curr_node->time;
		prev_node = curr_node;
		curr_node = curr_node->next;
	}
	//Insert at the end of the list
	if (NULL == curr_node) {
		prev_node->next = node;
		node->prev = prev_node;
		return 1;
	} 
	//Insert as a new head node
	if (NULL == prev_node) {
		curr_node->prev = node;
		node->next = curr_node;
		*head = node;
		if (NULL != node->next) {
			node->next->time -= node->time;
		}
		return 1;
	} 
	//Insert between two nodes
	prev_node->next = node;
	curr_node->prev = node;
	node->prev = prev_node;
	node->next = curr_node;
	if (NULL != node->next) {
		node->next->time -= node->time;
	}
	return 1;
}



/* This function removes a node with a particular packet number from the list.
   It returns 1 for a successful removal and 0 otherwise */
int remove_node(int del_val, timer_node_t** head) {
	//The list is empty
	if (NULL == *head) {
		printf("%s\n", "ERROR: The list is empty. Cannot delete specified node.");
		return 0;
	}
	timer_node_t* trash = NULL;
	//The node to be removed is the head node
	if ((*head)->p_num == del_val) {
		trash = *head;
		*head = (*head)->next;
		//Just in case we have removed the only node in the list
		if (NULL != *head) {
			(*head)->prev = NULL;
		}
		if (NULL != (*head)) {
			(*head)->time += trash->time;
		}
		destroy_node(trash);
		return 1;
	}

	timer_node_t* tracker = *head;
	while (NULL != tracker && tracker->p_num != del_val) {
		tracker = tracker->next;
	}
	//The node wasn't found
	if (NULL == tracker) {
		printf("%s%d%s\n", "ERROR: The node with packet number: ", del_val,  
			" was never found. Failed to delete.");
		return 0;
	}

	trash = tracker;
	(tracker->prev)->next = tracker->next;
	if (NULL != tracker->next) {
		(tracker->next)->prev = tracker->prev;
	}
	if (NULL != tracker->next) {
		tracker->next->time += tracker->time;
	}
	destroy_node(trash);
	return 1;
}



/* This function is meant as a wrapper around inser_node(). It contains 
   additional print statements for work flow traking */
void add_node(float seconds, int packet) {
	timer_node_t* node = create_node(seconds, packet);
	int check = insert_node(&head, node);

	if (check == 0) {
		printf("%s%.2f%s%d\n", "There was an issue adding the node with " 
			"seconds: ", seconds, " and packet number: ", packet);
	} else {
		printf("%s%.2f%s%d%s\n", "Node with seconds: ", seconds, " and packet "
			"number: ", packet, " added successfully");
	}

	printf("%s\n\n", "State of the delta timer:");
	print_full_list(head);
	printf("\n\n");
}



/* This function is meant as a wrapper around remove_node(). It contains
   additional print statements for work flow tracking */
void delete_node(int packet) {
	int check = remove_node(packet, &head);

	if (check == 0) {
		printf("%s%d%s\n", "There was an issue deleting the node with " 
			"packet number: ", packet, ". See above message.");
	} else {
		printf("%s%d%s\n", "Node with packet number: ", packet, " deleted "
			"successfully");	
	}

	printf("%s\n\n", "State of the delta timer:");
	print_full_list(head);
	printf("\n\n");

}

int main(int argc, char *argv[]) {
	printf("Timer process booting up...\n\n");

	/* Create a socket for the timer to listen on */
	int sock;
	struct sockaddr_in sin_addr;

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    	perror("Error opening socket");
    	exit(1);
    }

    sin_addr.sin_family = AF_INET;
    sin_addr.sin_port = htons(LISTENING_PORT);
    sin_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&(sin_addr.sin_zero), '\0', 8);

    if(bind(sock, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) {
      	perror("Error binding stream socket");
      	exit(1);
    }

    //The definition of the message received by the driver
    typedef struct recvd {
    	int type;
    	int p_num;
    	float time;
    } recv_msg_t;

    recv_msg_t recv_msg;
    bzero((char*)&recv_msg, sizeof(recv_msg));

     fd_set read_set;

    /* Create the variables that will be used for time keeping */
    time_t start_time, end_time, global_start_time;
	struct timeval wait_timer;
	wait_timer.tv_sec = WAIT_TIME;
	wait_timer.tv_usec = 0;

	time(&global_start_time);
	for(;;) {
		FD_ZERO(&read_set);
    	FD_SET(sock, &read_set);
		time(&start_time);
		if (select(FD_SETSIZE, &read_set, NULL, NULL, &wait_timer) < 0) {
			fprintf(stderr, "%s\n", "There was an issue with select()");
			exit(1);
		}

		time(&end_time);
		printf("%s%f\n", "------------------CURRENT TIME----------------", 
			difftime(end_time, global_start_time));

		if(NULL != head) {
			head->time -= difftime(end_time, start_time);
			printf("%s%.2f%s%.2f\n", "The head's time ", head->time, " Elapsed time: ", difftime(end_time, start_time));
			if (head->time <= 0) {
				printf("%s%d%s\n", "Packet number: ", head->p_num, " has timed out.");
				remove_node(head->p_num, &head);
				printf("%s\n", "Timed out node removed.");
				printf("%s\n", "Status of the delta timer list:");
				print_full_list(head);
				printf("\n\n");
			}
		}
			
		if(FD_ISSET(sock, &read_set)) {
			if(recvfrom(sock, (char*)&recv_msg, sizeof(recv_msg), 0, NULL, NULL) < 0) {
				perror("There was an error receiving from the driver");
				exit(1);
			}
			if (recv_msg.type == 0) {
				printf("%s%.2f%s%d\n", "Received add request ", recv_msg.time, ", ", recv_msg.p_num);
				add_node(recv_msg.time, recv_msg.p_num);
			} else if (recv_msg.type == 1) {
				printf("%s%d\n", "Received delete request ", recv_msg.p_num);
				delete_node(recv_msg.p_num);
			}
			bzero((char*)&recv_msg, sizeof(recv_msg));
			printf("%s\n", "Just serviced message");
		} 

	}//end for
	close(sock);
	return 0;
}