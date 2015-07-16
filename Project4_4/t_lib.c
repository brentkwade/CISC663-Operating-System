#include "t_lib.h"

tcb *ready_head_0 = NULL, *run_head = NULL, *main_c = NULL; // global variable - pointer to head node and main node.
tcb *ready_head_1 = NULL; // lower level priority queue, main thread will always be here.
tcb *sem_queue_head = NULL; // semaphore queue.
tcb_monitor *monitor_head = NULL;


// creates a new node for the message struct and returns a pointer to the newly allocated block
msg *GetNewMsgNode(int len)
{
    msg *newMsg = (msg*)malloc(sizeof(msg));
    newMsg->length = len;
    newMsg->message = (char*)malloc(1024*sizeof(char));
    newMsg->sender = 0;
    newMsg->receiver = 0;
    newMsg->next = NULL;    
    return newMsg;
}

/*Creates a new Node and returns pointer to it.*/
tcb* GetNewNode(int pri)
{
	size_t sz = 0x10000;
	tcb* newNode = (tcb*)malloc(sizeof(tcb));
	newNode->thread_priority = pri;
    newNode->buffer = GetNewMsgNode(0);
	getcontext(&newNode->thread_context); //get the context for newNode	
  	newNode->thread_context.uc_stack.ss_sp = malloc(sz);  /* new statement */
  	newNode->thread_context.uc_stack.ss_size = sz;
  	newNode->thread_context.uc_stack.ss_flags = 0;
  	newNode->thread_context.uc_link = 0;	// get a clarification of the uc_link pointer
    sem_init(&(newNode->msg_sem), 1);
	newNode->next = NULL;
	return newNode;
}

// creates a new node for the tcb_monitor struct and returns a pointer to the newly allocated block
tcb_monitor* GetNewMonitorNode(int id)
{
    tcb_monitor* newMonitorNode = (tcb_monitor*)malloc(sizeof(tcb_monitor));    
    newMonitorNode->tid = id;
    newMonitorNode->location = NULL;
    newMonitorNode->next = NULL;
    
    return newMonitorNode;
}



void handler(int sig)
{

    scheduler();

}

