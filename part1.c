#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define ARRAY_SIZE 70
#define SORT_THREADS ARRAY_SIZE-1

pthread_t *sort_threads;
pthread_t sortCheck;
pthread_mutex_t *element_mutex;
pthread_mutex_t print_lock;
//int arr[ARRAY_SIZE]={5,3,1,9,4,2,6,0,20,8,100,7};
int arr[ARRAY_SIZE];
int sorted;
void *sortArray(void *i)
{
	int* intp=i;
	int x=*intp;
	while(!sorted)
	{
		pthread_mutex_lock(&(element_mutex[x]));
		pthread_mutex_lock(&(element_mutex[x+1]));
		if(arr[x+1]>arr[x])
		{
			int temp=arr[x];
			arr[x]=arr[x+1];
			arr[x+1]=temp;
		}
		pthread_mutex_unlock(&(element_mutex[x]));
		pthread_mutex_unlock(&(element_mutex[x+1]));
	}
	pthread_exit(NULL);
}

void *isSorted(void *i)
{
	while(!sorted)
	{
		int x=0;
		for(x=0;x<ARRAY_SIZE;x++)
		{
			printf("attempting to lock %d\n",x);
			fflush(stdout);
			pthread_mutex_lock(&(element_mutex[x]));
		}
			
		for(x=0;x<SORT_THREADS;x++)
		{
			if(arr[x+1]>arr[x])
				break;
		}
		int un=0;
		for(un=0;un<ARRAY_SIZE;un++)
			pthread_mutex_unlock(&(element_mutex[un]));
		if(x==SORT_THREADS)
			sorted=1;
			
		else
		{
			sleep(5);
		}
	}
	
	return NULL;
}

int main(int argc, char** argv)
{
	sorted=0;
	sort_threads=malloc(sizeof(pthread_t)*SORT_THREADS);
	element_mutex=malloc(sizeof(pthread_mutex_t)*ARRAY_SIZE);
	int i;
	srand(time(NULL));
	for (i = 0; i < ARRAY_SIZE; i++)
	{
		arr[i]=rand()%500;
	}
	for(i=0;i<ARRAY_SIZE;i++)
		pthread_mutex_init(&element_mutex[i],NULL);
	for(i=0;i<SORT_THREADS;i++)
	{
		int *test=malloc(sizeof(int));
		*test=i;
		int x=pthread_create(&sort_threads[i],0,sortArray,test);
	}
	pthread_create(&sortCheck,0,isSorted,NULL);
	pthread_join(sortCheck,NULL);
	for(i=0;i<SORT_THREADS;i++)
		pthread_join(sort_threads[i],NULL);
	for (i = 0; i < ARRAY_SIZE; i++)
	{
		pthread_mutex_destroy(&element_mutex[i]);
	}
	
	for(i=0;i<ARRAY_SIZE;i++)
		printf("%d\n",arr[i]);

	return 0;
}
