/*
 * types used by thread library
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>

#define MSG_SIZE 1024

struct tcb {
  int         thread_id;
  int         thread_priority;
  ucontext_t  thread_context;
  struct msg *buffer;
  struct tcb *next;
  struct sem_t *msg_sem;
};

typedef struct tcb tcb;

typedef struct {
    int count;
    tcb *q;
} sem_t;

typedef struct msg {
  char       *message;
  int         length;
  int         sender;
  int         receiver;
  struct msg *next;
} msg;

typedef struct {
  struct msg *msg_queue;
  sem_t *sem_mbox;
} mbox;

struct tcb_monitor {
    int tid; // tid of the tcb to point to
    tcb *location;
    struct tcb_monitor *next;
};

typedef struct tcb_monitor tcb_monitor;



