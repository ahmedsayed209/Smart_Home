/*
 * uart.h
 *
 *
 *      Author: Ahmed Sayed
 */

#ifndef UART_H_
#define UART_H_
#include "std_types.h"
/*=====================================================================================
 * Enum: Detect All Modes For UART Protocol
 * -> Pooling On TXC AND RXC
 * -> INTERRUPT ON BOTH TRANSITER AND RECIEVER
 * ->  UART WITH TRANSIMMSION ONLY INTERRUPT
 * -> UART WITH RECIEVE ONLY INTERRUPT
 * =====================================================================================
 */
typedef enum
{
    Pooling,Interrupt_both,Transmission_Interrupt,Receiver_Interrupt
}UART_Work_Mode;
/*=====================================================================================================
 * Enum: To Choose The Frame Data Bits
 * UART WITH AVR ATMEGA 32 HAVE
 * FIVE_BITS MODE
 * SIX_BITS MODE
 * SEVEN_BIT
 * EIGHT_BIT-> MOST COMMON USED
 * NINE_BIT -> NOT USED
 * ======================================================================================================
 */
typedef enum
{
	Five_Bit,Six_bit,Seven_bit,Eight_bit,Nine_bit=0x07
}Charchter_Size_Config;
/*=================================================================================================================
 * Enum: To Choose The Bits Possible For Parity Bits                                                              =
 * UART AT ATMEGA 32                                                                                              =
 * HAVE EVEN PARITY AND ODD PARITY MODE                                                                           =
 *                                                                                                                =
 * =================================================================================================================
 */
typedef enum
{
	Disapled,Even_Parity=0x02,Odd_Parity
}Parity_Bits;
/*=================================================================================================================
 * Enum: To Choose The Bits Possible For Stop bits                                                                =
 * UART AT ATMEGA 32                                                                                              =
 * HAVE One Stop bit or two bits mode                                                                             =
 * Most Used Is One_bit                                                                                           =
 *                                                                                                                =
 * =================================================================================================================
 */
typedef enum
{
	One_Bit,Two_Bits
}Stop_Bits;
/*=================================================================================================================
 * Enum: To Choose The Speed Mode of UART                                                                         =
 * UART AT ATMEGA 32                                                                                              =
 *  U2X BIT IN UCSRA =1 -> DOUBLE SPEED MDOE                                                                      =
 *   U2X BIT IN UCSRA =0 -> Normal SPEED MDOE                                                                                                             =
 * =================================================================================================================
 */
typedef enum
{
	Normal_Mode,double_speed_Mode
}Baud_Rate_Mode;
/*=================================================================================================================
 *                                  Enum: To Choose The Speed Rate of UART                                                                      =                                                                                                            =
 *=================================================================================================================
 */
typedef enum
{
	Rate_10=10,Rate_300=300,Rate_600=600,Rate_1200=1200,Rate_2400=2400,Rate_4800=4800,Rate_9600=9600,Rate_14400=14400,Rate_19200=19200,Rate_38400=38400,Rate_57600=57600,Rate_115200=115200,Rate_128000=128000,Rate_256000=256000
}Baud_Rate_Const;
/*
 * Structure:for Configuration of Init Function To Configure The Init Functions Dynamically within code
 */
typedef struct
{
	Charchter_Size_Config FrameBits;
	Parity_Bits Parity;
	Stop_Bits Stop;
	Baud_Rate_Mode  Bud_Rate;
	Baud_Rate_Const Baud_RATE_Const;
	UART_Work_Mode UART_Mode;
}UART_Config;
/*=====================================================================================================================
 *                                            Proto-Types of functions
 * ====================================================================================================================
 */
void Uart_Init(const UART_Config *config );
void UART_SendByte(const uint8 data );
uint8 UART_RecieveByte(void);
void  UART_SendString(const uint8 *str);
void  UART_ReceiveString(uint8 *str);
void UART_mode( UART_Config *ptr,uint8 mode);
void UART_Deint(void);
#endif /* UART_H_ */
