#include <stdio.h>
#include "mypthread.h"
#include <stdlib.h>
#include "queue.h"
#include "testthread.h"

void handler(int n)
{
	//check eip here.
	//printf("eip: %p\n",*(&n+59));
	//printf("Interrupted thread %d\n",current_tid);
	void *eip=*(&n+59);
	void *beg=begin_app;
	void *end=end_app;
	if(eip>beg && eip<end)
	{
		if(!qp)
			return;
		int *tid=dequeue(qp);
		if(!tid)
			return;
		if(t_table[current_tid].st!=ZOMBIE && t_table[current_tid].st!=SLEEPING)
		{
			t_table[current_tid].st=RUNNABLE;
			int *current_tid_malloc=malloc(sizeof(int));
			*current_tid_malloc=current_tid;
			enqueue(qp,current_tid_malloc);
		}
		if(getSize(qp)==0 && active_threads==1)	//maybe check for active mutexes.
		{
			QDestroy(qp);
			qp=NULL;
		}
		int prev;
		prev=current_tid;
		current_tid=*tid;
		free(tid);
		t_table[current_tid].st=RUNNING;
		//printf("<preemptive> about to swap context with table[%d]\n",current_tid);
		swapcontext(&t_table[prev].ucp,&t_table[current_tid].ucp);
		//printf("Landed in handler.\n");
	}
	else
		//printf("Not a valid eip, returning from handler.\n");
	return;
}

int mypthread_create(mypthread *thread, void (*start_routine) (void))
{	
	if(t_table[0].st==UNUSED)	//initialize main thread, should only run once since we assume only good programmers are using the library.
	{
		signal(SIGALRM,handler);
		alarm.tv_usec=1;
		alarm.tv_sec=0;
		itimer.it_interval=alarm;
		itimer.it_value=alarm;
		setitimer(ITIMER_REAL,&itimer,NULL);	//uncommenting enables preemptive scheduling.
		
		getcontext(&t_table[0].ucp);
		t_table[0].tid=0;
		t_table[0].st=RUNNING;
		main_thread=t_table[0];
		current_tid=0;
		//printf("main thread initialized\n");
		int *tid=malloc(sizeof(int));
		*tid=0;
		active_threads=1;
	}
	if(!qp)
	{
		qp=QCreate();
	}
	int i;
	for (i = 1; i < 256; i++)
	{
		if(t_table[i].st==UNUSED)
		{
			goto found;
		}
	}
	printf("No free threads!\n");
	return -1;
found:
	getcontext(&t_table[i].ucp);
	t_table[i].tid=i;
	t_table[i].ucp.uc_link=NULL;
	t_table[i].ucp.uc_stack.ss_sp=malloc(STACK_SIZE);
	t_table[i].ucp.uc_stack.ss_size=STACK_SIZE;
	t_table[i].ucp.uc_stack.ss_flags=0;
	if(t_table[i].ucp.uc_stack.ss_sp==NULL)
	{
		perror("malloc: stack couldn't be allocated");
		exit(1);
	}
	makecontext(&t_table[i].ucp,start_routine,0);	//Note about args: makecontext accepts more args than this; the extra args are passed in are passed into the function.
	t_table[i].st=READY;
	int *tid=malloc(sizeof(int));
	*tid=i;
	enqueue(qp,tid);
	*thread=t_table[i];
	active_threads++;
	return 0;
}

int mypthread_exit()
{
	//Set state and free stuff here.
	free(t_table[current_tid].ucp.uc_stack.ss_sp);
	//t_table[current_tid].ucp.uc_stack.ss_size=STACK_SIZE;
	//printf("current tid: %d!\n",current_tid);
	t_table[current_tid].st=ZOMBIE;
	//printf("State: %d\n",t_table[current_tid].st);
	//printf("curr's state at exit: %d\n",t_table[current_tid].st);
	active_threads--;
	mypthread_yield();
	return 0;
}

int mypthread_yield()
{
	//Iterate through table here. Could make it into a queue later.
	if(!qp)
		return 0;
	int *tid=dequeue(qp);
	if(tid==NULL)
		return;
	if(t_table[current_tid].st!=ZOMBIE && t_table[current_tid].st!=SLEEPING)
	{
		t_table[current_tid].st=RUNNABLE;
		int *current_tid_malloc=malloc(sizeof(int));
		*current_tid_malloc=current_tid;
		enqueue(qp,current_tid_malloc);
	}
	if(getSize(qp)==0 && active_threads==1)	//maybe check for active mutexes.
	{
		QDestroy(qp);
		qp=NULL;
	}
	int prev;
	prev=current_tid;
	current_tid=*tid;
	free(tid);
	t_table[current_tid].st=RUNNING;
	//printf("about to swap context with table[%d]\n",current_tid);
	swapcontext(&t_table[prev].ucp,&t_table[current_tid].ucp);
}

