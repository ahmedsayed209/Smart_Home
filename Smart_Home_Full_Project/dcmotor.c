/*
 * dcmotor.c
 *
 *  Created on: Dec 5, 2023
 *      Author: Ahmed Sayed
 */

#include"dcmotor.h"
#include "Gpio.h"
#include "PWM.h"
void Dc_Motor_Init(void)
{
	GPIO_setupPinDirection(PORTD_ID,PIN2_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTD_ID,PIN3_ID,PIN_OUTPUT);
	GPIO_writePin(PORTD_ID,PIN2_ID,LOGIC_LOW);
	GPIO_writePin(PORTD_ID,PIN3_ID,LOGIC_LOW);
}

void Dc_MotorRotate(DcMotor_state state,uint8 speed)
{	PWM_config config={PWM_TIMER2,FAST_PWM,NON_INVERTED,CLK_64};
	speed=((speed*Max_Timer0)/100);
	FAST_PWM_init(&config,speed);
	switch(state)
	{
	case anticlock_wise:
		GPIO_writePin(PORTD_ID,PIN2_ID,LOGIC_HIGH);
		GPIO_writePin(PORTD_ID,PIN3_ID,LOGIC_LOW);
		break;
	case clockwise:
		GPIO_writePin(PORTD_ID,PIN2_ID,LOGIC_LOW);
		GPIO_writePin(PORTD_ID,PIN3_ID,LOGIC_HIGH);
		break;
	case stopped:
		GPIO_writePin(PORTD_ID,PIN2_ID,LOGIC_LOW);
		GPIO_writePin(PORTD_ID,PIN3_ID,LOGIC_LOW);
		break;

	}
}
void Servo_Motor(uint16 Duty_Cycle)
{
	PWM_config config={PWM_TIMER1,FAST_PWM,NON_INVERTED,CLK_64};
	FAST_PWM_init(&config,Duty_Cycle);
}
