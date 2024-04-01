/*
 * dcmotor.h
 *
 *  Created on: Dec 5, 2023
 *      Author: Ahmed Sayed
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_
#include "std_types.h"
#define Max_Timer0 256
typedef enum
{
	anticlock_wise=-1,stopped=0,clockwise=1

}DcMotor_state;


void Dc_Motor_Init(void);

void Dc_MotorRotate(DcMotor_state state,uint8 speed);

void Servo_Motor(uint16 Duty_Cycle);

#endif /* DCMOTOR_H_ */
