#include "main.h"

// 84MHz / 1680 == > 50kHz
// T = 20us
// Target period 20m
// ARR = 1000
// 2m 	==> ODR = 100 ==>	ARM 180' turn
// 1m 	==> ODR = 50  ==> 	ARM 0' 	turn
// 1.5m ==> ODR = 75  ==> 	ARM 90' turn
// tim2 에 대한 struct를 물고 와야함

extern TIM_HandleTypeDef htim2;
extern volatile int TIM11_10ms_servomotor_counter;
extern int distance_chk;
extern int elevator_mode_state;

int door_mode = 0;
int arm_rotate_indicator = 0;

void auto_door_sequence()
{

	if(door_mode == 3)
	{
		HAL_TIM_PWM_Stop_IT(&htim2, TIM_CHANNEL_3);
		if(elevator_mode_state == 3)
		{
			HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_3);
			TIM11_10ms_servomotor_counter = 0;
			door_mode = 1;
		}

	}

	// 180' turn
	if(door_mode == 1)
	{
		// get_button ==> timer 0initial
		if(TIM11_10ms_servomotor_counter == 0)
		{
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 110); // door open
		}

		else if(TIM11_10ms_servomotor_counter == 300)
		{
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 10);	// close

			door_mode = 0;
			elevator_mode_state = 0;
		}

	}
}
// origin
void servo_motor_control_main()
{
	while(1)
	{
		// 180' turn
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 100); // 100번 count
		HAL_Delay(1000);
		// 0' turn
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 50); // 50번 count
		HAL_Delay(1000);

	}
}
