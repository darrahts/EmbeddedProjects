

#include "stepper.h"

//test function
void stepper_test()
{
	printf("this is a test.\n");

	struct Stepper s;
	s.stepsPerRotation = 200;

	printf("%d\n", s.stepsPerRotation);
}


int init_stepper_pins(gpio* pins[])
{
	libsoc_set_debug(1);
	for(int i = 0; i < 8; i++)
	{
		if(pins[i] == NULL)
		{
			printf("failed to establish pin %d\n", i);
			return -1;
		}
		//outputs
		if(i < 8)
		{
			libsoc_gpio_set_direction(pins[i], OUTPUT);
			if (libsoc_gpio_get_direction(pins[i]) != OUTPUT)
			{
				printf("Failed to set i/o direction.\n");
				return -1;
			}
			libsoc_gpio_set_level(pins[i], LOW);
			if (libsoc_gpio_get_level(pins[i]) != LOW)
			{
				printf("Failed setting gpio level LOW\n");
				return -1;
			}
		}
		//inputs
		else
		{
			libsoc_gpio_set_direction(pins[i], INPUT);
			if (libsoc_gpio_get_direction(pins[i]) != INPUT)
			{
				printf("Failed to set i/o direction.\n");
				return -1;
			}
		}
	}
	return 1;
}


//initialize the stepper
void init_stepper(struct Stepper* stepper, int stepsPerRotation, int pins[4], char axis)
{
	stepper->stepsPerRotation = stepsPerRotation;
	stepper->pin1 = libsoc_gpio_request(pins[0], LS_GPIO_SHARED);
	stepper->pin2 = libsoc_gpio_request(pins[1], LS_GPIO_SHARED);
	stepper->pin3 = libsoc_gpio_request(pins[2], LS_GPIO_SHARED);
	stepper->pin4 = libsoc_gpio_request(pins[3], LS_GPIO_SHARED);
	//'s' means stopped, 'l' means left, 'r' means right
	stepper->direction = 's';
	stepper->previousDirection = 's';
	stepper->axis = axis;
	if(stepper->axis == 'x')
		stepper->current_step_number = 1600;
	else if(stepper->axis == 'y')
		stepper->current_step_number = 0;
}

//sets rpm speed, calculates delays in us for the coil firing
void set_speed(struct Stepper* stepper, long desiredSpeed)
{
	stepper->stepDelay = 60L * 1000L * 1000L / (unsigned long)stepper->stepsPerRotation / (unsigned long)desiredSpeed;
	printf("delay between steps is %ld us\n", stepper->stepDelay);
}

//step a given number of steps
int step(struct Stepper* stepper, int numSteps, char dir)
{
	if(numSteps <= 0)
	{
		printf("number of steps must be an int greater than 0.\n");
		return -1;
	}
	if(dir != 'l' && dir != 'r')
	{
		printf("direction must be (l)eft or (r)ight.\n");
		return -1;
	}
	stepper->previousDirection = stepper->direction;
	stepper->direction = dir;
	int stepsToTake = numSteps;
	//t = clock();
	while(stepsToTake > 0)
	{
		//printf("steps left: %d\n", stepsToTake);
		if(stepper->direction == 'l')
		{
			if(stepper->current_step_number >= 1600)
				return -1;
			stepper->current_step_number++;
			stepper->__stepNum__++;
			if(stepper->__stepNum__ == stepper->stepsPerRotation)
			{
				stepper->__stepNum__ = 0;
			}
		}
		else
		{
			if(stepper->current_step_number <= 0)
				return -1;
			if(stepper->__stepNum__ == 0)
			{
				stepper->__stepNum__ = stepper->stepsPerRotation;
			}
			stepper->__stepNum__--;
			stepper->current_step_number--;
		}
		stepsToTake--;
		if(__step_motor__(stepper) < 0)
			return -1;
		usleep(stepper->stepDelay);
	}
	t = clock() - t;
	//double total_time = (((double)t)/CLOCKS_PER_SEC)*1000;
	//printf("total time: %fms\n", total_time);
	stop_motor(stepper);
	return 0;
}

//continuously step until event (i.e. limit switch)
int step_continuous(struct Stepper* stepper, int dir)
{
	stepper->direction = dir;
	if(stepper->axis == 'x')
	{
		while(1)
		{
			for(int i = 0; i < 4; i++)
			{
				stepper->__stepNum__ = i;
				if(__step_motor__(stepper) < 0)
					return -1;
				usleep(stepper->stepDelay);
			}
		}
	}
	else if(stepper->axis == 'y')
	{
		while(1)
		{
			for(int i = 0; i < 4; i++)
			{
				stepper->__stepNum__ = i;
				if(__step_motor__(stepper) < 0)
					return -1;
				usleep(stepper->stepDelay);
			}
		}
	}
	return 0;
}


