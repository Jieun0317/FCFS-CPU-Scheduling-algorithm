#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define QUOTA 20 //define Time quantum QUOTA 20


//define the node structure of Simple Linked List as Struct
typedef struct node
{
	int process_id;//process ID
	int priority;//priority of process
	int computing_time;//service time
	int arrival_time;//arrive time
	int left_time;//variables that store the remaining time over time
	int turnaroundtime;//variables that store the turnaroundtime

	struct node* next;//pointer variables that can refer to their type of Struct(self-referenced structures)
}NODE;

//define required pointers as Struct in a Simple Linked List
typedef struct list {
	NODE* head;//a pointer pointing to the beginning
	NODE* tail;//a pointer pointing to the end
	NODE* cur;//pointer that points to the current location of the node to be output(for output)
	NODE* cal;//pointer that points to the location of the node under calculation(for calculation)
}LIST;

void NodeInit(LIST* plist);//Function that initializes pointer variables
void Insert_connect(LIST* plist, NODE new, int* P_TIME);//Function that adds new nodes, inserts data into nodes and connects new nodes with existing ones
void NodeRemove(LIST* plist);//Function for removing scheduled nodes from memory
void MIDDLE_SCHEDULING(LIST* plist, int* SUM, int* ADD_LEFT);//Function that schedules as much as the amount of time quantum when type is 1 and accumulates value to the sum and calculates the time remaining.
void FINAL_SCHEDULING(LIST* plist, int* P_TIME, int* SUM);//Function to schedule processes that are not scheduled in the order in which they are connected to the Simple Linked List
void Form();//Function for initial screen setup
void Data_Output(LIST* plist, int* SUM);//Function for calculating the turn around time value and outputting the data values of the completed node

int main()
{
	int type;//Type variables that determine process processing
	int p_time = 0;//Variables representing the current time
	int* P_TIME = &p_time;
	int sum = 0;//Variables used to accumulate values for calculating turn around time
	int* SUM = &sum;
	//When the QUOTA is greater than the remaining time of the process, 
	//the process completes scheduling with part of QUOTA, and then scheduled with the remaining QUOTA in the next process. The variable that stores the remaining QUOTA.
	int add_left = 0;
	int* ADD_LEFT = &add_left;

	LIST list;//Declare a struct variable that provides access to a pointer variable that is a member variable

	NODE new;//Declare Struct variables to store data values read from text files in each member variable in the Struct

	Form();//Call function for initial screen setting

	FILE* fp = fopen("input.txt", "r");//Open a file as read-only after creating a file pointer

	if (fp == NULL)
	{
		printf("File read error!");
		exit(1);
	}

	NodeInit(&list); //Pointer variable initialization

	while (fscanf(fp, "%d %d %d %d", &type, &new.process_id, &new.priority, &new.computing_time) != EOF)//Read the data to the end of the file and save it to each member variable
	{
		if (type == 0) {//Process creation, data insertion
			Insert_connect(&list, new, P_TIME);
		}
		else if (type == 1) {//The lapse of time and starting process scheduling
			*P_TIME += QUOTA;//The lapse of time as much as time quantum
			MIDDLE_SCHEDULING(&list, SUM, ADD_LEFT);
		}
		else if (type == -1) {//Input completed. There is no more data to accept, so the remaining processes are scheduled and completed
			FINAL_SCHEDULING(&list, P_TIME, SUM);
			break;
		}
	}

	fclose(fp);//Close a file

	return 0;
}
//Function that schedules as much as the amount of time quantum when type is 1 and accumulates value to the sum and calculates the time remaining.
void MIDDLE_SCHEDULING(LIST* plist, int* SUM, int* ADD_LEFT)
{
	//Case :the remaining time of the node to which cal is pointing is greater than or equal to the time quantum
	if (plist->cal->left_time >= QUOTA) 
	{
		plist->cal->left_time -= QUOTA;//As this is scheduled as much as time quota, subtract the time quantum from the remaining time
		*SUM += QUOTA;//Add Time quantum to sum
		//If the remaining time of the node to which cal points is zero, the process must be terminated, so move the node to which cal points to the next node and call the Data_Output()
		if (plist->cal->left_time == 0) 
		{
			plist->cal = plist->cal->next;
			Data_Output(plist, SUM);
		}
	}
	//If the remaining time of the node which cal is pointing to is less than the time quantum, consider not only the node which the current cal is pointing to, but also the next node being able to schedule
	else if (plist->cal->left_time < QUOTA) 
	{
		*SUM += plist->cal->left_time;
		//If the remaining time of the node which cal points to is less than time quantum, calculate add_left value because the next node can be scheduled additionally for the remaining time 
		*ADD_LEFT = QUOTA - (plist->cal->left_time);
		plist->cal->left_time = 0;
		plist->cal = plist->cal->next;//Move the node to which cal points to the next node
		Data_Output(plist, SUM);//Call Data_Output() to output completed process data values

		*SUM += *ADD_LEFT;
		plist->cal->left_time -= *ADD_LEFT;
		*ADD_LEFT = 0;//Reset add_left to zero
	}
}

