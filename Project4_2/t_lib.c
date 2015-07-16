/*
phase 1: T1, T1x works perfectly
phase 2: T1a, T2, T2a, T4, T7 works perfectly 
*/
#include "t_lib.h"

struct Node 
{
	int thread_id;
    	int thread_pri;
    	ucontext_t thread_context;// malloc this struct
	struct Node* next;
};
typedef struct Node tcb;
tcb *ready_head_0 = NULL, *run_head = NULL, *main_c = NULL; // global variable - pointer to head node and main node.
tcb *ready_head_1 = NULL; // lower level priority queue, main theread will always be here.
/*Creates a new Node and returns pointer to it.*/
tcb* GetNewNode(int pri)
{
	size_t sz = 0x10000;
	tcb* newNode = (tcb*)malloc(sizeof(tcb));
	newNode->thread_pri = pri;	
	getcontext(&newNode->thread_context); //get the context for newNode	
  	newNode->thread_context.uc_stack.ss_sp = malloc(sz);  /* new statement */
  	newNode->thread_context.uc_stack.ss_size = sz;
  	newNode->thread_context.uc_stack.ss_flags = 0;
  	newNode->thread_context.uc_link = 0;	// get a clarification of the uc_link pointer
	newNode->next = NULL;
	return newNode;
}

/*Inserts a Node at tail of level 0 linked list*/
void InsertAtTail_0(tcb* newNode)
{
	tcb* temp = ready_head_0;
	if(ready_head_0 == NULL)
	{
		ready_head_0 = newNode;
		return;
	}
	while(temp->next != NULL)
		temp = temp->next; // Go To last Node
	temp->next = newNode;
}

/*Inserts a Node at tail of priority 1 linked list*/
void InsertAtTail_1(tcb* newNode)
{
	tcb* temp = ready_head_1;
	if(ready_head_1 == NULL)
	{
		ready_head_1 = newNode;
		return;
	}
	while(temp->next != NULL)
		temp = temp->next; // Go To last Node
	temp->next = newNode;
}
void RRScheduler()
{
	tcb* tmp0 = ready_head_0;
	tcb* tmp1 = ready_head_1;
	tcb* last_running = run_head;
	if (last_running->thread_pri == 0)
	{
		if(ready_head_0 == NULL)
		{
			ualarm(1000, 0);
			return;			
		}
		else
		{			
			if(ready_head_0->next == NULL)
			{
				tmp0 = ready_head_0;
				ready_head_0 = run_head;
				run_head = tmp0;
			}
			else
			{
				tmp0 = ready_head_0;
				last_running = run_head;
				while (tmp0->next != NULL)
					tmp0 = tmp0->next;
				tmp0->next = last_running;
				run_head->next = NULL;
				run_head = ready_head_0;
				ready_head_0 = ready_head_0->next;
				run_head->next = NULL;
			}
		}
		ualarm(1000, 0);
		swapcontext(&last_running->thread_context, &run_head->thread_context);		
	}
	else
	{
		if (ready_head_0 == NULL)
		{			
			if (tmp1 == NULL)
			{
				ualarm(1000, 0);
				return;
			}
			else
			{
				while (tmp1->next != NULL)
					tmp1 = tmp1->next;
				tmp1->next = last_running;
				last_running->next=NULL;
				run_head=ready_head_1;
				if (ready_head_1->next != NULL)
					ready_head_1=ready_head_1->next;
			}
		}
		else
		{			
			if (tmp1 == NULL)
			{
				tmp1 = last_running;
				last_running->next = NULL;
				run_head = tmp0;
				
				if (ready_head_0->next != NULL)
					ready_head_0 = ready_head_0->next;
				else
					ready_head_0=NULL;
				run_head->next = NULL;
			}
			else
			{
				while (tmp1->next != NULL)
					tmp1 = tmp1->next;
				tmp1->next = last_running;
				last_running->next=NULL;
				run_head=ready_head_0;
				if (ready_head_0->next != NULL)
					ready_head_0=ready_head_0->next;
				else
					ready_head_0=NULL;
			}
		}
	ualarm(1000, 0);
	swapcontext(&last_running->thread_context, &run_head->thread_context);
	}	
}

void sig_func(int sig_no)
{
	if(ready_head_0 == NULL && ready_head_1 == NULL)
	{    		
    		printf("\nTIMER INTERRUPT...\n\n");
		ualarm(1000,0);
		return;
	}
	RRScheduler();	
}

/* main as a thread - init method to get the context of main and initialize the stack pointer */
void t_init()
{
	main_c = (tcb *) malloc(sizeof(tcb));	
	getcontext(&(main_c->thread_context));
	main_c->thread_pri = 1;
	run_head = main_c;
	
	signal(SIGALRM, sig_func); /* set signal handler for SIGALRM interrupt */

  	ualarm(1000, 0);  /* alarm in 1 micro-second */
}

