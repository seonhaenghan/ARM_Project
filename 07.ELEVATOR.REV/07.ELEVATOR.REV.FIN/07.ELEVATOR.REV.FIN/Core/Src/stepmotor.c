#include "main.h"
#include "button.h"

// 1분 : 60sec : 1,000,000us * 60 = 60,000,000us
// 1초 : 1000ms ==> 1,000,000us
// rpm : Revolutions per minutes
// 60,000,000 / 4096 / speed(1~13)
#define STEPS_PER_REV 4096
#define IDLE		0
#define FORWARD		1
#define BACKWARD	2
#define STANDBY_MODE	0
#define FORWARD_MODE	1
#define BACKWARD_MODE	2
#define ARRIVAL_MODE	3
extern volatile int exint_signal;

extern char lcd_buff_floor_data[40];
extern char lcd_buff_time[40];

int motor_state = 1;
int elevator_state = 1;
int elevator_mode_state = 0;



int pre_floor = 1;
int floor_gap = 0;
int current_floor = 1;
int floor_arrival = 0;

extern int buzzer_mode;
int temp_step = 0;
int temp_step_count = 0;


extern void led_re_shift();

extern int door_mode;
extern int get_button( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state);
extern uint8_t prev_button1_state;
extern uint8_t prev_button2_state;
extern uint8_t prev_button3_state;
extern uint8_t prev_button4_state;
extern uint8_t prev_button5_state;
extern uint8_t prev_button6_state;



extern void led_rl_shift();
extern void led_lr_shift();

extern void auto_door_sequence();
extern void arrival_buzzer();

extern volatile int exint_signal;


void set_rpm(int rpm)
{
	delay_us(60000000/ STEPS_PER_REV/rpm);
	// 최대 speed 기준 (13) ==> _delay_us(1126)
}


void stepmotor_driver_step(int direction, int step_limit)	// direction ==> Forward or Backward
{
	switch(temp_step)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 3:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 4:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 5:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;

		case 6:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;

		case 7:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;
	}
	if(direction == FORWARD)
	{
		if(temp_step_count <= step_limit)
		{
			temp_step_count++;
			temp_step++;
			temp_step %= 8;

			if(temp_step_count % STEPS_PER_REV == 0)
			{
				elevator_state++; // LCD

			}
		}
		else
		{
			temp_step = 0;
			temp_step_count = 0;
			//door_mode = 2; // ==> Standby
			buzzer_mode = 2;
			elevator_mode_state = ARRIVAL_MODE;
		}
	}

	if(direction == BACKWARD)
	{
		if(temp_step_count <= step_limit)
		{
			temp_step_count++;

			if(temp_step >= 0)
			{
				temp_step--;
				temp_step %= 8;
			}
			else if(temp_step < 0)
			{
				temp_step = 7;
			}

			if(temp_step_count % STEPS_PER_REV == 0)
			{
				elevator_state--;

			}
		}
		else
		{
			temp_step = 0;
			temp_step_count = 0;
			buzzer_mode = 2;
			elevator_mode_state = ARRIVAL_MODE;
		}
	}
}
void ex_int_data()
{

	if (exint_signal == 1)
	{
		if(pre_floor > 1)
		{
			elevator_state = pre_floor;
			floor_gap = pre_floor - 1;
			pre_floor = 1;
			elevator_mode_state = BACKWARD_MODE; // BACKWARD
		}
		else if(pre_floor < 1)
		{
			elevator_state = 1;
			pre_floor = 1;
			elevator_mode_state = FORWARD_MODE;
		}

	}

	if (exint_signal == 2)
	{
		if(pre_floor > 2)
		{
			elevator_state = pre_floor;
			floor_gap = pre_floor - 2;
			pre_floor = 2;
			elevator_mode_state = BACKWARD_MODE;
		}
		else if(pre_floor < 2)
		{
			elevator_state = pre_floor;
			floor_gap = 2 - pre_floor;
			pre_floor = 2;
			elevator_mode_state = FORWARD_MODE; // FORWARD
		}
	}

	if (exint_signal == 3)
	{
		if(pre_floor > 3)
		{
			elevator_state = pre_floor;
			floor_gap = pre_floor - 3;
			pre_floor = 3;
			elevator_mode_state = BACKWARD_MODE;
		}
		else if(pre_floor < 3)
		{
			elevator_state = pre_floor;
			floor_gap = 3 - pre_floor;
			pre_floor = 3;
			elevator_mode_state = FORWARD_MODE; // FORWARD
		}
	}

	if (exint_signal == 4)
	{
		if(pre_floor > 4)
		{
			elevator_state = pre_floor;
			floor_gap = pre_floor - 4;
			pre_floor = 4;
			elevator_mode_state = BACKWARD_MODE;
		}
		else if(pre_floor < 4)
		{
			elevator_state = pre_floor;
			floor_gap = 4 - pre_floor;
			pre_floor = 4;
			elevator_mode_state = FORWARD_MODE; // FORWARD
		}
	}
	if (exint_signal == 5)
	{
		if(pre_floor > 5)
		{
			elevator_state = pre_floor;
			floor_gap = pre_floor - 5;
			pre_floor = 5;
			elevator_mode_state = BACKWARD_MODE;
		}
		else if(pre_floor < 5)
		{
			elevator_state = pre_floor;
			floor_gap = 5 - pre_floor;
			pre_floor = 5;
			elevator_mode_state = FORWARD_MODE; // FORWARD
		}
	}
}

