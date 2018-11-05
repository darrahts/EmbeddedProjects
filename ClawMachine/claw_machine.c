
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include "stepper.h"
#include "buffer.h"
#include "input.h"
#include "libsoc_pwm.h"

//for the claw
#define PWM_CHIP 3
#define CHIP_OUTPUT 0
#define FREQ 50.0f
#define PERIOD (1.0E3/FREQ)

//to enable keyboard input without pressing return
static struct termios oldt, newt;

//Arguments that the execute_command thread needs
typedef struct thread_args
{
	struct c_buf_t* buf;
	struct Stepper* x_stp;
	struct Stepper* y_stp;

}thread_args;

//wrapper to read a gpio pin
int read_pin(gpio* pin)
{
	return libsoc_gpio_get_level(pin);
}

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


//test function to read gpio input
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

//this is a thread function that dequeues commands from the buffer
//and executes them
void* execute_command(void* args)
{
	struct thread_args* t_args = (struct thread_args*)args;

	while(1)
	{
		char cmd = dequeue(t_args->buf);
		printf("dequeued: %c\n", cmd);
		if(cmd == 'q')
			break;
		else if(cmd == 'a')
			step_continuous(t_args->x_stp, 'l');
		else if(cmd == 'd')
			step_continuous(t_args->x_stp, 'r');
		else if(cmd == 'w')
			step_continuous(t_args->y_stp, 'l');
		else if(cmd == 's')
			step_continuous(t_args->y_stp, 'r');
		else
		{
			stop_motor(t_args->x_stp);
			stop_motor(t_args->y_stp);
		}

	}
	return 0;
}


int main(void)
{
	/**********************************************************     setup   */
	int exitStatus = EXIT_SUCCESS;

	//initialize the steppers and inputs

	struct Stepper x_stepper;
	struct Stepper y_stepper;

	init_stepper(&x_stepper, 200, (int[]){A_1, A_2, A_3, A_4}, 'x');
	init_stepper(&y_stepper, 200, (int[]){B_1, B_2, B_3, B_4}, 'y');

	gpio* stepper_pins[] = {x_stepper.pin1, x_stepper.pin2, x_stepper.pin3, x_stepper.pin4,
						y_stepper.pin1, y_stepper.pin2, y_stepper.pin3, y_stepper.pin4};
	gpio* input_pins[] ={x_left_limit, x_right_limit, y_left_limit, y_right_limit};

	set_speed(&x_stepper, 120);
	set_speed(&y_stepper, 120);

	//initialize the servo to deploy the claw and the claw servo
	pwm* claw = libsoc_pwm_request(PWM_CHIP, CHIP_OUTPUT, LS_PWM_SHARED);
	pwm* servo = libsoc_pwm_request(PWM_CHIP, CHIP_OUTPUT, LS_PWM_SHARED);
	libsoc_pwm_set_period(claw, PERIOD*1.0E6);
	if(libsoc_pwm_get_period(claw) != PERIOD*1.0E6)
	{
		printf("failed to set period.");
		return -1;
	}
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

	//create the buffer and the input char (key)
	struct c_buf_t buffer;
	c_buf_init(&buffer);
	char key = ' ';


	//create the argument structure for the execute_command thread
	struct thread_args args;
	args.x_stp = &x_stepper;
	args.y_stp = &y_stepper;
	args.buf = &buffer;

	//create the thread
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, execute_command, &args);


/**************************************************************   main loop   */
	libsoc_set_debug(0);
	//set the keyboard to enable input without return
	acquire_keyboard(&oldt, &newt);

	//set_speed(&x_stepper, 60);
	//step(&x_stepper, 50, 'r');
	//step_continuous(&x_stepper, 'r');

	//main loop
	while (true)
	{
		key = get_key();
		enqueue(&buffer, key);
		if (key == 'q')
			goto finally;
		usleep(250000);
	}


/**************************************************************    cleanup   */
	finally:

	stop_motor(&x_stepper);
	stop_motor(&y_stepper);
	return_keyboard(&oldt);
	pthread_join(tid, NULL);
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





