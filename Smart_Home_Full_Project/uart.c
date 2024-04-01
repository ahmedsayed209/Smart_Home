/*
 * uart.c
 *      Author: Ahmed Sayed
 */

#include "uart.h"
#include <avr/io.h>
#include "common_macros.h"
#include <avr/interrupt.h>
/*===========================================================================================================================
 *               Global Variables Definitions: That Used To Read The Data From UDR Register With Interrupt Mode
 *    static + volatile To Avoid Extern Them In another files,, and to avoid any compiler Optimization
 *===========================================================================================================================
 */
static volatile uint8 Transsmision_data_Glo=0;
static volatile  uint8 Reciever_data_glo=0;
/* ==========================================================================================================================
 *                           ISRs That Used Only To Read or Transmit Data From UDR Register In Interrupt Mode
 * ==========================================================================================================================
 */
ISR(USART_TXC_vect)
{
   UDR=Transsmision_data_Glo; /* When Interrupt Is Fired Transmit ALL data In UDR REGISTER  */
}
ISR(USART_RXC_vect)
{
	Reciever_data_glo=UDR; /* When Interrupt is Fired Read All Data from UDR register   */
}
/*======================================================================================================================
 *                    UART_Init Function: Responsible To Enable and Configure The UART PREPHERAL
 * =====================================================================================================================
 */
void Uart_Init(const UART_Config *config )
{
	uint8 mode; /* Mode Variable is Read The Mode of UART To Detect Which Mode will work [Interrupt , Pooling ] */
	uint32 Baud_Rate=0; /* Baud_Rate variable To Read The Const_Rate of UART */
	uint16 UBRR_value=0; /* UBRR_value To put The Result of Baud and Frequency In UBRR REGISTER */
	mode = config->UART_Mode; /* Mode Variable To Read Which Mode was Choose From User and Switch On it  */
	switch(mode)
	{
	case Pooling: /* Case Pooling There's No Registers To be Enabled  BUT Disable The Interrupt Enable   */
		CLEAR_BIT(UCSRB,TXCIE);
		CLEAR_BIT(UCSRB,RXCIE);
		break;
	case Interrupt_both: /* For Both Interrupt Mode : We Enable RXCIE AND TXCIE  */
		SET_BIT(UCSRB,RXCIE);
		SET_BIT(UCSRB,TXCIE);
		break;
	case Transmission_Interrupt: /* for Transmission only will enable TXCIE   */
		SET_BIT(UCSRB,TXCIE);
		break;
	case Receiver_Interrupt: /* for Receive Only Will Enable RXCIE  */
		SET_BIT(UCSRB,RXCIE);
		break;
	}

	UCSRC = (1<<URSEL); /* Enable URSEL To Be Enable The Register UCSRC and to be able to write in it */

	/*===============================================================================
	 * Configure The Frame_bits of UART AND Configure The BIT POSITION IN REGISTER =
	 * FRAME_BITS : 5 BIT MODE , 6 BIT MODE , 7 BIT MODE , 8 BIT MODE , 9 BIT MODE =
	 *==============================================================================
	 */
	UCSRC=(UCSRC&0xF9) | (config->FrameBits<<1);

	if (config->FrameBits==Nine_bit)
	{
		UCSRB=(UCSRB&0XFB) | (config->FrameBits&0x04); /* Check if The User Choose 9-bit mode Will need UCSRB Register To put UCZ2 =1  */
	}

	UCSRC=(UCSRC & 0XCF) | (config->Parity<<4); /* Configure The Parity_bit Mode */

	UCSRC=(UCSRC & 0xF7) | (config->Stop<<3);  /* Configure The Stop_bit Mode */

	UCSRA=(UCSRA&0XFD)   | (config->Bud_Rate<<1); /* Configure The Baud_Rate Mode */

	Baud_Rate=config->Baud_RATE_Const; /* Read The Baud Rate Chosen from user if UART send and receive with 9600 or 11400 etc... */

	if(config->Bud_Rate==Normal_Mode)  /*CHECK IF U2X IN UCSRA IS = 0 ? WILL BE TRANSMIT AND RECIEVD WITH NORMAL MODE  */
	{
		UBRR_value=(uint16) (((F_CPU/(Baud_Rate * 16UL)))-1);
	}
	else if(config->Bud_Rate==double_speed_Mode) /*CHECK IF U2X IN UCSRA IS = 1? WILL BE TRANSMIT AND RECIEVD WITH Speed MODE  */
	{
		UBRR_value=(uint16) (((F_CPU/(Baud_Rate * 8UL)))-1);
	}

	UBRRH = UBRR_value>>8; /* As UBBRH IS 12-BIT So UBRR_Value Will be divided into two Registers  */
	UBRRL = UBRR_value;
	UCSRB=(1<<TXEN) | (1<<RXEN ); /* Enable The Peripheral BITS to Use UART IN ATMEGA32  */
}

