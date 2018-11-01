/*
 * switches.c
 *
 *  Created on: Oct 25, 2018
 *      Author: tdarrah
 */
#include "input.h"


int x_left_limit_watcher()
{
	pthread_mutex_lock(&x_left_flagM);
	x_left_flag = 1;
	pthread_mutex_unlock(&x_left_flagM);
	printf("x left limit reached.\n");
	return 0;
}
int x_right_limit_watcher()
{
	pthread_mutex_lock(&x_right_flagM);
	x_right_flag = 1;
	pthread_mutex_unlock(&x_right_flagM);
	printf("x right limit reached.\n");
	return 0;
}
int y_left_limit_watcher()
{
	pthread_mutex_lock(&y_left_flagM);
	y_left_flag = 1;
	pthread_mutex_unlock(&y_left_flagM);
	printf("y left limit reached.\n");
	return 0;
}
int y_right_limit_watcher()
{
	pthread_mutex_lock(&y_right_flagM);
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



int init_inputs()
{
	pthread_mutexattr_t mymutexattr;
	pthread_mutexattr_init(&mymutexattr);
	pthread_mutex_init(&x_left_flagM, &mymutexattr);
	pthread_mutex_init(&x_right_flagM, &mymutexattr);
	pthread_mutex_init(&y_left_flagM, &mymutexattr);
	pthread_mutex_init(&y_right_flagM, &mymutexattr);
	pthread_mutexattr_destroy(&mymutexattr);

	gpio* x_left_limit = libsoc_gpio_request(X_LEFT_SWITCH, LS_GPIO_SHARED);
	gpio* x_right_limit = libsoc_gpio_request(X_RIGHT_SWITCH, LS_GPIO_SHARED);
	gpio* y_left_limit = libsoc_gpio_request(Y_LEFT_SWITCH, LS_GPIO_SHARED);
	gpio* y_right_limit = libsoc_gpio_request(Y_RIGHT_SWITCH, LS_GPIO_SHARED);

	gpio* limits[] = {x_left_limit, x_right_limit, y_left_limit, y_right_limit};

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



	return 0;
}