int mypthread_join(mypthread thread)
{
	int join_tid=thread.tid;
	//printf("join: join tid: %d\n",join_tid);
	if(t_table[join_tid].st==UNUSED)
		return 0;
	while(t_table[join_tid].st!=ZOMBIE && t_table[join_tid].st!=UNUSED)
	{
		//printf("join: Thread %d's state before waiting: %d\n",join_tid,t_table[join_tid].st);
		//printf("join: Thread 2's state before waiting: %d\n",t_table[2].st);
		//printf("join: active threads(should be 3): %d\n",active_threads);
		mypthread_yield();
		//printf("join: yield has returned in join.\n");
		//printf("join: Thread %d's state while waiting: %d\n",join_tid,t_table[join_tid].st);
	}
	
	thread.st=UNUSED;
	return 0;
}

int mypthread_mutex_init(mypthread_mutex *mutex)
{
	int i;
	for (i = 0; i < 256; i++)
	{
		if(m_table[i].st==NOT_INITIALIZED)
		{
			m_table[i].waiting_threads=QCreate();
			m_table[i].mid=i;
			m_table[i].st=UNLOCKED;
			break;
		}
	}
	if(i!=256)
	{
		*mutex=m_table[i];
	}
	else
	{
		return -1;
	}
	
	return 0;
}

int mypthread_mutex_lock(mypthread_mutex *mutex)
{
	int mid=mutex->mid;
	//printf("lock: Thread %d is attempting to lock mutex\n",current_tid);
	if(m_table[mid].st==UNLOCKED)
	{
		//printf("lock: mutex is unlocked!\n");
		m_table[mid].st=LOCKED;
		m_table[mid].owner=current_tid;
	}
	
	else if(m_table[mid].st==LOCKED && m_table[mid].owner==current_tid)
	{
		//printf("lock: Owner is trying to unlock its own mutex!\n");
		return 0;
	}
	else if(m_table[mid].st==LOCKED)
	{
		//printf("lock: mutex is already locked, enqueuing!\n");
		
		int *wtid=malloc(sizeof(int));
		*wtid=current_tid;
		enqueue(m_table[mid].waiting_threads,wtid);
		t_table[current_tid].st=SLEEPING;
		//printf("enqueuing: %d\n", *wtid);
		mypthread_yield();
		m_table[mid].owner=current_tid;
		//Everything done?
	}
	return 0;
}

int mypthread_mutex_trylock(mypthread_mutex *mutex)
{
	int mid=mutex->mid;
	if(m_table[mid].st==UNLOCKED)
	{
		//printf("trylock: Tried to lock, worked!\n");
		mypthread_mutex_lock(mutex);
	}
		
	else
	{
		//printf("trylock: Tried to lock, was already locked!\n");
		return -1;
	}
		
	return 0;
}

int mypthread_mutex_unlock(mypthread_mutex *mutex)
{
	int mid=mutex->mid;
	if(m_table[mid].owner==current_tid && m_table[mid].st==LOCKED)
	{
		if(!m_table[mid].waiting_threads)
			return 0;
		int *tid=dequeue(m_table[mid].waiting_threads);

		//printf("unlock: dequeued\n");
		if(tid)
		{
			t_table[*tid].st=RUNNABLE;
			enqueue(qp,tid);
		}
		else
		{
			//printf("unlock: tid is NULL!\n");
			m_table[mid].owner=-1;
			m_table[mid].st=UNLOCKED;
			return 0;
		}
		
		//printf("unlock: Unlock successful, yielding.\n");
		mypthread_yield();
	}
	else
	{
		//printf("unlock: Invalid unlock\n");
		//printf("unlock: owner: %d\n",m_table[mid].owner);
		//printf("unlock: current tid: %d\n",current_tid);
		//printf("unlock: mutex state: %d\n",m_table[mid].st);
	}
	
	return 0;
}

int mypthread_mutex_destroy(mypthread_mutex *mutex)
{
	int mid=mutex->mid;
	if(m_table[mid].st==UNLOCKED)
	{
		QDestroy(m_table[mid].waiting_threads);
		m_table[mid].waiting_threads=NULL;
		m_table[mid].owner=-1;
		m_table[mid].st==UNUSED;
	}
	return 0;
}
