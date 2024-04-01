/*
 * Lm35_Sensor.c
 *
 *  Created on: Nov 29, 2023
 *      Author: Ahmed Sayed
 */
#include "Lm35_Sensor.h"
#include "adc.h"

/*
 *   This Function used to configure the Temperature Module as it read the adc value then make a calculation according to data sheet
 */
uint8 Get_Temp(void)
{
	uint8 temp=0;
	uint16 adc_value=0;
	adc_value=ADC_ReadChannel(PORT_Sensor_Connection_PORTA_ADC);
	temp=(uint8)(( (uint32)adc_value * Maximum_Tempreature_Sensor * Maximum_AREF_VOLTAGE)/(Maximum_Voltage_Sensor * Maximum_Bits_Adc));
	return temp;

}
