#include <stdio.h>
#include "queue.h"
struct QueueNode
{
	void *value;
	int refCounter;
	struct QueueNode *next;
};
typedef struct QueueNode QueueNode;
struct Queue
{
	int size;	//not byte size
	QueueNode *head;
	QueueNode *tail;
};

struct QueueIterator
{
	struct Queue *q;
	QueueNode *current;
};

QueuePtr QCreate()
{
	struct Queue *q=malloc(sizeof(struct Queue));
	q->size=0;
	q->head=NULL;
	
	QueuePtr qp=q;
	return qp;
}

int enqueue(QueuePtr queue, void *newObj)
{
	
	if(!newObj)
		return 0;
	if(!queue)
		return 0;
	if(!(queue->head))
	{
		queue->head=malloc(sizeof(QueueNode));
		queue->head->value=newObj;
		queue->head->next=NULL;
		queue->tail=queue->head;
		queue->size++;
		return 1;
	}
	else
	{
		QueueNode *tmp=malloc(sizeof(QueueNode));
		if(queue->tail == NULL){
			printf("QUEUE tail is null\n");
		}
		queue->tail->next=tmp;
		queue->tail=tmp;
		queue->tail->value=newObj;
		queue->tail->next=NULL;
		queue->size++;
		return 1;
	}
}

//Up to caller to free value!
void *dequeue(QueuePtr queue)
{
	QueueNode *tmp=queue->head;
	if(!queue->head)
		return 0;
	void *test=queue->head->value;
	queue->head=queue->head->next;
	free(tmp);
	queue->size--;
	return test;
}

void *peek(QueuePtr queue)
{
	void *test=(queue->head)->value;
	return test;
}

void QDestroy(QueuePtr queue)
{
	if(!queue)
		return;
	if(queue->size==0)		//if list is empty, just free the list.
	{
		free(queue);
		return;
	}
	QueueNode *it=queue->head;
	QueueNode *next=queue->head->next;
	while(it)
	{
		free(it);
		it=next;
		if(next)
			next=next->next;
	}
	free(queue);
}

/*
 * Prints all the values in the list.
 * @params list: the list to print.
 * @params type: the data type. 0 indicates string, 1 indicates int, and 2 indicates double.
 * 
 */
void PrintList(QueuePtr queue, int type)
{
	QueueNode *it=NULL;
	it=queue->head;
	if(type==0)
	{
		while(it)
		{
			printf("%s\n",((char*)it->value));
			it=it->next;
		}
	}
	else if(type==1)
	{
		while(it)
		{
			printf("%d\n",*((int*)it->value));
			it=it->next;
		}
	}
	else if(type==2)
	{
		while(it)
		{
			printf("%lf\n",*((double*)it->value));
			it=it->next;
		}
	}
}

int getSize(QueuePtr queue)
{
	return queue->size;
}
