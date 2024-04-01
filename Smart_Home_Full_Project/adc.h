/*
 * adc.h
 *
 *  Created on: Dec 4, 2023
 *      Author: Ahmed Sayed
 */

#ifndef ADC_H_
#define ADC_H_

#include "std_types.h"


/*This To Choose The Configuration Of ADC Mode
 *  For Pooling Let ADC_mode = 0
 *  For Interrupt Let ADC_mode =1
 *
 *  */
#define ADC_mode 0

#define Maximum_AREF_VOLTAGE 2.56
#define Maximum_Bits_Adc 1023

/*===================================================================================
 *                   This To Define All Channels Of ATMEGA32
 *                   ATMEGA32 HAVE 8 CHANNELS IN PORTA
 *====================================================================================*/

#define Chnnel0_ID 0
#define Chnnel1_ID 1
#define Chnnel2_ID 2
#define Chnnel3_ID 3
#define Chnnel4_ID 4
#define Chnnel5_ID 5
#define Chnnel6_ID 6
#define Chnnel7_ID 7

/* ===================================================================================================================
 * THIS ENUM IS DEFINED TO CONFIGURE THE INIT FUNCTION OF ADC TO CHOOSE WHICH CASE YOU NEED TO WORK
 *
 * ===================================================================================================================
 */
typedef enum
{
	Pooling_ADC,Interrupt
}Adc_Mode;
/* ========================================================================================
 *
 * This ENUM IS DEFINED TO CONFIGURE THE REFERENCE VOLT THAT NEEDED FOR ADC
 *
 * They Arranged In the ENUM according to the number of bits in data-sheet
 * ========================================================================================
 *
 */
typedef enum
{
	AREF_Voltage,AVCC_Voltage,Resreved,Internal_REF_Voltage

}ADC_ReferenceVolatge;
/*
 * ==========================================================================================
 * * This ENUM IS DEFINED TO CONFIGURE THE PRESCALLER THAT NEEDED FOR ADC
 *
 * They Arranged In the ENUM according to the number of bits in data-sheet
 *
 * ==========================================================================================
 */
typedef enum
{
	PreScaler_div_2=0x01,PreScaler_div_4,PreScaler_div_8,PreScaler_div_16,PreScaler_div_32,PreScaler_div_64,PreScaler_div_128
}ADC_Prescaler;

/*
 * ===========================================================================================
 * THIS STRUCTURE IS THE STRUCTURE THAT CARRY THE CHOOSEN CONFIGRATION FROM THE USER
 *
 * AND SEND IT TO INIT FUNCTION
 *
 *
 *============================================================================================
 */
typedef struct
{
	ADC_ReferenceVolatge adc_reference;
	ADC_Prescaler adc_scaler;
}ADC_ConfigType;

/*
 * INIT FUNCTION HAS TWO ARRGUMENTS
 *
 * PTR->STRUCTURE TO ACCESS THE ENTERED DATA FROM USER AND ALSO THE ADC_MODE NEEDED TO BE CONFIGURE
 */
void ADC_Init( ADC_ConfigType * Config_Ptr,Adc_Mode mode);

/*
 * THIS FUNCTION THAT READ THE CHANNEL DATA
 *
 */
#if(ADC_mode==0)

uint16 ADC_ReadChannel(uint8 channel_num);


#elif(ADC_mode==1)

void ADC_ReadChannel(uint8 channel_num);
#endif


#endif /* ADC_H_ */
