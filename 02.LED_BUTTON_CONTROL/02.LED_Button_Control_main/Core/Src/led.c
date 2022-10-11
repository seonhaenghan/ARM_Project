#include "led.h"
#include "button.h"

extern uint8_t prev_button1_state;

void button1_ledall_on_off()
{
	static int button1_count = 0;

	if(get_button(BUTTON1_GPIO_Port, BUTTON1_GPIO_Pin, &prev_button1_state)== BUTTON_PRESS)
	{
		button1_count++;
		button1_count %= 2;
		if(button1_count == 1)
		{
			led_all_on();
		}
		else if(button1_count == 0)
		{
			led_all_off();
		}
	}
}

void led2_toggle()
{
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // DEMO Board의 LED2
	HAL_Delay(500);
}

void led_flower_on()
{
	for(int i = 0; i < 4 ; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x08 >> i|0x10 << i, GPIO_PIN_SET);
		HAL_Delay(200);
	}
}

void led_flower_off()
{
	for(int i = 0; i < 4 ; i++)
		{
			HAL_GPIO_WritePin(GPIOB, 0x01 << i|0x80 >> i, GPIO_PIN_SET);
			HAL_Delay(200);
		}
}

void led_on_1_up()
{
	for(int i = 0 ; i < 8 ; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x01 << i, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOB, 0x01 << i, GPIO_PIN_RESET);
	}
}

void led_on_1_down()
{
	for(int i = 0 ; i < 8 ; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, GPIO_PIN_SET);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, GPIO_PIN_RESET);
	}
}

void led_on_up()
{
	for(int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x01 << i, GPIO_PIN_SET);
		HAL_Delay(200);
	}
}

void led_on_down()
{
	for(int i = 0; i < 8; i++)
	{
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, GPIO_PIN_SET);
		HAL_Delay(200);
	}
}

void led_all_on()
{
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
}

void led_all_off()
{
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
}
