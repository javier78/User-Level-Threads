#include <stdio.h>
#include "mypthread.h"
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
int end_app(void);
mypthread thread1;
mypthread thread2;
mypthread_mutex m1;
mypthread_mutex m2;
int begin_app(void)
{
	return 0;
}

void child_fn(void)
{
	int x=0;
	mypthread_mutex_lock(&m1);
	mypthread_mutex_lock(&m2);
	for (;x<10000000;x++)
	{
	}
	mypthread_mutex_unlock(&m1);
	mypthread_mutex_unlock(&m2);
	mypthread_exit();
}

int main(int argc, char **argv)
{
	printf("parent thread\n");
	
	mypthread_create(&thread1,child_fn);
	mypthread_create(&thread2,child_fn);
	
	printf("Child threads ready\n");
	int i;
	
	mypthread_mutex_init(&m1);
	mypthread_mutex_init(&m2);
	mypthread_mutex_lock(&m1);
	mypthread_mutex_lock(&m2);
	mypthread_yield();
	
	for (i = 0; i < 10000000; i++)
	{
	}
	
	mypthread_mutex_unlock(&m1);
	mypthread_mutex_unlock(&m2);
	
	//mypthread_yield();
	
	mypthread_join(thread1);
	mypthread_join(thread2);
	mypthread_mutex_destroy(&m1);
	mypthread_mutex_destroy(&m2);
	return 0;
}
int end_app(void)
{
	return 0;
}
