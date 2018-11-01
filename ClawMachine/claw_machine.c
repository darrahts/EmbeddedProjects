
#include <stdbool.h>
#include "stepper.h"
#include "buffer.h"
#include "input.h"
#include "libsoc_pwm.h"

//for the claw
#define PWM_CHIP 3
#define CHIP_OUTPUT 0
#define FREQ 50.0f
#define PERIOD (1.0E3/FREQ)

int read_pin(gpio* pin)
{
	return libsoc_gpio_get_level(pin);
}

void set_pin(gpio* pin, int state)
{
	if(state == 1)
	{
		printf("setting high.\n");
		libsoc_gpio_set_level(pin, HIGH);
	}
	else if(state == 0)
	{
		printf("setting low.\n");
		libsoc_gpio_set_level(pin, LOW);
	}
	else
	{
		printf("bad setting. valid states are 1 and 0.\n");
	}
}



void test_input(gpio* input)
{
	int status = 0;
	for(int i = 0; i < 10; i++)
	{

		usleep(100000);
		status = read_pin(input);
		printf("state is: %d\n", status);
	}
}


int main(void)
{
	/**********************************************************     setup   */
	int exitStatus = EXIT_SUCCESS;

	struct Stepper x_stepper;
	struct Stepper y_stepper;

	init_stepper(&x_stepper, 200, (int[]){A_1, A_2, A_3, A_4}, 'x');
	init_stepper(&y_stepper, 200, (int[]){B_1, B_2, B_3, B_4}, 'y');

	gpio* stepper_pins[] = {x_stepper.pin1, x_stepper.pin2, x_stepper.pin3, x_stepper.pin4,
						y_stepper.pin1, y_stepper.pin2, y_stepper.pin3, y_stepper.pin4};
	gpio* input_pins[] ={x_left_limit, x_right_limit, y_left_limit, y_right_limit};

	pwm* servo = libsoc_pwm_request(PWM_CHIP, CHIP_OUTPUT, LS_PWM_SHARED);
	libsoc_pwm_set_period(servo, PERIOD*1.0E6);
	if(libsoc_pwm_get_period(servo) != PERIOD*1.0E6)
	{
		printf("failed to set period.");
		return -1;
	}

	int ret = init_stepper_pins(stepper_pins);
	if(ret == -1)
	{
		exitStatus = EXIT_FAILURE;
		goto finally;
	}
	ret = init_inputs();
	if(ret == -1)
	{
		exitStatus = EXIT_FAILURE;
		goto finally;
	}
	else
	{
		printf("initialization successful.\n");
		fflush(stdout);
	}


/**************************************************************   main loop   */
	libsoc_set_debug(0);
	set_speed(&x_stepper, 60);

	step(&x_stepper, 50, 'r');
	step_continuous(&x_stepper, 'r');

	printf("Waiting for interrupt. Press 'q' and 'Enter' at any time to exit\n");

	char key = -1;
	while (true) {
		key = fgetc(stdin);
		if (key == 'q')
			goto finally;
	}


/**************************************************************    cleanup   */
	finally:

	stop_motor(&x_stepper);
	stop_motor(&y_stepper);
	libsoc_set_debug(1);
	for(int i = 0; i < 8; i++)
	{
		if(stepper_pins[i])
		{
			if (libsoc_gpio_get_direction(stepper_pins[i]) == OUTPUT)
			{
				set_pin(stepper_pins[i], LOW);
			}
			libsoc_gpio_free(stepper_pins[i]);
		}
	}
	for(int i = 0; i < 4; i++)
	{
		if(input_pins[i])
		{
			if (libsoc_gpio_get_direction(input_pins[i]) == OUTPUT)
			{
				set_pin(input_pins[i], LOW);
			}
			libsoc_gpio_free(input_pins[i]);
		}
	}

	//pthread_join(x_limit_thread, NULL);
	printf("main is finished.\n");
	return exitStatus;
}



void test2(gpio* b1, gpio* in1)
{
	stepper_test();

	int status = 0;
	for(int i = 0; i < 10; i++)
	{

		usleep(50000);
		status = read_pin(in1);
		printf("state is: %d\n", status);
	}
}





