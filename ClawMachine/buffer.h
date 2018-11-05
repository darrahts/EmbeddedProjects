/*
 * buffer.h
 *
 *  Created on: Oct 8, 2018
 *      Author: esdev
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>


#ifndef SRC_BUFFER_H_
#define SRC_BUFFER_H_

//change this to the desired capacity!
#define CAPACITY 30

//buffer
typedef struct c_buf_t
{
	int in;
	int out;
	int capacity;
	int counter;
	pthread_cond_t cv;
	pthread_mutex_t lock;

	char dataArray[CAPACITY];
} c_buf_t;

//initialize the buffer
void c_buf_init(struct c_buf_t* buf);

//add an item to the buffer
void enqueue(void* buffer, char dir);

//remove an item from the buffer
char dequeue(void* buffer);

void test();

#endif /* SRC_BUFFER_H_ */











