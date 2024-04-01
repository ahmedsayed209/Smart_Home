/*
 * adc.c
 *
 *  Created on: Dec 4, 2023
 *      Author: Ahmed Sayed
 */
#include "adc.h"
#include "Gpio.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*
 * This STATIC CONFIGURATION IS JUST FOR USING ADC WITH INTERRUPT MODE
 *  */
#if (ADC_mode ==1)
volatile uint16  data=0;
ISR(ADC_vect)
{
	data = ADC;
}
#endif
/*
 * Function Description : Initialization of adc
 * Function Arguments  : Pointer To Structure to modify the adc with needed dynamic configuration within RUNtime
 *  ADC_Mode mode : is just to initialize ADC with Configuration of pooling or interrupt
 */
void ADC_Init( ADC_ConfigType * Config_Ptr,Adc_Mode mode)
{
	switch(mode)
	{
	case Pooling_ADC:
		ADMUX=(ADMUX&0X3F) | (Config_Ptr->adc_reference<<6);
		ADCSRA=(ADCSRA&0xF8) | (Config_Ptr->adc_scaler);
		SET_BIT(ADCSRA,ADEN);
		break;
	case Interrupt:
		ADMUX=(ADMUX&0X3F) | (Config_Ptr->adc_reference<<6);
		ADCSRA=(ADCSRA&0xF8) | (Config_Ptr->adc_scaler);
		SET_BIT(ADCSRA,ADIE);
		SET_BIT(ADCSRA,ADEN);
		break;
	}
}

/*
 *  This Function To Read Channel of ADC and I make it as static Configuration as read channel :
 * WITH POOLING : WORKS AS SYNCHRONUS FUNCTION
 * WITH INTERRUPT WORKS AS ASYNCHRONUS FUNCTION
 */
#if (ADC_mode == 0)
	uint16 ADC_ReadChannel(uint8 channel_num)
	{
		ADMUX=( ADMUX&0XE0) |((channel_num&0x07));
		SET_BIT(ADCSRA,ADSC);
		while(!(ADCSRA&(1<<ADIF)));
		SET_BIT(ADCSRA,ADIF);
		return ADC;
	}
#elif(ADC_mode == 1)
	void ADC_ReadChannel(uint8 channel_num)
	{
		ADMUX=( ADMUX&0XE0) |((channel_num&0x07));
		SET_BIT(ADCSRA,ADSC);
	}
#endif


