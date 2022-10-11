#include "main.h"

#define BUTTON1_GPIO_Prot GPIOC
#define BUTTON2_GPIO_Prot GPIOC
#define BUTTON3_GPIO_Prot GPIOC


#define BUTTON1_GPIO_Pin GPIO_PIN_0
#define BUTTON2_GPIO_Pin GPIO_PIN_1
#define BUTTON3_GPIO_Pin GPIO_PIN_2

#define BUTTON_PRESS 0	// 버튼을 눌렀을 때 0 : active low
#define BUTTON_RELEASE 1 	// 버튼을 땐 상태 1

void button1_ledall_on_off();
int get_button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state);