/* ===============================================================================================================================
 *                                           UART_SendBYTE : Function To Send a Byte
 *                                           This Function Supports Two Modes [Pooling ,Interrupt]
 * ===============================================================================================================================
 */
void UART_SendByte(const uint8 data )
{
	uint8 mode=0; /*Variable To Check On The Last Mode is added To structure in Member: UART_MODE */
	UART_Config config; /* Variable From Data Type of UART_CONFIG  */
	mode =config.UART_Mode; /* Assign The Last Mode Added in structure */
	if (mode==Transmission_Interrupt || mode==Interrupt_both) /* Check if The mode Is Interrupt Or Not  */
	{
		/*
		 * IF Mode Carry Interrupt Mode:
		 * So Put Data In the Global Variable First Before Enable The TXCIE bit
		 * When INTERRUPT IS FIRED I WILL BE know That Data Is In Global Variable
		 */
		Transsmision_data_Glo=data;
		SET_BIT(UCSRB,TXCIE); /* Enable Transmission Interrupt Enable */
	}
	else
	{ /* If Non-of Both Interrupt Modes Choose From User ( Pooling Mode Will work ) */
	UDR=data; /* Transfer Data To UDR Register */
	while(BIT_IS_CLEAR(UCSRA,TXC)); /* Wait This Flag To raise ONE to Be Known That All Data Transmitted From UDR */
	SET_BIT(UCSRA,TXC); /* Clear Flag  */
}
}
uint8 UART_RecieveByte(void)
{
	uint8 mode=0; /*Variable To Check On The Last Mode is added To structure in Member: UART_MODE */
	UART_Config config;  /* Variable From Data Type of UART_CONFIG  */
	mode =config.UART_Mode; /* Assign The Last Mode Added in structure */
	if (mode==Receiver_Interrupt || mode ==Interrupt_both) /* Check if The mode Is Interrupt Or Not  */
	{
		SET_BIT(UCSRB,RXCIE);  /* Enable Receiver Interrupt Enable */
		/*
		 * Check If The Global Variable not equal -> zero
		 * So It Indicate That ISR Is reached and all data was carried from UDR To this Variable
		 */
		if (Reciever_data_glo!=0)
		{
			return (Reciever_data_glo); /* Return The Global Variable  */
		}
		Reciever_data_glo=0; /* Reset The Global Variable to check in the another time */
	}
	else
	{
		while (BIT_IS_CLEAR(UCSRA,RXC)); /* Wait This Flag To raise ONE to Be Known That All Data Received From UDR */
		return (UDR); /*RETURN UDR REGISTER ,, THE FLAG IS RESET AUTOMATIC ONCE THE REGISTER IS READ  */
	}
}
/*===========================================================================================================================
 *
 * UART_SendString : Function That Send a String Using UART
 * ARGUMENTS: A Pointer That Will carry The address of array of Charchters
 * This  Argument is const to avoid any change of this data
 *
 * ===========================================================================================================================
 */
void  UART_SendString(const uint8 *str)
{
	uint8 i=0; /* Increment variable i=0 */

	while(str[i] != '\0' )
	{
		UART_SendByte(str[i]); /* Looping and send Byte - byte using sendbyte function till '\0' */
		++i;
	}
}
/*==============================================================================================================================
 *
 *    UART_RecieveString : Function That Receive a String Using UART
 *    ARGUMENTS: A Pointer That Will carry The address of array of Charchters
 *    This  Argument is non const. because here we will need to modify inside it
 *
 * =============================================================================================================================
 */
void  UART_ReceiveString(uint8 *str)
{
	uint8 i=0; /* Increment variable i=0 */
	str[i]=UART_RecieveByte(); /* Ready To Receive The First-byte and save in first place in array  */

	/*===================================================
	 *  The '$' is Symbol Is The Indicator That The String Is Totally Received  From Transmitter We Will Receive Till This Symbol
	 *
	 *  This Symbol is Defined Between The Two Prtotcols To be Indicator
	 *
	 */
	while(str[i] != '*')
	{
		i++; /* Increment To The Second Element in array  */
		str[i]=UART_RecieveByte(); /*Receive The Second Data */
	}
	str[i]='\0'; /* After Finishing The ALL Data Will replace The Protocol Symbol into NULL '\0' To be Used In another functions
	as normal String   */
}
/*=============================================================================================================
 * UART_mode : Function That Can CHoose And Change The Mode Dynamically within code
 *
 * This Function has a ptr to the config structure and point the UART mode and replace The New Mode inside it within function
 *
 * and when call the send or receive It will Check EveryTime About Mode inside Every Function
 *
 * ============================================================================================================
 */
void UART_mode( UART_Config *ptr,uint8 mode)
{
	ptr->UART_Mode=mode;
}
/*
 * UART_Deinit: Function To De-initialize The UART and stop this Peripheral
 */

void UART_Deint(void)
{
	UCSRC=0;
	UCSRB=0;
	UCSRA=0;
	Transsmision_data_Glo=0;
	Reciever_data_glo=0;
}
