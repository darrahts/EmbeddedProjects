/*
 * keyboard.c
 *
 *  Created on: Nov 26, 2018
 *      Author: tdarrah
 */

#include "keyboard.h"

//this sets the keyboard to read a character without pressing return
//used to simulate joystick control using:
// a (x-left), d (x-right), w (y-left), s (y-right)
void acquire_keyboard(struct termios* old, struct termios* new)
{
    //tcgetattr gets the parameters of the current terminal
    //STDIN_FILENO will tell tcgetattr that it should write the settings
    //of stdin to old
    tcgetattr( STDIN_FILENO, old);

    new = old;

    //ICANON normally takes care that one line at a time will be processed
    //that means it will return if it sees a "\n" or an EOF or an EOL
    new->c_lflag &= ~(ICANON);

    //Those new settings will be set to STDIN
    //TCSANOW tells tcsetattr to change attributes immediately.
    tcsetattr( STDIN_FILENO, TCSANOW, new);
}

//returns stdin to the original settings
void return_keyboard(struct termios* old)
{
	tcsetattr( STDIN_FILENO, TCSANOW, old);

}

//wrapper to get a key from the keyboard
char get_key()
{
	return fgetc(stdin);
}

void keyboard_control(struct Stepper* x_stepper, struct Stepper* y_stepper)
{
	acquire_keyboard(&oldt, &newt);
	//	//create the buffer and the input char (key)
	struct c_buf_t buffer;
	c_buf_init(&buffer);
	char key = ' ';

	args.x_stp = x_stepper;
	args.y_stp = y_stepper;
	args.buf = &buffer;

	pthread_t keyboard_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&keyboard_thread, &attr, execute_command, &args);

	while (!quit_flag)
	{
		key = get_key();
		enqueue(&buffer, key);
		printf("you entered: %c\n", key);
		if (key == 'q')
			break;
		usleep(STEP_SIZE * 5000);
	}
	return_keyboard(&oldt);
	pthread_join(keyboard_thread, NULL);
}

//this is a thread function that dequeues commands from the buffer
//and executes them
void* execute_command(void* args)
{
	struct thread_args* t_args = (struct thread_args*)args;

	while(!quit_flag)
	{
		printf("x step num: %d\n", t_args->x_stp->current_step_number);
		printf("y step num: %d\n", t_args->y_stp->current_step_number);

		printf("checking commands...\n");
		char cmd = dequeue(t_args->buf);
		printf("dequeued: %c\n", cmd);
		if(cmd == 'q')
			break;
		else if(cmd == 'a')
			step(t_args->x_stp, STEP_SIZE, 'l');
			//step_continuous(t_args->x_stp, 'l');
		else if(cmd == 'd')
			step(t_args->x_stp, STEP_SIZE, 'r');
			//step_continuous(t_args->x_stp, 'r');
		else if(cmd == 'w')
			step(t_args->y_stp, STEP_SIZE, 'l');
			//step_continuous(t_args->y_stp, 'l');
		else if(cmd == 's')
			step(t_args->y_stp, STEP_SIZE, 'r');
			//step_continuous(t_args->y_stp, 'r');
		else if(cmd == 'x')
		{
			stop_motor(t_args->x_stp);
			stop_motor(t_args->y_stp);
		}
		else
		{
			stop_motor(t_args->x_stp);
			stop_motor(t_args->y_stp);
		}
	}
	printf("exited the loop!\n");
	return 0;
}