/* to create new threads threads */
int t_create(void (*fct)(int), int id, int pri)
{
	tcb *temp = GetNewNode(pri);
	temp->thread_id = id;
	makecontext(&temp->thread_context, (void(*) (void))fct, 1, id);
	if(pri == 0)
		InsertAtTail_0(temp);
	else
		InsertAtTail_1(temp);
}

/* thread yields the control to a thread at the head of ready linked list */
void t_yield()
{
	ualarm(0,0); 				/////ualarm is canceled but it needs to be restarted at proper place
	tcb* tmp0 = ready_head_0;
	tcb* tmp1 = ready_head_1;
	tcb* last_running = run_head;
	// if the currently running thread is of pri 0, then put it at the end of pri 0 and swap with priority 0 head
	if (last_running->thread_pri == 0) //umbrella if, for priority 0
	{
		if(ready_head_0 == NULL)
		{
			// only one high priority thread exist. keep running it untill it terminates
			// so no context switch
			return;			
		}
		else
		{
			// since their are other threads with priority 0, so swap context them.
			if(ready_head_0->next == NULL)
			{
				// swap between only two threads
				tmp0 = ready_head_0;
				ready_head_0 = run_head;
				run_head = tmp0;
				// swapcontext call remaining.
			}
			else
			{
				//swap between running and ready_head_0
				tmp0 = ready_head_0;
				last_running = run_head;
				while (tmp0->next != NULL)
					tmp0 = tmp0->next;	//go to the last node aka tail of ready_head_0
				tmp0->next = last_running;
				run_head->next = NULL;
				run_head = ready_head_0;
				ready_head_0 = ready_head_0->next;
				run_head->next = NULL;
				
				//swapcontext call remaining.
			}
		}
		swapcontext(&last_running->thread_context, &run_head->thread_context);		
	}
	else //umbrella else for last_running thread priority as 1
	{
		if (ready_head_0 == NULL)//if ready_head_0 is empty
		{
			//since priority 0 queue is null, swapping will be within the priority 1 queue only.
			if (tmp1 == NULL)
			{
				tmp1 = last_running;
				last_running->next = NULL;
				run_head = tmp1;
				run_head->next = NULL;
				if (tmp1->next != NULL)
					tmp1 = tmp1->next;
				/*
				* Or I can just return from this call, both way works
				*/
			}
			else
			{
				while (tmp1->next != NULL)
					tmp1 = tmp1->next;
				tmp1->next = last_running;
				last_running->next=NULL;
				run_head=ready_head_1;
				if (ready_head_1->next != NULL)
					ready_head_1=ready_head_1->next;
			}
		}
		else//if ready head has some nodes in it
		{
			// swap running pri 1 node with pri 0 queue
			if (tmp1 == NULL)
			{
				tmp1 = last_running;
				last_running->next = NULL;
				run_head = tmp0;
				
				if (ready_head_0->next != NULL)
					ready_head_0 = ready_head_0->next;
				else
					ready_head_0=NULL;
				run_head->next = NULL;
			}
			else
			{
				while (tmp1->next != NULL)
					tmp1 = tmp1->next;
				tmp1->next = last_running;
				last_running->next=NULL;
				run_head=ready_head_0;
				if (ready_head_0->next != NULL)
					ready_head_0=ready_head_0->next;
				else
					ready_head_0=NULL;
			}
		}
	swapcontext(&last_running->thread_context, &run_head->thread_context);
	}
}

/* to terminate the currently running thread context and free the memory allocated to the node and thread context*/            // some issues exist
void t_terminate()
{
    	tcb *temp1 = run_head, *tmp0=ready_head_0,*tmp1=ready_head_1;
	int pri = run_head->thread_pri;
   	
	
		if (ready_head_0 != NULL)
		{
			run_head = ready_head_0;
			if (ready_head_0->next != NULL)
				ready_head_0 = ready_head_0->next;
			else
				ready_head_0 = NULL;
		}
		else
		{
			run_head = ready_head_1;
			if (ready_head_1->next != NULL)
				ready_head_1 = ready_head_1->next;
			else
				ready_head_1 = NULL;
		}
		free(temp1);
	
  	setcontext(&run_head->thread_context);
}

void t_shutdown()
{
    	tcb* temp1 = ready_head_0;    
    	run_head = NULL;
    	free(run_head);    
	if (ready_head_0 != NULL)
	{
    		while(ready_head_0->next != NULL)
    		{
    		    ready_head_0 = ready_head_0->next;
    		    free(temp1);
    		    temp1 = ready_head_0;
    		}    
    		free(ready_head_0);		
	}
	temp1 = ready_head_1;
	if (ready_head_1 != NULL)
	{
		while(ready_head_1->next != NULL)
    		{
    		    ready_head_1 = ready_head_1->next;
    		    free(temp1);
    		    temp1 = ready_head_1;
    		}    
    		free(ready_head_1);    	    
	}
}