//Function to schedule processes that are not scheduled in the order in which they are connected to the Simple Linked List
void FINAL_SCHEDULING(LIST* plist, int* P_TIME, int* SUM)
{
	while (1)//Run while until the remaining processes are scheduled
	{
		//If the node which Cal points is NULL within an infinite repeat statement, the program is shut down because all processes are scheduled
		if (plist->cal == NULL) 
		{
			printf("\n\t==================================FINISH=====================================\n\n");
			break;
		}

		//If the left_time value of the node which cal points to is not 0, the process is not yet scheduled, so perform the scheduling of the node which cal points to
		if (plist->cal->left_time != 0) {
			*P_TIME += plist->cal->left_time;
			*SUM += plist->cal->left_time;
			plist->cal->left_time = 0;
			plist->cal = plist->cal->next;//Move the node which is cal points to next node(to start scheduling next process because of finishing process scheduling)
			Data_Output(plist, SUM);//Call Data_Output() to output completed process data values
		}
	}

}
//Function that adds new nodes, inserts data into nodes and connects new nodes with existing ones
void Insert_connect(LIST* plist, NODE new, int* P_TIME)
{
	//Assign a new node to insert
	NODE* newnode = (NODE*)malloc(sizeof(NODE));

	//store each data
	newnode->process_id = new.process_id;
	newnode->priority = new.priority;
	newnode->computing_time = new.computing_time;
	newnode->arrival_time = *P_TIME;
	newnode->left_time = new.computing_time;
	
	newnode->next = NULL;//Initialize next location

	//If the head is NULL, that is, if the node is added for the first time, the head is pointing to newnode
	if (plist->head == NULL) {
		plist->head = newnode;
		plist->cal = plist->head;//Cal points to the newnode that is newly introduced (for scheduling)
		plist->cur = plist->head;//Cur points to the newnode that is newly introduced (for output)
	}
	
	//If head is not NULL, that is, since add node secondly, connect to front node
	else
		plist->tail->next = newnode;
	//Always tail points to newnode
	plist->tail = newnode;
}

//Function for initial screen setup
void Form()
{
	printf("\n\t==================================FCFS=====================================\n\n");
	printf("\tProcess ID \t Priority \t Computing TIme \t turn_around time\n");
}

//Function for calculating the turn around time value and outputting the data values of the completed node
void Data_Output(LIST * plist, int* SUM)
{
	//turnaroundtime = sum(time to accumulated computing_time) - arrival_time

	//Enter the calculated turn around time value for the node to which cur is pointing
	plist->cur->turnaroundtime = (*SUM - plist->cur->arrival_time);

	//Outputs data values stored on the node to which cur is pointing
	printf("%13d\t%14d\t%15d\t%25d\n", plist->cur->process_id, plist->cur->priority, plist->cur->computing_time, plist->cur->turnaroundtime);

	NodeRemove(plist);//call a function to remove a scheduled node

}

//Function to initialize pointer variables to prevent waste values of pointer variables
void NodeInit(LIST * plist) {
	plist->head = NULL;
	plist->tail = NULL;
	plist->cur = NULL;
	plist->cal = NULL;
}

//Function for removing scheduled nodes from memory
void NodeRemove(LIST * plist) {
	NODE* buf = plist->cur;//Create temporary buffer to point to the node to which cur points
	plist->cur = plist->cur->next;//Move the node to which cur points to the next node before removing the node
	free(buf);//Use free() to remove temporary buffs (remove scheduled nodes)
}