void flow_button_move()
{
	ex_int_data();

	switch(elevator_mode_state)
		{
			case STANDBY_MODE:

				if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
				{
					if(pre_floor > 1)
					{
						elevator_state = pre_floor;
						floor_gap = pre_floor - 1;
						pre_floor = 1;
						elevator_mode_state = BACKWARD_MODE; // BACKWARD
					}
					else if(pre_floor < 1)
					{
						elevator_state = 1;
						pre_floor = 1;
						elevator_mode_state = FORWARD_MODE;
					}

				}

				if (get_button(BUTTON1_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
				{
					if(pre_floor > 2)
					{
						elevator_state = pre_floor;
						floor_gap = pre_floor - 2;
						pre_floor = 2;
						elevator_mode_state = BACKWARD_MODE;
					}
					else if(pre_floor < 2)
					{
						elevator_state = pre_floor;
						floor_gap = 2 - pre_floor;
						pre_floor = 2;
						elevator_mode_state = FORWARD_MODE; // FORWARD
					}
				}

				if (get_button(BUTTON1_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
				{
					if(pre_floor > 3)
					{
						elevator_state = pre_floor;
						floor_gap = pre_floor - 3;
						pre_floor = 3;
						elevator_mode_state = BACKWARD_MODE;
					}
					else if(pre_floor < 3)
					{
						elevator_state = pre_floor;
						floor_gap = 3 - pre_floor;
						pre_floor = 3;
						elevator_mode_state = FORWARD_MODE; // FORWARD
					}
				}

				if (get_button(BUTTON1_GPIO_Port, BUTTON4_Pin, &prev_button4_state) == BUTTON_PRESS)
				{
					if(pre_floor > 4)
					{
						elevator_state = pre_floor;
						floor_gap = pre_floor - 4;
						pre_floor = 4;
						elevator_mode_state = BACKWARD_MODE;
					}
					else if(pre_floor < 4)
					{
						elevator_state = pre_floor;
						floor_gap = 4 - pre_floor;
						pre_floor = 4;
						elevator_mode_state = FORWARD_MODE; // FORWARD
					}
				}
				if (get_button(BUTTON1_GPIO_Port, BUTTON5_Pin, &prev_button5_state) == BUTTON_PRESS)
				{
					if(pre_floor > 5)
					{
						elevator_state = pre_floor;
						floor_gap = pre_floor - 5;
						pre_floor = 5;
						elevator_mode_state = BACKWARD_MODE;
					}
					else if(pre_floor < 5)
					{
						elevator_state = pre_floor;
						floor_gap = 5 - pre_floor;
						pre_floor = 5;
						elevator_mode_state = FORWARD_MODE; // FORWARD
					}
				}
			break;
// if ==> pre_f < current_f
			case FORWARD_MODE:

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				elevator_mode_state = STANDBY_MODE;
			}
			else
			{
				stepmotor_driver_step(FORWARD, floor_gap * STEPS_PER_REV);
				set_rpm(13);
			}
			break;
// if ==> pre_f > current_f
			case BACKWARD_MODE:

				if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
				{
					elevator_mode_state = STANDBY_MODE;
				}
				else
				{

					stepmotor_driver_step(BACKWARD, floor_gap * STEPS_PER_REV);
					set_rpm(13);
				}
			break;

			case ARRIVAL_MODE:
				arrival_buzzer();
				auto_door_sequence();
				exint_signal = 0;

			break;
		}
}


void stepmotor_driver(int step)
{
	switch(temp_step)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

		break;

		case 1:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 2:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 3:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 4:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		break;

		case 5:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;

		case 6:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;

		case 7:
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		break;
	}


}



// btn1 : Forward / backward
// btn2 or btn3 : prestate : idle
void stepmotor_forward_backward_stop()
{
	switch(motor_state)
		{
			case IDLE:
				if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
				{
					motor_state = FORWARD;
				}
			break;

			case FORWARD:
				//stepmotor_driver(FORWARD);
				set_rpm(13);

				if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
				{
					motor_state =BACKWARD;
				}
				if (get_button(BUTTON1_GPIO_Port, BUTTON2_Pin, &prev_button2_state) == BUTTON_PRESS)
				{
					motor_state =IDLE;
				}
			break;

			case BACKWARD:
				//stepmotor_driver(BACKWARD);
				set_rpm(13);

				if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
				{
					motor_state =FORWARD;
				}
				if (get_button(BUTTON1_GPIO_Port, BUTTON3_Pin, &prev_button3_state) == BUTTON_PRESS)
				{
					motor_state =IDLE;
				}
			break;

		}
}

// called by main()
void stepmotor_cw_ccw_control()
{
	switch(motor_state)
	{
		case IDLE:
			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state = FORWARD;
			}
		break;

		case FORWARD:
			//stepmotor_driver(FORWARD);
			set_rpm(13);

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state =BACKWARD;
			}
		break;

		case BACKWARD:
			//stepmotor_driver(BACKWARD);
			set_rpm(13);

			if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, &prev_button1_state) == BUTTON_PRESS)
			{
				motor_state =FORWARD;
			}
		break;

	}
}



