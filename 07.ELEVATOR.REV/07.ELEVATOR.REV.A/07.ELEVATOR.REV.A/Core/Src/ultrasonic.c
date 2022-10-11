#include "main.h" // gpio_hal 관련 header

#define TRIG_PORT	GPIOC
#define TRIG_PIN	GPIO_PIN_5

// 여기는 cubeIDE와 만나지 않는 영역이다.
// 한글로 주석을 달아도 깨지지 않는다.

int distance, distance_chk; // 거리 측정 값을 저장하는 변수
int ic_cpt_flag; // Rising Edge, Falling Edge를 Detect했을 때 check 하는 변수

// Rising Edge, Falling Edge Interrupt 가 발생 되면 이곳으로 들어 온다.
 void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  static uint8_t is_first_capture = 0;

  if(htim->Instance == TIM1)
  {
	  if(is_first_capture == 0) // Rising Edge detect!!
	  {
		  __HAL_TIM_SET_COUNTER(htim, 0);	// counter clear
		  is_first_capture = 1; // Rising Edge 를 만났다는 indicator 를 set
	  }
	  else if(is_first_capture == 1)	// falling Edge detect!!!!!
	  {
		  is_first_capture = 0; // 다음 INTRR를 위해 initial
		  distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1); // 현재 까지 count된 값을 읽어 옴
		  ic_cpt_flag  = 1;
	  }
  }
}


void make_trigger()
{
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
	delay_us(2);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

}

extern volatile int TIM11_10ms_ultrasonic_counter;

void ultrasonic_processing()
{
	if(TIM11_10ms_ultrasonic_counter >= 100)	// 1000ms cycle
	{
		TIM11_10ms_ultrasonic_counter = 0;
		make_trigger();
		if(ic_cpt_flag)
		{
			ic_cpt_flag = 0;
			distance = distance * 0.034 / 2; // 1us 가 0.034cm 이동
			distance_chk = distance;
			// 왕복 값을 return 하기 때문에 편도 거리 값을 구하기 위해 /2를 해야함
			printf("distance : %dcm\n", distance);
		}
	}
}





