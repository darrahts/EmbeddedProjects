
#include "claw_machine.h"



//wrapper to set a gpio pin
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






//void enable_pwm_pins()
//{
//	system("/home/debian/enablePWM.sh");
//}

void* x_axis_control(void* stepper)
{
	struct Stepper* x_stp = (struct Stepper*)stepper;
	while(!quit_flag)
	{
		while(x_l_move)
		{
			step(x_stp, STEP_SIZE, 'l');
		}
		while(x_r_move)
		{
			step(x_stp, STEP_SIZE, 'r');
		}
		if(deploy_flag == 0)
			stop_motor(x_stp);
	}
	return NULL;
}

void* y_axis_control(void* stepper)
{
	struct Stepper* y_stp = (struct Stepper*)stepper;
	while(!quit_flag)
	{
		while(y_l_move)
		{
			step(y_stp, STEP_SIZE, 'l');
		}
		while(y_r_move)
		{
			step(y_stp, STEP_SIZE, 'r');
		}
		if(deploy_flag == 0)
			stop_motor(y_stp);
	}
	return NULL;
}

void go_home(struct Stepper* x_stp, struct Stepper* y_stp)
{
	printf("x step num: %d\n", x_stp->current_step_number);
	printf("y step num: %d\n", y_stp->current_step_number);
	while(x_stp->current_step_number < 1592)
	{
		step(x_stp, 8, 'l');
		usleep(50);
	}
	while(y_stp->current_step_number > 8)
	{
		step(y_stp, 8, 'r');
		usleep(50);
	}
	while(x_stp->current_step_number < 1600)
	{
		step(x_stp, 1, 'l');
		usleep(50);
	}
	while(y_stp->current_step_number > 0)
	{
		step(y_stp, 1, 'r');
		usleep(50);
	}
}

void open_claw(gpio* sig_pin)
{
	if(deploy_flag == 4)
	{
		libsoc_gpio_set_level(claw_signal, HIGH);
	}
}

int main(void)
{
	libsoc_set_debug(1);
	int exitStatus = EXIT_SUCCESS;

	int ret = init_inputs();
	if(ret == -1)
	{
		exitStatus = EXIT_FAILURE;
		goto finally;
	}

	quit_flag = -1;
	while(quit_flag == -1) ;

	/**********************************************************     setup   */

	//initialize the steppers and inputs

	struct Stepper x_stepper;
	struct Stepper y_stepper;

	init_stepper(&x_stepper, 200, (int[]){A_1, A_2, A_3, A_4}, 'x');
	init_stepper(&y_stepper, 200, (int[]){B_1, B_2, B_3, B_4}, 'y');

	gpio* stepper_pins[] = {x_stepper.pin1, x_stepper.pin2, x_stepper.pin3, x_stepper.pin4,
						y_stepper.pin1, y_stepper.pin2, y_stepper.pin3, y_stepper.pin4};

	set_speed(&x_stepper, 60);
	set_speed(&y_stepper, 60);


	//enable_pwm_pins();


	//initialize the servo to deploy the claw and the claw servo
//	pwm* claw = libsoc_pwm_request(PWM_CHIP, CLAW_SERVO, LS_PWM_SHARED);
//	pwm* deploy = libsoc_pwm_request(PWM_CHIP, DEPLOY_SERVO, LS_PWM_SHARED);
//	libsoc_pwm_set_period(claw, PERIOD*1.0E6);
//	if(libsoc_pwm_get_period(claw) != PERIOD*1.0E6)
//	{
//		printf("failed to set period.");
//		return -1;
//	}
//	libsoc_pwm_set_period(deploy, PERIOD*1.0E6);
//	if(libsoc_pwm_get_period(deploy) != PERIOD*1.0E6)
//	{
//		printf("failed to set period.");
//		return -1;
//	}
//	struct Servos servos;
//	servos.claw_servo = claw;
//	servos.deploy_servo = deploy;

	ret = init_stepper_pins(stepper_pins);
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


	//create the threads
	pthread_t keyboard_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
//	pthread_create(&keyboard_thread, &attr, execute_command, &args);

	pthread_t x_thread;
	pthread_t y_thread;
    pthread_t claw_thread;
	pthread_create(&x_thread, &attr, x_axis_control, (void*)&x_stepper);
	pthread_create(&y_thread, &attr, y_axis_control, (void*)&y_stepper);
//  pthread_create(&claw_thread, &attr, servo_control, (void*)&servos);


/**************************************************************   main loop   */
	libsoc_set_debug(0);

	//main loop
	while(!quit_flag)
	{
		if(deploy_flag == 3)
		{
			pthread_mutex_lock(&deployM);
			deploy_flag = 4;
			pthread_mutex_unlock(&deployM);
			printf("going home\n");
			go_home(&x_stepper, &y_stepper);
			open_claw(claw_signal);
			printf("you got a prize!!!\n");
			usleep(5000000);
		}
	}
	printf("exiting...\n");

/**************************************************************    cleanup   */
	finally:

	go_home(&x_stepper, &y_stepper);
	stop_motor(&x_stepper);
	stop_motor(&y_stepper);
	pthread_join(x_thread, NULL);
	printf("x joined...\n");
	pthread_join(y_thread, NULL);
	printf("y joined...\n");
//	pthread_join(claw_thread, NULL);
//	printf("claw joined...\n");
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
	gpio* input_pins[] ={x_left_limit, x_right_limit, y_left_limit, y_right_limit,
						x_l_joystick, x_r_joystick, y_l_joystick, y_r_joystick,
						deploy_claw, start_button};//, servo_feedback};

	for(int i = 0; i < 10; i++)
	{
		if(input_pins[i])
		{
			libsoc_gpio_free(input_pins[i]);
		}
	}
	printf("freeing these\n");
	libsoc_gpio_free(claw_signal);
	libsoc_gpio_free(ardu_signal);

//	libsoc_pwm_free(deploy);

	printf("main is finished.\n");
	return exitStatus;
}








