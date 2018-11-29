/*
 * switches.c
 *
 *  Created on: Oct 25, 2018
 *      Author: tdarrah
 */
#include "input.h"


//wrapper to read a gpio pin
int read_pin(int gpio_num)
{
	//variable to hold the character read from the file
	char file_val;
	//format the fp string with the gpio number (i.e. %d)
	sprintf(fp, "/sys/class/gpio/gpio%d/value", gpio_num);
	//open the file
	file = fopen(fp, "r");
	if (file==NULL)
	{
		perror("");
		printf("Error reading level for pin: %d\n", gpio_num);
		return -1;
	}
	else
		//return the first char in the file
		file_val = getc(file);
	//use ascii math to turn the char number to an int number
	return (int)file_val - '0';
}

/*****************************************************  LIMIT CALLBACKS ***/

//limit callbacks
int x_left_limit_watcher()
{
	pthread_mutex_lock(&x_left_flagM);
	if(x_left_flag == 0)
		x_left_flag = 1;
	pthread_mutex_unlock(&x_left_flagM);
	printf("x left limit reached.\n");
	return 0;
}
int x_right_limit_watcher()
{
	pthread_mutex_lock(&x_right_flagM);
	if(x_right_flag == 0)
		x_right_flag = 1;
	pthread_mutex_unlock(&x_right_flagM);
	printf("x right limit reached.\n");
	return 0;
}
int y_left_limit_watcher()
{
	pthread_mutex_lock(&y_left_flagM);
	if(y_left_flag == 0)
		y_left_flag = 1;
	pthread_mutex_unlock(&y_left_flagM);
	printf("y left limit reached.\n");
	return 0;
}
int y_right_limit_watcher()
{
	pthread_mutex_lock(&y_right_flagM);
	if(y_right_flag == 0)
		y_right_flag = 1;
	pthread_mutex_unlock(&y_right_flagM);
	printf("y right limit reached.\n");
	return 0;
}
int stop_limit_watcher()
{
	pthread_mutex_lock(&stop_flagM);
	stop_flag = 1;
	pthread_mutex_unlock(&stop_flagM);
	printf("stop signal detected.\n");
	return 0;
}

/*****************************************************  JOYSTICK CALLBACKS ***/
//joystick callbacks
int x_l_joystick_callback()
{
	printf("x left\n");
	pthread_mutex_lock(&x_l_moveM);
	if(x_l_move == 0 && deploy_flag == 0)
	{
		if(read_pin(X_L_JOYSTICK) == 0)
		{
			x_l_move = 1;
			printf("x_l_joystick = 1\n");
		}
	}
	else if(x_l_move == 1)
	{
		if(read_pin(X_L_JOYSTICK) == 1)
		{
			x_l_move = 0;
			printf("x_l_joystick = 0\n");
		}
	}
	pthread_mutex_unlock(&x_l_moveM);
	return 0;
}

int x_r_joystick_callback()
{
	printf("x right\n");
	pthread_mutex_lock(&x_r_moveM);
	if(x_r_move == 0 && deploy_flag == 0)
	{
		if(read_pin(X_R_JOYSTICK) == 0)
		{
			x_r_move = 1;
			printf("x_r_joystick = 1\n");
		}
	}
	else if(x_r_move == 1)
	{
		if(read_pin(X_R_JOYSTICK) == 1)
		{
			x_r_move = 0;
			printf("x_r_joystick = 0\n");
		}
	}
	pthread_mutex_unlock(&x_r_moveM);
	return 0;
}

int y_l_joystick_callback()
{
	printf("y left\n");
	pthread_mutex_lock(&y_l_moveM);
	if(y_l_move == 0 && deploy_flag == 0)
	{
		if(read_pin(Y_L_JOYSTICK) == 0)
		{
			y_l_move = 1;
			printf("y_l_joystick = 1\n");
		}
	}
	else if(y_l_move == 1)
	{
		if(read_pin(Y_L_JOYSTICK) == 1)
		{
			y_l_move = 0;
			printf("y_l_joystick = 0\n");
		}
	}
	pthread_mutex_unlock(&y_l_moveM);
	return 0;
}

