/*
 * buffer.c
 *
 *  Created on: Oct 8, 2018
 *      Author: esdev
 */

#include "buffer.h"


//initialize the buffer
void c_buf_init(struct c_buf_t* buf, int capacity)
{
	pthread_mutex_init(&buf->lock, NULL);
	pthread_cond_init(&buf->cv, NULL);
	buf->capacity = capacity;
	buf->counter = 0;
	buf->in = 0;
	buf->out = 0;
}

void enqueue(void* buffer, char dir)
{
	struct c_buf_t* buf = (struct c_buf_t*)buffer;

	pthread_mutex_lock(&buf->lock);
	while(!(buf->counter < buf->capacity))
	{
		pthread_cond_wait(&buf->cv, &buf->lock);
	}

	buf->dataArray[buf->in % buf->capacity] = dir;
	printf("enqueuing: %c\n", dir);
	buf->in++;
	buf->counter++;

	pthread_cond_broadcast(&buf->cv);
	pthread_mutex_unlock(&buf->lock);

	if(buf->in == 16)
	{
		buf->in = 0;
	}
}

char dequeue(void* buffer)
{
	struct c_buf_t* buf = (struct c_buf_t*)buffer;
	pthread_mutex_lock(&buf->lock);
	while(!(buf->counter > 0))
	{
		pthread_cond_wait(&buf->cv, &buf->lock);
	}
	printf("no wait.\n");
	fflush(stdout);

	char dir = buf->dataArray[buf->out % buf->capacity];
	printf("denqueuing: %c\n", dir);
	buf->out++;
	buf->counter--;

	pthread_cond_broadcast(&buf->cv);
	pthread_mutex_unlock(&buf->lock);

	if(buf->out == 16)
	{
		buf->out = 0;
	}
	return dir;
}


void test(struct c_buf_t* buf)
{
	printf("this is a test.\n");
	char testChar = 'U';
	printf("created char: %c\n", testChar);
	enqueue(buf, testChar);
	char test = dequeue(buf);
	printf("char is: %c\n", test);
}
