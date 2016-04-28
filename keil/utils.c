#include "utils.h"


int init_queue(queue *p, uint16_t buf[], uint16_t max_size)
{
	p->data = buf;
	p->front = p->rear = max_size - 1;
	p->count = 0;
	p->max_size = max_size;
	return 0;
}

int in_queue(queue *p, uint16_t c)
{
	if(p->count == p->max_size)
		return -1;
	
	p->rear = (p->rear + 1) % p->max_size;
	p->data[p->rear] = c;
	p->count++;
	
	return 0;
}

int out_queue(queue *p, uint16_t *c)
{
	if(0 == p->count)
		return -1;
	
	p->front = (p->front + 1) % p->max_size;
	*c = p->data[p->front];
	p->count--;
	
	return 0;
}
