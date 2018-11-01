/*
 * switches.h
 *
 *  Created on: Oct 25, 2018
 *      Author: tdarrah
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <pthread.h>
#include <time.h>

#include "libsoc_gpio.h"
#include "libsoc_debug.h"

#ifndef SRC_SWITCHES_H_
#define SRC_SWITCHES_H_

								//p8 header
#define X_LEFT_SWITCH    46		// 16
#define X_RIGHT_SWITCH   65		// 18
#define Y_LEFT_SWITCH    61		// 26
#define Y_RIGHT_SWITCH   27		// 17

								//p9 header
#define CLAW_RELEASE     60     // 12
#define X_JOYSTICK		  0     //39 (AIN0)
#define Y_JOYSTICK		  1     //40 (AIN1)


gpio* x_left_limit;
gpio* x_right_limit;
gpio* y_left_limit;
gpio* y_right_limit;

gpio* claw_release;
gpio* x_joystick;
gpio* y_joystick;

pthread_mutex_t x_left_flagM;
pthread_mutex_t x_right_flagM;
pthread_mutex_t y_left_flagM;
pthread_mutex_t y_right_flagM;
pthread_mutex_t stop_flagM;

volatile int x_left_flag;
volatile int x_right_flag;
volatile int y_left_flag;
volatile int y_right_flag;
volatile int stop_flag;

/*these functions are callbacks registered to x and y axis limit switch interrupts
 */
int x_left_limit_watcher();
int x_right_limit_watcher();
int y_left_limit_watcher();
int y_right_limit_watcher();

/*callback function registered to a non-limit stop input (such as a button)
 * labeled as stop_limit_watcher for similarity convention
 */
int stop_limit_watcher();

/*initialize the gpio pins of the inputs
 */
int init_inputs();




#endif /* SRC_SWITCHES_H_ */



















