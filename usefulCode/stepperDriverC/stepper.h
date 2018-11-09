/*
 * stepper.h
 *
 *  Created on: Oct 23, 2018
 *      Author: tdarrah
 */

#include "input.h"

#ifndef SRC_STEPPER_H_
#define SRC_STEPPER_H_

					//p8 header
#define A_1 66		// 7
#define A_2 69		// 9
#define A_3 45		// 11
#define A_4 47		// 15

#define B_1 67		// 8
#define B_2 68		// 10
#define B_3 44		// 12
#define B_4 26 		// 14

#define STEP_SIZE 20


/* the stepper "object" that is controlled with this "class"
 */
typedef struct Stepper
{
	// Direction of rotation
    char direction;
    char previousDirection;

    // the axis the motor is on, necessary to watch the appropriate
    // limit switches
    char axis;

    // delay between steps, in us, based on speed
    unsigned long stepDelay;

    // total number of steps this motor can take in one rotation
    int stepsPerRotation;

    // time stamp in us of when the last step was taken
	unsigned long lastStepTime;

	// gpio pins for the motor in order A+,A-, B+, B-
	// to determin order, set multimeter to ohms and probe the motor
	// A/A or B/B will have little to no resistance, OL will be
	// the reading of A/B or B/A. if this check is verified and the motor
	// behaves unexpectedly, swap one pair at a time and check results
	// i.e. you could have A-/A+ instead of A+/A- (same with B+/B-)
	gpio* pin1;
	gpio* pin2;
	gpio* pin3;
	gpio* pin4;

    // which step the motor is on (base 4) to determine the correct
   	// firing of the gpio pins, do not modify or use
    int __stepNum__;

} Stepper;

/*initializes the gpio pins
 *
 * @param pins[] : array of gpio pins for the 4-wire stepper motor
				  in order (a+, a-, b+, b-)
 */
int init_stepper_pins(gpio* pins[]);

/*initialize the stepper
 *
 * @param stepper           :
 * @param stepsPerRotation  :
 * @param pins[]            :
 * @param axis              :
 */
void init_stepper(struct Stepper* stepper, int stepsPerRotation, int pins[4], char axis);

//test function
void stepper_test();

/*set speed, calculates delays within coil firing
 *
 * @param stepper           :
 * @param desiredSpeed      :
 */
void set_speed(struct Stepper* stepper, long desiredSpeed);

/*step a given number of steps where numSteps is an int > 0, dir is either 'l' or 'r'
 *
 * @param stepper           :
 * @param numSteps          :
 * @param dir               :
 */
int step(struct Stepper* stepper, int numSteps, char dir);

/*continuously step until event (i.e. limit switch)
 *
 * @param stepper           :
 * @param dir               :
 */
int step_continuous(struct Stepper* stepper, int dir);

/*writes the gpio pins low
 *
 * @param stepper           :
 */
void stop_motor(struct Stepper* stepper);

/*withdraws the motor from the limit position and resets the flag
 *
 * @param stepper           :
 */
void __reset_limit_flags__(struct Stepper* stepper);

/*writes the gpio pins, do not modify or use
 *
 * @param stepper           :
 */
int __step_motor__(struct Stepper* stepper);

#endif /* SRC_STEPPER_H_ */










