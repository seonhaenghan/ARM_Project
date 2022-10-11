
#include "button.h"

uint8_t prev_button1_state = BUTTON_RELEASE;
uint8_t prev_button2_state = BUTTON_RELEASE;
uint8_t prev_button3_state = BUTTON_RELEASE;

// 1. GPIO 2. PIN No. 3. 이전의 BUTTON 상태 (PRE_BUTTON_STATE)
// Full up button
int get_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state)
{
	uint8_t current_state;
	current_state = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin); // 함수 입력 값을 읽어옴

	if(current_state == BUTTON_PRESS && *prev_button_state == BUTTON_RELEASE)
	{
		*prev_button_state = current_state;
		HAL_Delay(60);	//noise 가 지나가기를 기다린다.

		return BUTTON_RELEASE;		// 아직 버튼이 눌러 지지 않은 것으로 처리 0을 리턴
	}
	else if(current_state == BUTTON_RELEASE && *prev_button_state == BUTTON_PRESS)
	{
		*prev_button_state = current_state; // 릴리즈 상태
		HAL_Delay(30);
		return BUTTON_PRESS; // 버튼을 눌렀다 땐 상태로 판단 ==> 1을 반환
	}
	return BUTTON_RELEASE;  //버튼이 눌렀다 때어진 상태가 아니다.
}

