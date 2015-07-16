#include "t_lib.h"

struct Node 
{
	int thread_id;
    	int thread_pri;
    	ucontext_t thread_context;// malloc this struct
	struct Node* next;
};
typedef struct Node tcb;
tcb *ready_head = NULL, *run_head = NULL, *main_c = NULL; // global variable - pointer to head node and main node.

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
  	newNode->thread_context.uc_link = &main_c->thread_context;	
	newNode->next = NULL;
	return newNode;
}

/*Inserts a Node at tail of linked list*/
void InsertAtTail(tcb* newNode)
{
	tcb* temp = ready_head;
	if(ready_head == NULL)
	{
		ready_head = newNode;
		return;
	}
	while(temp->next != NULL)
		temp = temp->next; // Go To last Node
	temp->next = newNode;
}

/* main as a thread - init method to get the context of main and initialize the stack pointer */
void t_init()
{
	main_c = (tcb *) malloc(sizeof(tcb));	
	getcontext(&(main_c->thread_context));
	main_c->thread_pri = 1;
	run_head = main_c;
}

/* to create new threads threads */
int t_create(void (*fct)(int), int id, int pri)
{
	tcb *temp = GetNewNode(pri);	
	makecontext(&temp->thread_context, (void(*) (void))fct, 1, id);
	InsertAtTail(temp);	
}

/* thread yields the control to a thread at the head of ready linked list */
void t_yield()
{
	tcb* tmp = ready_head;
	tcb* last_running;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = run_head;
	last_running = run_head;
	run_head->next=NULL;
	run_head=ready_head;
	if (ready_head->next != NULL)
		ready_head=ready_head->next;
  	swapcontext(&last_running->thread_context, &run_head->thread_context);
}

/* to terminate the currently running thread context and free the memory allocated to the node and thread context*/
void t_terminate()
{
    	tcb* temp1 = run_head;    	
    	run_head = NULL;
    	free(temp1);    
    	run_head = ready_head;
	if(ready_head->next != NULL)
    		ready_head = ready_head->next;    	
    	setcontext(&run_head->thread_context);
}

void t_shutdown()
{
    tcb* temp1 = ready_head;    
    run_head = NULL;
    free(run_head);    
    while(ready_head->next != NULL)
    {
        ready_head = ready_head->next;
        free(temp1);
        temp1 = ready_head;
    }    
    free(ready_head);        
}

