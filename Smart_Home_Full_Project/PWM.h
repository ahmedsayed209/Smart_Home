/*
 * PWM.h
 *
 *  Created on: Mar 20, 2024
 *      Author: Ahmed Sayed
 */

#ifndef PWM_H_
#define PWM_H_
#include "std_types.h"
#define Door_Open_Duty 187.5
#define Door_Close_Duty 0
typedef enum {

	PWM_TIMER0,PWM_TIMER1,PWM_TIMER2
}TIMER_X_Used;


typedef enum
{
	NORMAL_PORT,RESREVED,NON_INVERTED,INVERTED
}PHASE_SIGNAL;

typedef enum
{
	NORMAL,PWM_PHASE_CORRECT,CTC_MODE,FAST_PWM
}WAVE_GENERATION;

typedef enum
{
	no_clk,CLK_1,CLK_8,CLK_64,CLK_256,CLK_1024,CLK_FALLING_EDGE,CLK_RISING_EDGE
}CLK_SELECT;

typedef struct
{

	TIMER_X_Used TIMER;
	WAVE_GENERATION wave_generation;
	PHASE_SIGNAL Signal;
	CLK_SELECT clk;
}PWM_config;


void FAST_PWM_init(PWM_config *confi,uint16 Duty_Cycle);

#endif /* PWM_H_ */
