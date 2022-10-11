/*
 * UART0.h
 *
 * Created: 2022-08-09 오전 10:45:22
 *  Author: kccistc
 */ 


#ifndef UART0_H_
#define UART0_H_

void init_uart0();
void uart0_transmit(uint8_t data);
void pc_command_processing();
extern void set_rtc_date_time(char *date_time);


#endif /* UART0_H_ */
