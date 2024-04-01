/*
 * Lm35_Sensor.h
 *
 *  Created on: Nov 29, 2023
 *      Author: Ahmed Sayed
 */

#ifndef LM35_SENSOR_H_
#define LM35_SENSOR_H_
#include "std_types.h"

#define PORT_Sensor_Connection_PORTA_ADC     0
#define Maximum_Tempreature_Sensor 150
#define Maximum_Voltage_Sensor     1.5

uint8 Get_Temp(void);

#endif /* LM35_SENSOR_H_ */
