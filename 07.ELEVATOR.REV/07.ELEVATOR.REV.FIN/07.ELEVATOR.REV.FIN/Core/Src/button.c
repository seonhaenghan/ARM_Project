#include "button.h"

uint8_t prev_button0_state=0;

uint8_t prev_button1_state=BUTTON_RELEASE;
uint8_t prev_button2_state=BUTTON_RELEASE;
uint8_t prev_button3_state=BUTTON_RELEASE;
uint8_t prev_button4_state=BUTTON_RELEASE;
uint8_t prev_button5_state=BUTTON_RELEASE;
uint8_t prev_button6_state=BUTTON_RELEASE;
uint8_t prev_button7_state=BUTTON_RELEASE;

// 1. GPIO 2. GPIO PIN 3. 이전의 버튼
int get_button( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state)
{
	unsigned char current_state;

	current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

	if ((current_state == BUTTON_PRESS) && (*prev_button_state == BUTTON_RELEASE))  // 처음 누른상태
	{
		*prev_button_state = current_state;
		HAL_Delay(30);   // noise가 지나가기를 기다린다.
		return BUTTON_RELEASE;    // 아직 버튼이 눌러 지지 않는것으로 처리 0을 리턴
	}
	else if ((current_state == BUTTON_RELEASE) && (*prev_button_state == BUTTON_PRESS))
	{
		*prev_button_state = current_state;   // 릴리즈 상태
		return BUTTON_PRESS;   // 버튼이 완전히 눌렀다 뗀 상태로 판단 하고 1을 리턴 한다.
	}

	return BUTTON_RELEASE;    // 버튼이 눌렀다 떼어진 상태가 아니다.
}


int get0_button( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state)
{
	unsigned char current_state;

	current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

	if ((current_state == 1) && (*prev_button_state == 0))  // 처음 누른상태
	{
		*prev_button_state = current_state;
		HAL_Delay(30);   // noise가 지나가기를 기다린다.
		return 0;    // 아직 버튼이 눌러 지지 않는것으로 처리 0을 리턴
	}
	else if ((current_state == 0) && (*prev_button_state == 1))
	{
		*prev_button_state = current_state;   // 릴리즈 상태
		return 1;   // 버튼이 완전히 눌렀다 뗀 상태로 판단 하고 1을 리턴 한다.
	}

	return 0;    // 버튼이 눌렀다 떼어진 상태가 아니다.
}
