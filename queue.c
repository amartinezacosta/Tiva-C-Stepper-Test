#include "queue.h"

void CreateQueue(Queue *q, QueueEntry *Motion_Buffer){
	q->count = 0;
	q->front = 0;
	q->rear = -1;
	q->Buffer = Motion_Buffer;
}

bool QueueEmpty(Queue *q){
	bool empty;
	empty = q->count <= 0 ? true : false;

	return empty;
}

bool QueueFull(Queue *q){
	bool full;
	full = q->count >= MAX_QUEUE ? true : false;

	return full;
}

bool Append(QueueEntry entry, Queue *q){
	if(QueueFull(q)){
		return false;
	}
	else{
		q->count++;
		q->rear = (q->rear + 1) % MAX_QUEUE;
		q->Buffer[q->rear] = entry;
		return true;
	}
}

bool Serve(QueueEntry *entry, Queue *q){
	if(QueueEmpty(q)){
		return false;
	}
	else{
		q->count--;
		*entry = q->Buffer[q->front];
		q->front = (q->front + 1) % MAX_QUEUE;
		return true;
	}
}

uint32_t QueueSize(Queue *q){
	return q->count;
}

void ClearQueue(Queue *q){
	q->count = 0;
	q->front = 0;
	q->rear = -1;
}

void QueueFront(QueueEntry *entry, Queue *q){
	*entry = q->Buffer[q->front];
}
