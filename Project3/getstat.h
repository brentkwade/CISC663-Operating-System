/**********************************
*
*	getstat.c for watchmail
*
**********************************/
////
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
int fsize_old, fsize_new;
struct stat st;
char *filename;
int done = 0;	
int t_count;
char tempname[50];
int threadCreateFlag = 0;
pthread_t tid1;
struct threadNode 
{
	char filename[50];
	int f_size_old;

	struct threadNode* threadnext;
};
struct threadNode* thead; // global variable - pointer to head node.

//Creates a new Node and returns pointer to it.
struct threadNode* GetThreadNode(char* fname, int f_size_o) 
{
	struct threadNode* newNode = (struct threadNode*)malloc(sizeof(struct threadNode));
	strcpy(newNode->filename, filename);	
	newNode->f_size_old = f_size_o;
	newNode->threadnext = NULL;
	return newNode;
}
//Inserts a Node at tail of Doubly linked list
void InsertThread(char* fname, int f_size_old)
{
	
	struct threadNode* temp = thead;
	struct threadNode* newNode = GetThreadNode(fname, f_size_old);		
	if(thead == NULL) 
	{		
		thead = newNode;
		return;
	}	
	while(temp->threadnext != NULL)
		temp = temp->threadnext; // Go To last Node
	temp->threadnext = newNode;
}


/* code for thread to keep executing  */
char* readNodeUserMail()		//to get a user from the watchuser linkedlist
{
	int count = 0;
	struct threadNode* temp = thead;
	
	if(t_count == -1)
		t_count = 0;
	else
	{
		while(count != t_count)
		{
			count++;
			if (temp->threadnext == NULL)
			{
				t_count = -1;
				count = -1;
			}
			else 
				temp = temp->threadnext;		
		}	
	}
	if(t_count == -1)
		strcpy(tempname,"NULL1");
	else
	{
		t_count++;
		strcpy(tempname,temp->filename);
		fsize_old = temp->f_size_old;
	}
	return tempname; // this will return the address of local variable, which won't exist after contol exit from this function.So make it global
}
void changeSize()
{
	struct threadNode* temp = thead;
	if(temp->filename != NULL)
	{
		if(strcmp(filename, temp->filename) == 0)
		{
			temp->f_size_old = fsize_new;
			return;		
		}
	}
	while(temp->threadnext != NULL)
	{
		temp = temp->threadnext; // Go To last Node
		if(strcmp(filename, temp->filename) == 0)
		{
			temp->f_size_old = fsize_new;
			return;		
		}
	}
}
static void *mailCheckThread(void *param)
{
	while (1)
	{	if (thead != NULL)
		{
			filename = readNodeUserMail();
			if ( strcmp(filename,"NULL1") != 0)
			{
				stat(filename, &st);
				fsize_new = st.st_size;
				if(fsize_old < fsize_new)
				{
					printf("\n \aYou've Got Mail in %s at %d\n", filename, fsize_new);
					changeSize();
				}
			}
		}
		sleep(3);
	}
	
}
int getfilesize()
{	
	stat(filename, &st);
	fsize_new = st.st_size;
	return fsize_new;
}

void createThread(char* fname)
{
	int fsize;
	if (threadCreateFlag == 0)
	{
		pthread_create(&tid1, NULL, mailCheckThread, "Thread 1");
		threadCreateFlag = 1;
		fsize = getfilesize();
		InsertThread(filename, fsize);		
	}
	else
	{
		fsize = getfilesize();
		InsertThread(fname, fsize);
	}
}

/**/
void deleteMailBox(char *str)
{
	struct threadNode *temp, *temp1;
	int foundflag = 0;
	temp = thead;
	if(strcmp(temp->filename, commandArgv[1]) == 0)
	{		
		thead = thead->threadnext;
		return;
	}
	
	while(temp->threadnext != NULL)
	{
		temp1 = temp->threadnext;
		if(strcmp(temp1->filename, commandArgv[1]) == 0)
		{
			temp->threadnext = temp1->threadnext;
			foundflag = 1;
			return;
		}
	}
	if (foundflag == 1)
	{
		printf("%s : pthread_cancel successfully\n",commandArgv[1]);
		//if (head == NULL)//.//////
			//pthread_exit()
	}		
	else	
		printf("%s : No such thread\n");
}