void stop_motor(struct Stepper* stepper)
{
	libsoc_gpio_set_level(stepper->pin1, LOW);
	libsoc_gpio_set_level(stepper->pin2, LOW);
	libsoc_gpio_set_level(stepper->pin3, LOW);
	libsoc_gpio_set_level(stepper->pin4, LOW);
}


//sets the gpio pins to the correct motor coil firing
int __step_motor__(struct Stepper* stepper)
{
	//printf("%c\n", stepper->direction);
	int thisStep = abs(stepper->__stepNum__) % 4;
	//printf("step number: %d\n", thisStep);
	if((stepper->axis == 'x' && ((stepper->direction == 'l' && x_left_flag) || (stepper->direction == 'r' && x_right_flag))) ||
		(stepper->axis == 'y' && ((stepper->direction == 'l' && y_left_flag) || (stepper->direction == 'r' && y_right_flag))) || stop_flag)
	{
		printf("%c\t%d\t%d\t%d\t%d\t%d\n", stepper->axis, x_left_flag, x_right_flag, y_left_flag, y_right_flag, stop_flag);
		printf("%d\n", (int)(stepper->axis == 'x' && (!x_left_flag || !x_right_flag)));
		printf("stopping motor!\n");
		stop_motor(stepper);
		__reset_limit_flags__(stepper);
		return -1;
	}
	else if(thisStep == 0)
	{
		libsoc_gpio_set_level(stepper->pin1, HIGH);
		libsoc_gpio_set_level(stepper->pin2, LOW);
		libsoc_gpio_set_level(stepper->pin3, HIGH);
		libsoc_gpio_set_level(stepper->pin4, LOW);
	}
	else if(thisStep == 1)
	{
		libsoc_gpio_set_level(stepper->pin1, LOW);
		libsoc_gpio_set_level(stepper->pin2, HIGH);
		libsoc_gpio_set_level(stepper->pin3, HIGH);
		libsoc_gpio_set_level(stepper->pin4, LOW);
	}
	else if(thisStep == 2)
	{
		libsoc_gpio_set_level(stepper->pin1, LOW);
		libsoc_gpio_set_level(stepper->pin2, HIGH);
		libsoc_gpio_set_level(stepper->pin3, LOW);
		libsoc_gpio_set_level(stepper->pin4, HIGH);
	}
	else if(thisStep == 3)
	{
		libsoc_gpio_set_level(stepper->pin1, HIGH);
		libsoc_gpio_set_level(stepper->pin2, LOW);
		libsoc_gpio_set_level(stepper->pin3, LOW);
		libsoc_gpio_set_level(stepper->pin4, HIGH);
	}
	else
	{
		stop_motor(stepper);
	}
	return 0;
}

//moves the opposite way when a limit switch is hit
void __reset_limit_flags__(struct Stepper* stepper)
{
	char dir = stepper->direction;
	char oppositeDir = stepper->previousDirection;
	if(dir == 'l')
	{
		oppositeDir = 'r';
	}
	else if(dir == 'r')
	{
		oppositeDir = 'l';
	}
	step(stepper, 16, oppositeDir);
	printf("resetting stepper.\n");
	if(stepper->axis == 'x' && dir == 'l')
	{
		printf("resetting x_left_flag.\n");
		pthread_mutex_lock(&x_left_flagM);
		x_left_flag = 0;
		pthread_mutex_unlock(&x_left_flagM);
	}
	else if(stepper->axis == 'x' && dir == 'r')
	{
		printf("resetting x_right_flag.\n");
		pthread_mutex_lock(&x_right_flagM);
		x_right_flag = 0;
		pthread_mutex_unlock(&x_right_flagM);
	}
	else if(stepper->axis == 'y' && dir == 'l')
	{
		printf("resetting y_left_flag.\n");
		pthread_mutex_lock(&y_left_flagM);
		y_left_flag = 0;
		pthread_mutex_unlock(&y_left_flagM);
	}
	else if(stepper->axis == 'y' && dir == 'r')
	{
		printf("resetting y_right_flag.\n");
		pthread_mutex_lock(&y_right_flagM);
		y_right_flag = 0;
		pthread_mutex_unlock(&y_right_flagM);
	}
	else
	{
		stop_motor(stepper);
	}

}










