#include "main.h"   // for HAL GPIO handling


#define  BUTTON_PRESS       0     // 버튼을 누르면 0 : active LOW
#define  BUTTON_RELEASE     1     // 버튼을 뗀다.  1

void button1_ledall_on_off();
int get_button( GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *prev_button_state);