//시계 <--> 반시계 회전 반복
// 1바퀴 회전 하는데 필요한 총 step 수 : 4096
// 4096/ 8 (0.7도) == 512sequence
// 모터가 360도 회전 하는데 512개의 sequence 를 진행해야 한다.
// 1 sequence 당 : 0.70312도
// 0.70312 * 512 = 360도
// 이걸 이용하여 동작

void stepmotor_main_test()
{
	while(1)
	{

		for(int i = 0; i < 512; i++) // cw
		{
			for(int j = 0; j < 8; j++) // 1 sequence : 0.75
			{
				stepmotor_driver(j);
				set_rpm(6); // rpm : 13 ==> Max speed
			}
		}

		for(int i = 0; i < 512; i++) // ccw
		{
			for(int j = 7; j >=0 ; j--) // 1 sequence : 0.75
			{
				stepmotor_driver(j);
				// rpm 만큼 wait
				set_rpm(13);
			}
		}

	}
}

void exint_motor_control()
{
	if(exint_signal == 0)
	{
		if(temp_step < STEPS_PER_REV)
		{
			temp_step++;
			temp_step_count = temp_step%8;
			stepmotor_driver(temp_step_count);
			set_rpm(13);
		}
		else
		{
			exint_signal = 3;
			temp_step = 0;
		}
	}

	if(exint_signal == 1)
	{
		if(temp_step < STEPS_PER_REV*2)
		{
			temp_step++;
			temp_step_count = temp_step%8;
			stepmotor_driver(temp_step_count);
			set_rpm(13);
		}
		else
		{
			exint_signal = 3;
			temp_step = 0;
		}
	}
}



