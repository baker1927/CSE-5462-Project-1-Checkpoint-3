#include"header.h"

/* Prototypes in header ^^^ */

int AddToBuffer(char *p, struct node *temp)
{
	
	int ready = 0;
	
	
	/* Node to get info on current buffer slot of seq */
	//int i = 0;
	while (ready == 0) {
		struct node *ptr;
		ptr = (struct node *)malloc(sizeof(struct node));
		ptr->next = NULL;
		ptr = findNode(temp, end);
			
		if (ptr != NULL) {
			//printf("Checking Seq. Slot: %d Acked: %d Seq: %d\n", ptr->start, ptr->ack, ptr->seq);
			if (ptr->ack == 1) {
				
				//buffer slot is ready
				ready = 1;
			} else {
				// increment buffer slot
				end = (end + MSS) % CBUFFERSIZE;
			}
		}
		else {
			// buffer slot ready
			ready = 1;
		}
		//i = i+1;
		//printf("i: %d\n", i);
		
		//printList(temp);
	}
	
	
	int temp2 = end;
	
	//cBuffer[end] = p;
	
    cBuffer[end] = malloc(MSS);
	
	bcopy(p, cBuffer[end], MSS);

	//strcpy(cBuffer[end], p);
    end = (end + MSS) % CBUFFERSIZE;


    if (active <= CBUFFERSIZE)
    {
       active = active + MSS;
    } else {
        start = (start + MSS) % CBUFFERSIZE;
    }
	
	
	return temp2;
}

int AddToBufferForServer(char *p)
{
	
	
	int temp2 = end;
	
	//cBuffer[end] = p;
	
    cBuffer[end] = malloc(MSS);
	
	bcopy(p, cBuffer[end], MSS);

	//strcpy(cBuffer[end], p);
    end = (end + MSS) % CBUFFERSIZE;


    if (active <= CBUFFERSIZE)
    {
       active = active + MSS;
    } else {
        start = (start + MSS) % CBUFFERSIZE;
    }
	
	
	return temp2;
}

char * GetFromBuffer()
{
    char *p;

    if (!active) { 
	return NULL; 
    }

    p = cBuffer[start];
    start = (start + MSS) % CBUFFERSIZE;

    active = active - MSS;
    return p;
}

char * GetFromBufferByIndex(int index)
{
	char *p;
	printf("In Get Buffer\n");
	fflush(stdout);
	/*if (!active) {
		return NULL;
	}*/
	
	p = cBuffer[index];
	//printf("Copied data from buffer slot: %d\n", index);
	/*
	start = (start + MSS) % CBUFFERSIZE;

	active = active - MSS;
	*/
	return p;
}

int cBufferFull() {
	if (start == 63000) {
		return 1;
	}
	else {
		return 0;
	}
}

int cBufferReady(struct node *temp) {
	int i = 0;
	int acked = 0;
	int notAcked = 0;
	for (i = 0; i < CBUFFERSIZE; i = (i + MSS)) {
		struct node *ptr;
		ptr = (struct node *)malloc(sizeof(struct node));
		ptr->next = NULL;
		ptr = findNode(temp, i);
		
		if (ptr->ack == 0) {
			return 0;
		}
	}
	return 1;
}

int getStart() {
	return start;
}

int getEnd() {
	return end;
}

void displayBuffer() { /* Function to display status of Circular Queue */
	int i = 0;
	printf("\nSTATUS OF BUFFER\nFront[%d]->", start);
	for (i = 0; i < CBUFFERSIZE; i = (i + MSS)) {
		printf("Index: %d, Contents: %s\n", i, cBuffer[i]);
	}
	printf("<-[%d]Rear", end);
	
}




/* For local testing of buffer inside this file */

/*int main(int argc, char *argv[]) {


	char test1[MSS] = "TESTING1";
	char test2[MSS] = "TESTING2";
	char test3[MSS] = "TESTING3";
	char block[MSS] = {0};
	char result[MSS] = {0};
	int k = 0;
	for(k=0; k<64; k++) {
		
		AddToBuffer((char *)&test1);
		AddToBuffer((char *)&test2);
		AddToBuffer((char *)&test3);
	}
	//display();
	/*int q = 63000;
	AddToBuffer((char *)&test1);
	printf("\nIndex: %d Contents: %s\n", q, cBuffer[q]);
	AddToBuffer((char *)&test2);
	q = 15000;
	printf("\nIndex: %d Contents: %s\n", q, cBuffer[q]);
	display();

	//bcopy(RetrieveFromBuffer(), (char *)&result, sizeof(result));

	display();
	for(k=0; k<63; k++) {
		bcopy(RetrieveFromBuffer(), (char *)&result, sizeof(result));
		//printf("%s\n", result);
	}
	display();

	return 0;




}*/

