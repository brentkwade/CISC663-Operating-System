/* 
 * thread library function prototypes
 */
typedef struct Node tcb;
typedef void sem_t;  // for semaphore
void t_create(void(*function)(int), int thread_id, int priority);
void t_yield(void);
void t_init(void);
tcb* GetNewNode(int pri);
void InsertAtTail_0(tcb* newNode);
void InsertAtTail_1(tcb* newNode);
void t_terminate();
void t_shutdown();
void sig_func(int sig_no);
