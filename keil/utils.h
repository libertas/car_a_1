#include <stdint.h>


#ifndef UTILS_H
#define UTILS_H


typedef struct
{
	uint16_t *data;
	uint32_t front, rear;
	uint16_t count, max_size;
}	queue;

int init_queue(queue *p, uint16_t buf[], uint16_t max_size);
int in_queue(queue *p, uint16_t c);
int out_queue(queue *p, uint16_t *c);

#endif
