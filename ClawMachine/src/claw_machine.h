/*
 * claw_machine.h
 *
 *  Created on: Nov 16, 2018
 *      Author: tdarrah
 */

#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include "stepper.h"
#include "buffer.h"
#include "input.h"
#include "libsoc_pwm.h"


#ifndef SRC_CLAW_MACHINE_H_
#define SRC_CLAW_MACHINE_H_


//for the claw
#define PWM_CHIP 3		//ehrpwm1
#define DEPLOY_SERVO 0  //ehrpwm1A     p9-14
#define CLAW_SERVO 1    //ehrpwm1B	   p9-16
#define FREQ 50.0f      //50hz
#define PERIOD (1.0E3/FREQ) //20ms


//typedef struct Servos
//{
//	pwm* claw_servo;
//	pwm* deploy_servo;
//}Servos;


//wrapper to set a gpio pin
void set_pin(gpio* pin, int state);

//test function to read gpio input
void test_input(gpio* input);

//this sets the keyboard to read a character without pressing return
//used to simulate joystick control using:
// a (x-left), d (x-right), w (y-left), s (y-right)
void acquire_keyboard(struct termios* old, struct termios* new);

//returns stdin to the original settings
void return_keyboard(struct termios* old);

//wrapper to get a key from the keyboard
char get_key();

//this is a thread function that dequeues commands from the buffer
//and executes them (keyboard input only!)
void* execute_command(void* args);


void enable_pwm_pins();


void* x_axis_control(void* stepper);


void* y_axis_control(void* stepper);


void* claw_control(void* servos);

void deploy_the_claw(void* deploy);

void retract_claw(void* deploy);

void claw_grasp(void* claw);

void claw_release(void* claw);


void go_home(struct Stepper* x_stp, struct Stepper* y_stp);



#endif /* SRC_CLAW_MACHINE_H_ */