int y_r_joystick_callback()
{
	printf("y right\n");
	pthread_mutex_lock(&y_r_moveM);
	if(y_r_move == 0 && deploy_flag == 0)
	{
		if(read_pin(Y_R_JOYSTICK) == 0)
		{
			y_r_move = 1;
			printf("y_r_joystick = 1\n");
		}
	}
	else if(y_r_move == 1)
	{
		if(read_pin(Y_R_JOYSTICK) == 1)
		{
			y_r_move = 0;
			printf("y_r_joystick = 0\n");
		}
	}
	pthread_mutex_unlock(&y_r_moveM);
	return 0;
}

int start_button_callback()
{
	printf("start button toggled!\n");
	pthread_mutex_lock(&start_buttonM);
	if(read_pin(START_BUTTON) == 1 && quit_flag == -1)
	{
		quit_flag = 0;
		printf("setting flag to 0\n");
	}
	else if(read_pin(START_BUTTON) == 0 && quit_flag == 0)
	{
		quit_flag = 1;
		printf("setting flag to 1\n");
	}
	pthread_mutex_unlock(&start_buttonM);
	return 0;
}


//sets the flag to deploy the claw, must be reset elsewhere!
//user presses big red button
int deploy_claw_callback()
{
	pthread_mutex_lock(&deployM);
	if(read_pin(DEPLOY_CLAW) == 0 && deploy_flag == 0)
	{
		printf("deploying claw!\n");
		deploy_flag = 1;
		//initiate the claw deployment
		libsoc_gpio_set_level(claw_signal, HIGH);
		printf("signal sent to arduino!\n");
	}
	pthread_mutex_unlock(&deployM);
	return 0;
}

int ardu_signal_callback()
{
	pthread_mutex_lock(&deployM);
	//claw routine is initiated on arduino
	if(read_pin(ARDU_SIGNAL) == 0 && deploy_flag == 1)
	{
		printf("received signal from arduino on 1!\n");
		deploy_flag = 2;
		libsoc_gpio_set_level(claw_signal, LOW);
		printf("deploy_flag == 2\n");
	}
	//claw routine has finished
	else if(read_pin(ARDU_SIGNAL) == 0 && deploy_flag == 2)
	{
		printf("received signal from arduino on 2!\n");
		deploy_flag = 3;
		printf("deploy_flag == 3\n");
	}
	else if(read_pin(ARDU_SIGNAL) == 0 && deploy_flag == 4)
	{
		printf("received signal from arduino on 4!\n");
		libsoc_gpio_set_level(claw_signal, LOW);
		deploy_flag = 0;
	}
	pthread_mutex_unlock(&deployM);
	return 0;
}


