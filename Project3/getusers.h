/* file: showusers.c
   Ben Miller, for cisc361

   This is just a tiny main() to show how to get utmpx entries and printout
   info about logged in users with entries.  Read some man pages to find out
   how these functions work more...
*/
#include <utmpx.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
pthread_mutex_t    mutex = PTHREAD_MUTEX_INITIALIZER;
int                uselock=1;


	
struct users
{
	char name[10];
	struct users *next;
};
struct users* head_user;

int r_count = -1;	//global user name index pointer for watchuser linkedlist
char name[10];

int userflag = 0; 	//flag to set 1 if user already in the list

void printInserted(char *usr);
void Insert( char*);


/* Thread function */
/*void *threadfunc(void* parm)
{
  	int   loop = 0;
	int   rc;
	
  	//for (loop=0; loop<LOOPCONSTANT; ++loop) 
	{
	    	if (uselock) 
		{
      			rc = pthread_mutex_lock(&mutex);
      			//checkResults("pthread_mutex_lock()\n", rc);
    		}
    		
		//++i; ++j; ++k; ++l;
		//Insert("rahuld");
		
		struct Node* temp = head;
		struct Node* newNode = GetNewNode(usr);
		printInserted(usr);
		if(head == NULL) 
		{
			head = newNode;
			return;
		}
		while(temp->next != NULL) 
			temp = temp->next; // Go To last Node
		temp->next = newNode;
	
		//

	
    		if (uselock) 
		{
      			rc = pthread_mutex_unlock(&mutex);
      			//checkResults("pthread_mutex_unlock()\n", rc);
    		}
  	}
  	return NULL;
}
*/

struct users* GetNewNodeUser(char *usr) 
{
	struct users* newNode = (struct users*)malloc(sizeof(struct users));
	strcpy(newNode->name, usr);
	newNode->next = NULL;	
	return newNode;
}
/* Find if the user is already in the linked list */
void findUser(char *usr)
{
	struct users* temp = head_user;	
	if(temp == NULL) 
	{		
		return;
	}
	while(temp->name != NULL)
	{
		if (strcmp(temp->name, usr) == 0)
		{
			userflag = 1;		//user already exist in the global linked list	
			if (temp->next != NULL)
				temp = temp->next;
			else
				return;
		}	
		else
		{
			if (temp->next != NULL)
				temp = temp->next; // Go To last Node
			else return;
		}
		
	}
	//temp->next = newNode;
}

/* get the log off's of user */
void getLogOff(char *usr)
{
	struct utmpx *up;
	setutxent();			/* start at beginning */
  	while (up = getutxent() )	/* get an entry */
  	{
		if ( up->ut_type == DEAD_PROCESS )	/* only care about logoff's */
    		{
			if (strcmp(up->ut_user, usr) == 0)
			{
      				printf("%s has logged off %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
			}
    		}
  	}
}
/* get log on's of user */
void getLogOn(char *usr)
{
	struct utmpx *up;
	setutxent();			/* start at beginning */
  	while (up = getutxent() )	/* get an entry */
  	{
		if ( up->ut_type == USER_PROCESS )	/* only care about logoff's */
    		{
			if (strcmp(up->ut_user, usr) == 0)
			{
      				printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
			}
    		}
  	}
}
/* 
*	Insert a username into global linked list if user doesnot exist, if user exist, print logon and logoff
*/
void Insert(char* usr) 	
{	
	
	findUser(usr);
	
	if( userflag == 1)
	{
		getLogOn(usr);
		getLogOff(usr);
		userflag = 0;
			
	}
	else
	{
		struct users* temp = head_user;
		struct users* newNode = GetNewNodeUser(usr);
		printInserted(usr);
		if(head_user == NULL) 
		{
			head_user = newNode;
			return;
		}
		while(temp->next != NULL) 
			temp = temp->next; // Go To last Node
		temp->next = newNode;
	}		
}	
void Delete(char *usr)
{
	findUser(usr);
	if( userflag == 1)
	{
		struct users* temp = head_user; struct users* temp1;
		if (strcmp(temp->name, usr) == 0)
		{
			if(temp->next != NULL)
				head_user = head_user->next;
			else head_user = NULL;
			return;
		}
		while (temp->next != NULL)
		{
			temp1 = temp->next;
			if (strcmp(temp1->name, usr) == 0)
			{
				if (temp1->next != 0)
				{
					temp->next = temp1->next;				
				}			
				else
				{
					temp->next = NULL;
				}
			}
			else
			{ 
				if (temp1->next != NULL)
				{
					temp = temp->next;
					temp1 = temp1->next;
				} 
			}
		}
	}
	else
	{
		printf("No such user\n");
	}
}
/* structure creation, user deletion and addition ends here */

/* code for thread to keep executing  */
char* readNodeUser()		//to get a user from the watchuser linkedlist
{
	int count = 0;
	struct users* temp = head_user;
	
	if(r_count == -1)
		r_count = 0;
	else
	{
		while(count != r_count)
		{
			count++;
			if (temp->next == NULL)
			{
				r_count = -1;
				count = -1;
			}
			else 
				temp = temp->next;		
		}	
	}
	if(r_count == -1)
		strcpy(name,"NULL1");
	else
	{
		r_count++;
		strcpy(name,temp->name);
	}
	return name; // this will return the address of local variable, which will not exist after contol exit from this function. So make this global
}


/* to print the watchuser outputto command line */
void printWatch()
{
	char *uname = readNodeUser();
	struct utmpx *up;
	setutxent();			/* start at beginning */	
	//printf("***next user: %s\n",uname);
	while (strcmp("NULL1", uname) != 0)
  	{
		while (up = getutxent() )	/* get an entry */
  		{			
			if(strcmp(up->ut_user, uname) == 0)
      			{
				printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
				//_Exit(0);
			}
  		}	
		strcpy(uname,readNodeUser());
		setutxent();			/* start at beginning again for a reason */		
	}
}

/* to print watch of the recently added user  */
void printInserted(char *usr)
{
	/*	For traversing to the last node name, 
	*	comparing it to the usernames in utmpx system structure 
	*	and printing
	struct Node temp;
	while(temp->next != NULL) 
		temp = temp->next; // Go To last Node
	*/
	printf("------------------------------------\n"); 
	printf("%s is added to list \n", usr);
	struct utmpx *up;
	setutxent();										/* start at beginning */	
	//while (strcmp(usr, (up = getutxent())->ut_user) != 0);					/* get an entry and compare */
	while (up = getutxent())
	{
		if (strcmp(up->ut_user, usr) == 0)
			printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up ->ut_host);
	}
	printf("------------------------------------\n");
}


//int main()
//{
	//pthread_t             threadid;
  	//int                   rc=0;
  	//int                   loop=0;
  	//pthread_attr_t        pta;
	
	//pthread_attr_init(&pta);
	//pthread_attr_setdetachstate(&pta, PTHREAD_CREATE_JOINABLE);
	//rc = pthread_create(&threadid, NULL, threadfunc, NULL);
	//printWatch();	
	
//	Insert("rahuld");
	//printWatch();
	//printf("\n\nnow userflag=%d\n\n",userflag);
	//Insert("rahuld");
	//printf("\n\nnow userflag=%d\n\n",userflag);
	//Insert("rahuld");
	/*
  	Insert("LOGIN");
	//printWatch();

	Insert("reboot");
	//printWatch();
	*/
//	return 0;
//} /* main() */

