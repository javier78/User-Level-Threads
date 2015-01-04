#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
 * queue.h
 */

#include <stdlib.h>

/*
 * Sorted list type.  You need to fill in the type as part of your implementation.
*/

struct QueueNode;

struct Queue;
typedef struct Queue* QueuePtr;

QueuePtr QCreate();

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 *
 * You need to fill in this function as part of your implementation.
 */
void QDestroy(QueuePtr queue);

/*
 * SLInsert inserts a given object into a sorted list, maintaining sorted
 * order of all objects in the list.  If the new object is equal to a subset
 * of existing objects in the list, then the subset can be kept in any
 * order.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

int enqueue(QueuePtr queue, void *newObj);

/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

void *dequeue(QueuePtr list);

void PrintList(QueuePtr queue, int type);

int getSize(QueuePtr queue);

void *peek(QueuePtr queue);

#endif
