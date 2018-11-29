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
#define X_L_JOYSTICK	 31		//11
#define X_R_JOYSTICK	 112	//30
#define Y_L_JOYSTICK	 60		//13
#define Y_R_JOYSTICK	 30		//14

#define DEPLOY_CLAW     48		//15
#define START_BUTTON	 5  	//17

//actually an output
#define CLAW_SIGNAL     49 	//27

//in from arduino
#define ARDU_SIGNAL		20		//25

//used for reading the level of the gpio pins
FILE *file;
char fp[31];


gpio* x_left_limit;
gpio* x_right_limit;
gpio* y_left_limit;
gpio* y_right_limit;

gpio* deploy_claw;

gpio* x_l_joystick;
gpio* x_r_joystick;
gpio* y_l_joystick;
gpio* y_r_joystick;

gpio* start_button;

//actually an output to the arduino
gpio* claw_signal;

gpio* ardu_signal;

/*mutexes to protect the limit flags
 */
pthread_mutex_t x_left_flagM;
pthread_mutex_t x_right_flagM;
pthread_mutex_t y_left_flagM;
pthread_mutex_t y_right_flagM;
pthread_mutex_t stop_flagM;

/*mutexes to protect the move flags
 */
pthread_mutex_t x_l_moveM;
pthread_mutex_t x_r_moveM;
pthread_mutex_t y_l_moveM;
pthread_mutex_t y_r_moveM;

/*mutex to protect the deploy flag
 */
pthread_mutex_t deployM;

//protexts the quit flag
pthread_mutex_t start_buttonM;


/*limit flags (1 if limit reached, 0 if not)
 */
volatile int x_left_flag;
volatile int x_right_flag;
volatile int y_left_flag;
volatile int y_right_flag;
volatile int stop_flag;

/*joystick flags (1 if move, 0 if not)
 */
volatile int x_l_move;
volatile int x_r_move;
volatile int y_l_move;
volatile int y_r_move;

//flag to quit the game
volatile int quit_flag;

//flag to deploy the servos and stop the x/y steppers
volatile int deploy_flag;

int servo_count;

/************************************************** FUNCTIONS *******/


//wrapper to read a gpio pin, give it the gpio num, not the
//p9.pinNum or p8.pinNum
int read_pin(int gpio_num);

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

/*callbacks for the joystick control, they set the flags
 */
int x_l_joystick_callback();

int x_r_joystick_callback();

int y_l_joystick_callback();

int y_r_joystick_callback();

int quit_flag_callback();

/*initialize the gpio pins of the inputs
 */
int init_inputs();




#endif /* SRC_SWITCHES_H_ */



















