Welcome to the CISC663-Operating-System wiki! All projects are fully functional and developed in C. Following is the high-level overview of the projects:

Project1:

Contains a doubly linked list demonstration. The project is songs database using linked list data structure. Some part of the code is still buggy (especially with freeing memory that was not malloc'ed) since it was written when I was learning linked list and pointers. The projects following this has lot less memory faults (testes with valgrind).

Project 2:

This is a shell implementation. Some part of the main code skeleton is from an open source online code of BDShell. I call my shell RShell.The skeleton will be updated with a better structure and will be solely my contribution. Built in commands of the shell:

history
which
where
setenv
printenv
pid
pwd
prompt
exit
kill
cd
ls
and all other commands are executed on parent shell and then the output is displayed...

Project 3:
Enhanced the Shell from project 2.

Added commands:

Backgrounding job using '&'
warnload
watchuser
watchmail
noclobber flag
redirection operator ( >, >&, >>, >>& ) 
IPC using pips

Project 4:
User level thread library:

part1:

void t_init(); 
void t_shutdown(); 
int t_create(void (*func)(int), int thr_id, int pri); 
void t_terminate(); 
void t_yield();

part2:

2 level queues 
round robin scheduling

part3:

int sem_init(sem_t *sp, int sem_count); 
void sem_wait(sem_t *sp); 
void sem_signal(sem_t *sp); 
void sem_destroy(sem_t *sp); 
dining philosophers

part4:

mailbox;

int mbox_create(mbox **mb); /* Create a mailbox pointed to by mb. */

void mbox_destroy(mbox **mb); /* Destroy any state related to the mailbox pointed to by mb. */

void mbox_deposit(mbox *mb, char *msg, int len); /* Deposit message msg of length len into the mailbox pointed to by mb. */

void mbox_withdraw(mbox *mb, char *msg, int *len); /* Withdraw the first message from the mailbox pointed to by mb into msg and set the message's length in len accordingly. Similar to receive(). */

message passing;

void send(int tid, char *msg, int len); /* Send a message to the thread whose tid is tid. msg is the pointer to the start of the message, and len specifies the length of the message in bytes. In your implementation, all messages are character strings. */

void receive(int *tid, char *msg, int *len); /* Receive a message from another thread. The caller has to specify the sender's tid in tid, or sets tid to 0 if intends to receive from any thread. Upon returning, the message is stored starting at msg. The tid of the thread that sent the message is stored in tid, and the length of the message is stored in len. The caller of receive() is responsible for allocating the space in which the message is stored. If there is no message for the caller, then both tid and len are set to 0. Even if more than one message awaits the caller, only one message is returned per call to receive(). Messages are received in the order in which they were sent. */

void block_send(int tid, char *msg, int length); /* Send a message and wait for reception. The same as send(), except that the caller does not return until the destination thread has received the message. */

void block_receive(int *tid, char *msg, int *length); /* Wait for and receive a message. Similar to receive() except that the caller will not resume execution until it has received a message. */