void scheduler()
{
 
    tcb* tmp0 = ready_head_0;
	tcb* tmp1 = ready_head_1;
	tcb* last_running = run_head;
	// if the currently running thread is of pri 0, then put it at the end of pri 0 and swap with priority 0 head
	if (last_running->thread_priority == 0) //umbrella if, for priority 0
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
		else//if ready head 0 has some nodes in it
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


/* main as a thread - init method to get the context of main and initialize the stack pointer */
void t_init()
{
    signal(SIGALRM, scheduler);
    
	main_c = (tcb *) malloc(sizeof(tcb));
    tcb_monitor* temp = GetNewMonitorNode(0);
    
	getcontext(&(main_c->thread_context));
	main_c->thread_priority = 1;
	
    // insert main thread node into the monitor queue at the head
    temp->location = main_c;
    temp->tid = 0;
    temp->next = NULL;
    
    monitor_head = temp;
	run_head = main_c;
}

/* to create new threads */
int t_create(void (*fct)(int), int id, int pri)
{
	
    tcb_monitor *temp1 = GetNewMonitorNode(id);
    tcb_monitor* temp2 = monitor_head;
 
	tcb *temp = GetNewNode(pri);

	temp->thread_id = id;
	makecontext(&temp->thread_context, (void(*) (void))fct, 1, id);
   
    // traverse to end of monitor queue and insert newly created thread block
    while(temp2->next != NULL)
        temp2 = temp2->next;//go to last node
    
    temp1->location = temp;
    temp1->tid = temp->thread_id;
    temp1->next = NULL;
    
    temp2->next = temp1;
    
    // insert tcb into priority queue
	if(pri == 0)
		InsertAtTail_0(temp);
	else
		InsertAtTail_1(temp);
    
}

/* thread yields the control to a thread at the head of ready linked list */
void t_yield()
{

	tcb* tmp0 = ready_head_0;
	tcb* tmp1 = ready_head_1;
	tcb* last_running = run_head;
	// if the currently running thread is of pri 0, then put it at the end of pri 0 and swap with priority 0 head
	if (last_running->thread_priority == 0) //umbrella if, for priority 0
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
		else//if ready head 0 has some nodes in it
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
	int pri = run_head->thread_priority;
   	
	
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



// Semaphores Logic //

int sem_init(sem_t **sp, int sem_count)
{

    *sp = malloc(sizeof(sem_t));
    (*sp)->count = sem_count;
    (*sp)->q = NULL;

}


void sem_wait(sem_t *s)
{

	tcb *stmp = s->q;
	tcb *last_running = run_head;
	tcb *tmp1=ready_head_1;//for ready 1 queue
	tcb *tmp0=ready_head_0;//for ready 0 queue
	s->count = s->count - 1;
	if (s->count < 0)
	// 0 represents no more threads can get lock, less than 0 indicates the number of threads that are in the semaphore queue
	{
		// put the thread at end of sem queue | and pop another from ready queue
		if (s->q == NULL)
		{
			s->q=run_head;
			s->q->next=NULL;
		}
		else
		{
			while (stmp->next != NULL)
            stmp = stmp->next;//go to last node
			stmp->next = run_head;
			last_running->next = NULL;
		}
		// Now pop thread from queue
		
		if(ready_head_0 != NULL)
		{
			//pop from ready head 1 since ready head 0 is empty
			run_head = ready_head_0;
			ready_head_0 = ready_head_0->next;
			run_head->next=NULL;
		}
		else	//if ready_head_0 is null, so check for ready_head_1
		{
			//pop from ready head 0
			if (ready_head_1 != 0)
			{
				run_head = ready_head_1;
				ready_head_1 = ready_head_1->next;
				run_head->next=NULL;
			}
			else
			{
				printf("Deadlock: Bad resource management. Semaphore says you've not synchronized your threads properly\n");
			}
		}
	}
	else
	{
		//do nothing
	}

	swapcontext(&last_running->thread_context, &run_head->thread_context);
}

void sem_signal(sem_t *s)
{

	tcb *stmp = s->q;
	tcb *last_running = run_head;
	tcb *tmp1=ready_head_1;//for ready 1 queue
	tcb *tmp0=ready_head_0;//for ready 0 queue
	s->count = s->count + 1;
	if (s->count <= 0)
	{
		
		if(s->q->thread_priority == 0)
		{
			while (tmp0->next != NULL)
            tmp0=tmp0->next;	// go to last node
			tmp0->next = stmp;		// put sem waiting thread into ready_head_0 tail.
			if (s->q->next != NULL)
            s->q = s->q->next;
			stmp->next = NULL;
		}
		else
		{
			while (tmp1->next != NULL)
            tmp1=tmp1->next;	// go to last node
			tmp1->next = stmp;		// put sem waiting thread into ready_head_1 tail.
			if (s->q->next != NULL)
            s->q = s->q->next;
			stmp->next = NULL;
		}
	}
	else	// so count is 0, which means their is only one thread in critical section and no waiting threads, so push the running thread into 	ready queue and pop another from ready queue, according to the priority.
	{
		//s->count = s->count + 1;
	}
	//t_yield(); // pop
	//printf("--------------------------------------Return from yield in sem_wait:\t%d\n",run_head->thread_id);
	sigrelse(SIGALRM);
}

 
void sem_destroy(sem_t **s)
{
    free(*s);
}



// Mailbox Functions //


void mbox_create(mbox **mb)
{
    *mb = malloc(sizeof(mbox));
    sem_init(&(*mb)->sem_mbox, 1);
    (*mb)->msg_queue = NULL;
}


void mbox_destroy(mbox **mb)
{
    sem_destroy(&(*mb)->sem_mbox);
    free(*mb);
}




void mbox_withdraw(mbox *mb, char *msg, int *len)
{
    
    struct msg *temp;
    temp = mb->msg_queue;
    
    char *msg1 = malloc(1024);
    
    
    sem_wait(mb->sem_mbox);
    // lock mailbox - critical section locked
    if(mb->msg_queue == NULL)
    {
        printf("No message to withdraw!\n");
        return;
    }
    else
    {
        // withdraw first message from message queue
        strcpy(msg1, temp->message);
        *len = temp->length;
        strcpy(msg, msg1);
        
        // make the msg_queue head point to the new first message in the queue after the first message has been removed
        if(mb->msg_queue->next != NULL)
            mb->msg_queue = mb->msg_queue->next;
      
    }
    // unlock mailbox - critical section unlocked
    sem_signal(mb->sem_mbox);
    
}
void mbox_deposit(mbox *mb, char *msg, int len)
{
    struct msg *temp, *temp1;
    char *msg1 = malloc(1024);

    int a = len;
    
    strcpy(msg1, msg);

    temp1 = mb->msg_queue;
    
    sem_wait(mb->sem_mbox);
    // lock mailbox - critical section locked
    if(mb->msg_queue == NULL)
    {
        mb->msg_queue = GetNewMsgNode(len);
        strcpy(mb->msg_queue->message, msg1);
        mb->msg_queue->length = a;
        mb->msg_queue->next = NULL;
    }
    else
    {
        temp = GetNewMsgNode(len);
        
        strcpy(temp->message, msg1);
        temp->length = a;
        temp->next = NULL;
        
        while(temp1->next != NULL)
            temp1 = temp1->next;
        
        temp1->next = temp;
        
    }
        
    // unlock mailbox - critical section unlocked
    sem_signal(mb->sem_mbox);
}


// Send-Receive Functions
void receive(int *tid, char *msg, int *len)
{
    int run_tid;
    // capture receiver's tid
    run_tid = run_head->thread_id;
    
    tcb_monitor *temp = monitor_head;
    tcb *temp1;
    struct msg *temp2;
    
    char *msg1 = malloc(1024);
    
    // traverse the tcb_monitor queue to retrieve the correct tcb to deposit the message into
    while(temp != NULL)   // traversing the monitor queue
    {
        if(temp->tid == run_tid)        // umbrella IF
        {
            temp1 = temp->location;     // this is a tcb pointer
            
            temp2 = temp1->buffer->next;      // this is the pointer to the buffer head
            
            if(temp2 == NULL)
            {
                *len = 0;
                *tid = 0;
                return;
            }
            else if(*tid == 0)
            {
                // lock the buffer
                sem_wait(temp1->msg_sem);
                // critical section
                *len = temp2->length;
                *tid = temp2->sender;
                strcpy(msg1, temp2->message);
                //unlock the buffer
                sem_signal(temp1->msg_sem);
                
                if(temp1->buffer->next != NULL)
                    temp1->buffer = temp1->buffer->next;
                
                strcpy(msg, msg1);
                return;
            }
            else
            {
                while(temp2->next != NULL)      // traversing the buffer
                {
                    if(*tid == temp2->sender)
                    {
                        // lock the tcb and hence the buffer
                        sem_wait(temp1->msg_sem);
                        // critical section
                        *len = temp2->length;
                        strcpy(msg1, temp2->message);
                        //unlock the tcb and hence the buffer
                        sem_signal(temp1->msg_sem);
                        
                        if(temp1->buffer->next != NULL)
                            temp1->buffer = temp1->buffer->next;
                        
                        strcpy(msg, msg1);
                        return;
                        
                    }
                    else
                    {
                        temp2 = temp2->next;    // moving to next message in buffer
                    }

                }
                return;
            }
        }
        else    // Umbrella ELSE
        {
             temp = temp->next;      // moving to next node in tcb_monitor queue
        }
    }
}

void send(int tid, char *msg, int len)
{
    tcb_monitor *temp = monitor_head;
    tcb *temp1;
    struct msg *temp2;
    struct msg *node = GetNewMsgNode(len);
    int run_tid;
    
    // capture sender's tid
    run_tid = run_head->thread_id;
    
    char *msg1 = malloc(1024);
    strcpy(msg1, msg);
    
    // traverse the tcb_monitor queue to retrieve the correct tcb to deposit the message into
    while(temp != NULL)
    {	
        if(temp->tid == tid)
        {
            temp1 = temp->location;     // note: this is a tcb pointer
            
            // traverse the buffer of this tcb till the end and insert message at the end
            temp2 = temp1->buffer;      // this is the pointer to the buffer head
            
            strcpy(node->message, msg1);
            node->length = len;
            node->receiver = tid;
            node->sender = run_tid;
            node->next = NULL;
            
            if(temp2->message == NULL)
            {
                // lock the tcb and hence the buffer as well
                sem_wait(temp1->msg_sem);
                // critical section : insertion into list
                temp2 = node;
                // unlock the tcb and hence the buffer
                sem_signal(temp1->msg_sem);
            }
            else
            {
                while(temp2->next != NULL)
                    temp2 = temp2->next;// go to last node

                // lock the tcb and hence the buffer as well
                sem_wait(temp1->msg_sem);
                // critical section : insertion into list
                temp2->next = node;
                // unlock the tcb and hence the buffer
                sem_signal(temp1->msg_sem);
                
                break;
            }
        }
        else
        {
            if(temp->next != NULL)
                temp = temp->next;
        }
    }
}



