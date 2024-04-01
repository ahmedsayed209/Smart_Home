/*
 * PWM.c
 *
 *  Created on: Mar 20, 2024
 *      Author: Ahmed Sayed
 */

#include <avr/io.h>
#include "PWM.h"
#include "Gpio.h"
#include "common_macros.h"
void FAST_PWM_init(PWM_config *confi,uint16 Duty_Cycle)
{
	if (confi->TIMER == PWM_TIMER0)
	{
		TCNT0=0;
		OCR0=Duty_Cycle;
		GPIO_setupPinDirection(PORTB_ID,PIN3_ID,PIN_OUTPUT);
		TCCR0=(1<<WGM00) | (1<<WGM01);
		TCCR0 = (TCCR0 & 0x4f)   |  ((confi->Signal)<<4);
		TCCR0 = (TCCR0 & 0X78)   |   (confi->clk);
	}

	else if (confi->TIMER == PWM_TIMER1)
	{
		TCNT1 = 0;
		ICR1 = 2499;
		OCR1A = Duty_Cycle;
		GPIO_setupPinDirection(PORTD_ID,PIN5_ID,PIN_OUTPUT);
		TCCR1A = (1<<WGM11) | (1<<COM1A1);
		TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS10) | (1<<CS11);
	}

	else if (confi->TIMER == PWM_TIMER2)
	{
		TCNT2=0;
		OCR2=Duty_Cycle;
		GPIO_setupPinDirection(PORTD_ID,PIN7_ID,PIN_OUTPUT);
		TCCR2=(1<<WGM00) | (1<<WGM01);
		TCCR2 = (TCCR2 & 0x4f)   |  ((confi->Signal)<<4);
		TCCR2 = (TCCR2 & 0X78)   |   (confi->clk);
	}

}
