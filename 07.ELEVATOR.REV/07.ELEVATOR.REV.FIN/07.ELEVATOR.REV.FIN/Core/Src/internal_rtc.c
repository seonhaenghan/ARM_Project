#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern int elevator_state; // 층 정보
RTC_TimeTypeDef sTime = {0};	// 시간
RTC_DateTypeDef sDate = {0};	// 날짜
char lcd_buff_floor_data[40];
char lcd_buff_time[40];
// 예) 22의 BCD의 메모리에 저장된 format
// high nibble, Low nibble
// 0010 0010
// 20 + 2
// ==> 22
uint8_t bin2dec(uint8_t byte)
{
	uint8_t high, low;

	low = byte & 0x0f; // 하위 4비트만 취한다.
	high = ((byte >> 4) & 0x0f) * 10; 		// 00100010 ----> 4bit shift 우측으로 ==> 00000010 * 10 ==> 22

	return high + low;
}

// 10진수 --> BCD			22
uint8_t dec2bin(uint8_t byte)
{
	uint8_t high, low;

	high = ((byte / 10) << 4);
	//00000010 << 4 ==> 00100000
	low = byte % 10;

	return high + low; // 0010 0010
}

// RTC에서 날짜와 시각 정보를 가져오는 함수
void get_rtc_time(void)
{
	static RTC_TimeTypeDef oTime;	// 이전 시각 정보

	// 각 매개 변수에 call by ref 이기 때문에 주소 연산자를 앞에 붙임
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	if(oTime.Seconds != sTime.Seconds)
	{
		// YY-MM-DD HH:mm:SS
		// 01234567890123456789
//		printf("%04d-%02d-%02d %02d:%02d:%02d\n",
//				bin2dec(sDate.Year)+2000, bin2dec(sDate.Month), bin2dec(sDate.Date),
//				bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
		// text lcd의 1번 째 line에 출력

		sprintf(lcd_buff_floor_data,"Floor : 0%d",elevator_state);
		move_cursor(1,0);
		lcd_string(lcd_buff_floor_data);

		sprintf(lcd_buff_time,"TIME %02d:%02d:%02d",bin2dec(sTime.Hours), bin2dec(sTime.Minutes), bin2dec(sTime.Seconds));
		move_cursor(0,0);
		lcd_string(lcd_buff_time);
	}
	oTime.Seconds = sTime.Seconds;
}

// setrtc220830111500
// 012345678901234567
// pc_command_processing에 입력 받음
// data time이라는 변수에는  220830111500의 주소가 넘어온다.
void set_rtc_date_time(char *date_time)
{
	char yy[4], mm[4], dd[4]; // 날짜 정보
	char hh[4], min[4], ss[4]; // 시각 정보

	strncpy(yy, date_time, 2);
	strncpy(mm, date_time+2, 2);
	strncpy(dd, date_time+4, 2);

	strncpy(hh, date_time+6, 2);
	strncpy(min, date_time+8, 2);
	strncpy(ss, date_time+10, 2);

	// 1.ascii ==> int ==> bcd ==> rtc
	sDate.Year = dec2bin(atoi(yy));
	sDate.Month = dec2bin(atoi(mm));
	sDate.Date = dec2bin(atoi(dd));

	sTime.Hours = dec2bin(atoi(hh));
	sTime.Minutes = dec2bin(atoi(min));
	sTime.Seconds = dec2bin(atoi(ss));

	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}

