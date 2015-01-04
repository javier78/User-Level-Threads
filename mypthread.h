#include <ucontext.h>
#include "queue.h"
#include <time.h>
#include <sys/time.h>
#define STACK_SIZE 1024*64
typedef enum{UNUSED,READY,SLEEPING,RUNNING,RUNNABLE,ZOMBIE} state;
typedef enum{NOT_INITIALIZED,UNLOCKED,LOCKED} mutex_state;
struct mypthread
{
	/*
	void *esp;	//might not be needed, stored in context
	void *eip;	//Also stored in context.
	*/
	volatile int tid;
	volatile int st;
	ucontext_t ucp;
};

struct mypthread_mutex
{
	QueuePtr waiting_threads;
	volatile int mid;
	volatile int owner;
	int st;
};

typedef struct mypthread_mutex mypthread_mutex;
typedef struct mypthread mypthread;

struct itimerval itimer;
struct timeval alarm;

volatile int active_threads;
volatile mypthread main_thread;		//Might not be needed!
volatile int current_tid;
mypthread t_table[256];
mypthread_mutex m_table[256];
QueuePtr qp;
void handler(int n);
int mypthread_create(mypthread *thread, void (*start_routine) (void));

int mypthread_exit();

int mypthread_yield();

int mypthread_join(mypthread thread);

int mypthread_mutex_init(mypthread_mutex *mutex);

int mypthread_mutex_lock(mypthread_mutex *mutex);

int mypthread_mutex_trylock(mypthread_mutex *mutex);

int mypthread_mutex_unlock(mypthread_mutex *mutex);