/*****************************************************  INIT INPUTS ***/
int init_inputs()
{
	x_left_flag = 0;
	x_right_flag = 0;
	y_left_flag = 0;
	y_right_flag = 0;
	stop_flag = 0;

	x_l_move = 0;
	x_r_move = 0;
	y_l_move = 0;
	y_r_move = 0;

	quit_flag = 0;

	deploy_flag = 0;

	servo_count = 0;

	pthread_mutexattr_t mymutexattr;
	pthread_mutexattr_init(&mymutexattr);

	//set the limit flag mutexes
	pthread_mutex_init(&x_left_flagM, &mymutexattr);
	pthread_mutex_init(&x_right_flagM, &mymutexattr);
	pthread_mutex_init(&y_left_flagM, &mymutexattr);
	pthread_mutex_init(&y_right_flagM, &mymutexattr);

	//set the joystick flag mutexes
	pthread_mutex_init(&x_l_moveM, &mymutexattr);
	pthread_mutex_init(&x_r_moveM, &mymutexattr);
	pthread_mutex_init(&y_l_moveM, &mymutexattr);
	pthread_mutex_init(&y_r_moveM, &mymutexattr);

	//set the deploy flag mutex
	pthread_mutex_init(&deployM, &mymutexattr);

	//set the start button flag mutex
	pthread_mutex_init(&start_buttonM, &mymutexattr);

	pthread_mutexattr_destroy(&mymutexattr);

	//set the limit gpios
	x_left_limit = libsoc_gpio_request(X_LEFT_SWITCH, LS_GPIO_SHARED);
	x_right_limit = libsoc_gpio_request(X_RIGHT_SWITCH, LS_GPIO_SHARED);
	y_left_limit = libsoc_gpio_request(Y_LEFT_SWITCH, LS_GPIO_SHARED);
	y_right_limit = libsoc_gpio_request(Y_RIGHT_SWITCH, LS_GPIO_SHARED);

	//set the joystick gpios
	x_l_joystick = libsoc_gpio_request(X_L_JOYSTICK, LS_GPIO_SHARED);
	x_r_joystick = libsoc_gpio_request(X_R_JOYSTICK, LS_GPIO_SHARED);
	y_l_joystick = libsoc_gpio_request(Y_L_JOYSTICK, LS_GPIO_SHARED);
	y_r_joystick = libsoc_gpio_request(Y_R_JOYSTICK, LS_GPIO_SHARED);

	deploy_claw = libsoc_gpio_request(DEPLOY_CLAW, LS_GPIO_SHARED);

	start_button = libsoc_gpio_request(START_BUTTON, LS_GPIO_SHARED);

	claw_signal = libsoc_gpio_request(CLAW_SIGNAL, LS_GPIO_SHARED);

	ardu_signal = libsoc_gpio_request(ARDU_SIGNAL, LS_GPIO_SHARED);


	//set the gpio edges of the limits and joystick
	gpio* limits[] = {x_left_limit, x_right_limit, y_left_limit, y_right_limit};
	gpio* joystick[] = {x_l_joystick, x_r_joystick, y_l_joystick, y_r_joystick};

	int ret = 1;
	for(int i = 0; i < 4; i++)
	{
		ret = libsoc_gpio_set_edge(limits[i], FALLING);
		if (ret == EXIT_FAILURE)
		{
			perror("Failed to set gpio edge for limit");
			return -1;
		}
	}

	for(int i = 0; i < 4; i++)
	{
		ret = libsoc_gpio_set_edge(joystick[i], BOTH);
		if (ret == EXIT_FAILURE)
		{
			perror("Failed to set gpio edge for joystick");
			return -1;
		}
	}

	ret = libsoc_gpio_set_direction(claw_signal, OUTPUT);
	if (libsoc_gpio_get_direction(claw_signal) != OUTPUT)
	{
		printf("Failed to set claw signal  direction.\n");
		return -1;
	}
	ret = libsoc_gpio_set_level(claw_signal, LOW);
	if(ret == EXIT_FAILURE)
	{
		perror("failed to set claw signal level low.\n");
		return -1;
	}

	ret = libsoc_gpio_set_edge(ardu_signal, FALLING);
	if(ret == EXIT_FAILURE)
	{
		perror("failed to set ardu signal edge.\n");
		return -1;
	}

	ret = libsoc_gpio_set_edge(deploy_claw, FALLING);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio edge for start button");
		return -1;
	}

	ret = libsoc_gpio_set_edge(start_button, BOTH);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio edge for start button");
		return -1;
	}

	//set the limit switch callbacks
	ret = libsoc_gpio_callback_interrupt(x_left_limit, &x_left_limit_watcher, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(x_right_limit, &x_right_limit_watcher, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(y_left_limit, &y_left_limit_watcher, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(y_right_limit, &y_right_limit_watcher, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}

	//set the joystick callbacks
	ret = libsoc_gpio_callback_interrupt(x_l_joystick, &x_l_joystick_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(x_r_joystick, &x_r_joystick_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(y_l_joystick, &y_l_joystick_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(y_r_joystick, &y_r_joystick_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}

	//set the start button callback
	ret = libsoc_gpio_callback_interrupt(start_button, &start_button_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}

	//set the claw release call back
	ret = libsoc_gpio_callback_interrupt(deploy_claw, &deploy_claw_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}
	ret = libsoc_gpio_callback_interrupt(ardu_signal, &ardu_signal_callback, NULL);
	if (ret == EXIT_FAILURE)
	{
		perror("Failed to set gpio callback\n");
		return -1;
	}



	return 0;
}











