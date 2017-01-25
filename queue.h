#ifndef QUEUE_H_
#define QUEUE_H_
#include <stdint.h>
#include <stdbool.h>
#include "planner.h"

#define MAX_QUEUE	64

/*Definition QueueEntry starts here*/
typedef struct{
	uint32_t ID;
	Linear_Motion motion;
}Motion_Entry;

typedef Motion_Entry QueueEntry;
/*Definition QueueEntry ends here*/

typedef struct{
	uint32_t count;
	int32_t front;
	int32_t rear;
	QueueEntry *Buffer;
}Queue;

void CreateQueue(Queue *q, QueueEntry *buffer);
bool QueueEmpty(Queue *q);
bool QueueFull(Queue *q);
bool Append(QueueEntry entry, Queue *q);
bool Serve(QueueEntry *entry, Queue *q);
uint32_t QueueSize(Queue *q);
void ClearQueue(Queue *q);
void QueueFront(QueueEntry *entry, Queue *q);


#endif /* QUEUE_H_ */
