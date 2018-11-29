/*
 * keyboard.h
 *
 *  Created on: Nov 26, 2018
 *      Author: tdarrah
 */

#include "stepper.h"
#include "buffer.h"
#include "input.h"
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>



#ifndef SRC_KEYBOARD_H_
#define SRC_KEYBOARD_H_


//to enable keyboard input without pressing return
static struct termios oldt, newt;


//Arguments that the execute_command thread needs
//(for keyboard input only!)
typedef struct thread_args
{
	struct c_buf_t* buf;
	struct Stepper* x_stp;
	struct Stepper* y_stp;

}thread_args;

//create the argument structure for the execute_command thread
//for keyboard input only!
struct thread_args args;

void acquire_keyboard(struct termios* old, struct termios* new);

void return_keyboard(struct termios* old);

char get_key();

void keyboard_control(struct Stepper* x_stepper, struct Stepper* y_stepper);

void* execute_command(void* args);


#endif /* SRC_KEYBOARD_H_ */
