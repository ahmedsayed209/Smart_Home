/*
 * Application.c
 *
 *  Created on: Mar 20, 2024
 *      Author: Ahmed Sayed
 */
/*============================================================================================================================= */
                                                    /* Libraries Defintions */
/*============================================================================================================================= */
#include "Lm35_Sensor.h"
#include "adc.h"
#include "uart.h"
#include "twi.h"
#include "external_eeprom.h"
#include "dcmotor.h"
#include "keypad.h"
#include "lcd.h"
#include"Gpio.h"
#include <util/delay.h>
#include "PWM.h"
#include "dcmotor.h"
#include <avr/io.h>
/*============================================================================================================================= */
                                           /* Global Definitions and some defines */
/*============================================================================================================================= */
#define Maximum_User_password 5                   /* Maximium Array Password Size */
#define Maximum_Username 8                        /* Maximium Array User_Admin Size */
#define brightness_control 230                    /* Brightness Control For The Lamp  */
uint8 Admin_User_Name[Maximum_Username];          /* Goblal Array To Used within Functions To Save UserNames of Admin  */
uint8 User_name[Maximum_Username];                /* Goblal Array To Used within Functions To Save UserNames of user */
uint8 Admin_User_Password[Maximum_User_password]; /* Goblal Array To Used within Functions To Admin Password  */
static uint8 counter=0;                           /* Globl Counter To used within functions  */
static uint8 Count=0;                             /*  Globl Counter Variable  */
static uint8 G_Flag_Login=0;                      /* Counter To Count The Login Times For Admin  */
static void Create_First_Account_Admin(uint8 state); /* Function That Respnosiable To Handle Creating The Account For Admin   */
static void INIT_ALL_FUNCTIONS(void);                /* Function To Init The All Functions   */
 uint8 ADD_USER(void);                               /* Function To Handle Adding User  */
uint8 Login_Into_System_Admin(void);                 /* Function To handle The Login Into System And Check The  */
static void User_Control_Unit(uint8 state );         /* Function To handle The Admin Control Acess */
static void Admin_Control_Uint(uint8 state );        /* Function To handle The User Control Acess */

int main(void)
{
	uint8 Byte_Recieved_Bluetooth=0;
	uint8 Valid=0;
	uint8 Keypad_Get_Button=0;
	uint8 Remove_user_counter=0;
	INIT_ALL_FUNCTIONS();
	LCD_displayStringRowColumn(0,0,"PRESS'S'");   /* System Ask Admin To Start And Press 'S */
	do
	{
		Byte_Recieved_Bluetooth=UART_RecieveByte();
	if('S'!=Byte_Recieved_Bluetooth )
	{
		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,0,"Wrong!");
		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,0,"PRESS'S'");
	}
	else
	{
		Create_First_Account_Admin(Byte_Recieved_Bluetooth);
	}

}while ('S' != Byte_Recieved_Bluetooth);

	while(Login_Into_System_Admin()) /* While This Function Returns 1 it will Be Insie an open-loop */
	{
		if(G_Flag_Login>=2) /* if The Globl_Flag More Than 2 That Means We Entered User Or Password Wrong for more than two times */
		{
			LCD_ClearScreen();
			LCD_displayStringRowColumn(0,0,"Reset System");
			_delay_ms(400);
			while(1);
		}
	}
	LCD_ClearScreen();


	for (;;)
	{
		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,2,"Enter State:");
		LCD_displayStringRowColumn(1,0,"C:Cntrl Admn,usr");
		LCD_displayStringRowColumn(2,0,"A:Add User");
		LCD_displayStringRowColumn(3,0,"R:Remove User");
		Byte_Recieved_Bluetooth=UART_RecieveByte();
		LCD_ClearScreen();
		switch(Byte_Recieved_Bluetooth)
		{
		/* System To Choose The System Conrol Admin Control ,, User Control */
		case 'C':
			LCD_displayStringRowColumn(0,0,"Choose");
			LCD_displayStringRowColumn(1,0,"A:Admin Control");
			LCD_displayStringRowColumn(2,0,"U:User Control");
			Byte_Recieved_Bluetooth=UART_RecieveByte();
			if('A' == Byte_Recieved_Bluetooth)
			{
				uint8 i =0;
				LCD_ClearScreen();
				LCD_displayStringRowColumn(0,2,"Admin:");
				while(Admin_User_Name[i] != '\0')
				{
					_delay_ms(10);
					EEPROM_readByte(0x0650+i,&(Admin_User_Name[i]));
					_delay_ms(10);
					Move_Cursor(0,8+i);
					LCD_DisplayCharchter(Admin_User_Name[i]);
					i++;
				}

				LCD_displayStringRowColumn(1,0,"L:LEDS ");
				LCD_displayStringRowColumn(1,6," H:H.Lamp");
				LCD_displayStringRowColumn(2,0,"O:O.Door");
				LCD_displayStringRowColumn(3,0,"T:check AC");
				Byte_Recieved_Bluetooth=UART_RecieveByte();
				_delay_ms(30);
				Admin_Control_Uint(Byte_Recieved_Bluetooth);

			}
			else if('U' ==Byte_Recieved_Bluetooth )
			{
				LCD_ClearScreen();
				uint8 i =0;
					LCD_ClearScreen();
					LCD_displayStringRowColumn(0,2,"User:");
					while(User_name[i] != '\0')
					{
						_delay_ms(10);
						EEPROM_readByte(0x100+i,&(User_name[i]));
						_delay_ms(10);
						Move_Cursor(0,8+i);
						LCD_DisplayCharchter(User_name[i]);
						i++;
					}
					LCD_displayStringRowColumn(1,0," 2:LEDS ");
					LCD_displayStringRowColumn(1,6," 4:H.Lamp");
					LCD_displayStringRowColumn(2,0," 3:O.Door");
					LCD_displayStringRowColumn(3,0," 5:check AC");
			    	Keypad_Get_Button=GET_PressedKey();
			    	/* Users Not Allow To Access The Door if Pressed Button Three */
			    	if(Keypad_Get_Button == 3)
			    	{
			    		LCD_ClearScreen();
			    		LCD_displayStringRowColumn(1,0,"can't Access Door");
			    		_delay_ms(500);
			    	}
			    	else
			    	{
			    		User_Control_Unit(Keypad_Get_Button);
			    	}

			}
			break;
			/* This Case For Add Users  */
		case 'A':
			 Valid=ADD_USER();
			 if(0 == Valid)
			 {
				 LCD_ClearScreen();
					LCD_displayStringRowColumn(0,0,"Exceed Max.Users");
					 _delay_ms(950);
			 }
			 else
			 {LCD_ClearScreen();
				 LCD_displayStringRowColumn(0,0,"User Added");
				 _delay_ms(950);
			 }
			 break;
			 /* This Case To Remove User From System */
		case 'R':

			 LCD_ClearScreen();
			 for (Remove_user_counter=0;Remove_user_counter<Maximum_Username;Remove_user_counter++)
			 {
				 User_name[Remove_user_counter]=0;
			 }
				LCD_displayStringRowColumn(0,0,"User Removed");
				 _delay_ms(950);
				 Count=0;


			break;
		}


	}


}

/*********************************************************************************************
 *   Function Name : Create_First_Account_Admin(uint8 State)
 *   Description   : This Function Used To Create The Admin First Account
 *   Parameters    : State: Is Just a parameter to Acess When I PRESS On 'O' Button
 *   Return Value  : Void
 *   Remarks       : None
 **********************************************************************************************/
static void Create_First_Account_Admin(uint8 state)
{
	uint8 cou=0;
	switch (state)
	{
	/*
	 * THIS CASE FOR FIRST TIME ONLYY FOR ADMIN
	 */
	case 'S':

		LCD_displayStringRowColumn(0,0,"Welcome Admin");
		_delay_ms(1000);
		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,0,"Create Ur Account");
		LCD_displayStringRowColumn(1,0,"User: ");
		LCD_displayStringRowColumn(2,0,"Password ");
		UART_ReceiveString(Admin_User_Name);
		counter=0;
		while(Admin_User_Name[cou] != '\0')
		{
			EEPROM_writeByte(0x0650+cou,Admin_User_Name[cou]);
			_delay_ms(10);
			EEPROM_readByte(0x0650+cou,&Admin_User_Name[cou]);
			_delay_ms(10);
			Move_Cursor(1,6+cou);
			LCD_DisplayCharchter(Admin_User_Name[cou]);
			cou++;
		}

		for(counter=0;counter<Maximum_User_password;counter++)
		{
			Admin_User_Password[counter]=UART_RecieveByte();
			_delay_ms(10);
			EEPROM_writeByte(0x753+counter,Admin_User_Password[counter]);
			_delay_ms(10);
			EEPROM_readByte(0x753+counter,&Admin_User_Password[counter]);
			_delay_ms(50);
			Move_Cursor(2,9+counter);
			LCD_DisplayCharchter('*');
		}
		LCD_ClearScreen();
		break;

	}
}
/*********************************************************************************************
 *   Function Name : INIT_ALL_FUNCTIONS()
 *   Description   : This Function Used To Initialzie All Functions of Modules
 *   Parameters    : void
 *   Return Value  : Void
 *   Remarks       : None
 **********************************************************************************************/
static void INIT_ALL_FUNCTIONS(void)
{
	UART_Config config={Eight_bit,Disapled,One_Bit,double_speed_Mode,Rate_9600,Pooling};
	Uart_Init(&config);
	ADC_ConfigType ADC_config={Internal_REF_Voltage,PreScaler_div_8};
	ADC_Init(&ADC_config,Pooling_ADC);
	LCD_init();
	TWI_init();
	GPIO_setupPinDirection(PORTB_ID,PIN2_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTA_ID,PIN1_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTC_ID,PIN2_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTC_ID,PIN3_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTB_ID,PIN0_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTB_ID,PIN1_ID,PIN_OUTPUT);
	Dc_Motor_Init();


}
/*********************************************************************************************
 *   Function Name : Add_User()
 *   Description   : This Function Used To Add Users To System
 *   Parameters    : void
 *   Return Value  : uint8 State To Confirm That User Successfully Added
 *   Remarks       : Note:There's only Maximum One User
 *                   If Function Returns 0 That Means We Add Maximum Users To System
 *
 **********************************************************************************************/
 uint8 ADD_USER(void)
{

	uint16 StartAddress=0X100;

	if(Count>=1)
	{
		return 0;
	}

	else
	{
	LCD_ClearScreen();
	LCD_displayStringRowColumn(0,0,"User: ");
	UART_ReceiveString(User_name);
	counter=0;
	while(User_name[counter] != '\0')
	{
		_delay_ms(20);
		EEPROM_writeByte(StartAddress+counter,User_name[counter]);
		_delay_ms(10);
		EEPROM_readByte(StartAddress+counter,&(User_name[counter]));
		Move_Cursor(0,5+counter);
		_delay_ms(150);
		LCD_DisplayCharchter(User_name[counter]);
		counter++;
	}
	Count++;

}
	return 1;
}
 /*********************************************************************************************
  *   Function Name : Login_Into_System_Admin(void)
  *   Description   : This Function Used To Login Admin To System With User_name & Password Created
  *   Parameters    : void
  *   Return Value  : uint8 State : To Indicate The State of Logging
  *   Remarks       : If System Returns 1 : That Means It's Failed To Login to be in Open Loop
  *                   If System Returns 0 : That Means It's Sucessful To Login
  **********************************************************************************************/
uint8 Login_Into_System_Admin(void)
{
	uint8 Admin_User_Name_Temp[Maximum_Username];
	uint8 flag_temp=0;
	uint8 counter_temp=0;
	uint8 pass_flag_temp=0;
	uint8 Admin_User_Password_Temp[Maximum_User_password];

	LCD_ClearScreen();
	LCD_displayStringRowColumn(0,0,"Login In");
	LCD_displayStringRowColumn(1,0,"User:");
	LCD_displayStringRowColumn(2,0,"Password");
	UART_ReceiveString(Admin_User_Name_Temp);
	counter=0;
	while(Admin_User_Name_Temp[counter] != '\0')
	{
		_delay_ms(10);
		EEPROM_readByte(0x0650+counter,&Admin_User_Name[counter]);
		_delay_ms(10);
		if(Admin_User_Name_Temp[counter]!=Admin_User_Name[counter])
		{
			flag_temp = 9;
			break;

		}
		else
		{
			flag_temp++;
		}
		Move_Cursor(1,6+counter);
		LCD_DisplayCharchter(Admin_User_Name_Temp[counter]);
		counter++;
		_delay_ms(100);
	}
	for(counter_temp=0;counter_temp<Maximum_User_password;counter_temp++)
	{
		Admin_User_Password_Temp[counter_temp]=UART_RecieveByte();
		_delay_ms(100);
		EEPROM_readByte(0x753+counter_temp,&Admin_User_Password[counter_temp]);
		_delay_ms(50);
		if(Admin_User_Password_Temp[counter_temp] != Admin_User_Password[counter_temp])
		{
			pass_flag_temp=9;
			break;
		}
		else
		{
			pass_flag_temp++;
		}
		_delay_ms(50);
		Move_Cursor(2,9+counter_temp);
		LCD_DisplayCharchter('*');
		_delay_ms(100);
	}

	if (( 9== pass_flag_temp) || ( 9==flag_temp ) )
	{
		if(2==G_Flag_Login)
		{
			GPIO_writePin(PORTB_ID,PIN2_ID,LOGIC_HIGH);
			_delay_ms(50);
			LCD_ClearScreen();
			LCD_displayStringRowColumn(1,6,"NOT ADMIN");
			_delay_ms(500);
			return 1;
		}
		LCD_ClearScreen();
		LCD_displayStringRowColumn(2,3,"Login Failed");
		_delay_ms(300);
		++G_Flag_Login;
		return 1;
	}
	else if(pass_flag_temp == counter_temp || flag_temp==counter)
	{
		GPIO_writePin(PORTB_ID,PIN2_ID,LOGIC_LOW);
		uint8 i=0;
		LCD_ClearScreen();
		LCD_displayStringRowColumn(2,3,"logging");
		for(i=0;i<3;i++)
		{
			LCD_DisplayCharchter('.');
			_delay_ms(50);
		}
		LCD_ClearScreen();
		LCD_displayStringRowColumn(2,3,"Login Successful");
		_delay_ms(800);
		G_Flag_Login=0;
		return 0;

	}

}
/*********************************************************************************************
 *   Function Name : Admin_Control_Uint(void)
 *   Description   : This Function Used To Acess The Control System
 *   Parameters    : void
 *   Return Value  : uint8 State : To Indicate The State of which Element Nedded To Be Controlled
 *   Remarks       : L : The LEDS inside The System
 *                   H : The HIGH Power Lamp of System
 *                   O : To Open The Door
 *                   T : To Check The Temperature of room and control Ac
 **********************************************************************************************/
static void Admin_Control_Uint(uint8 state )
{
	uint8 Get_Status;
	uint8 Brightness_Control=0;
	uint8 Door_Status=0;
	uint8 temp_value=0;
	LCD_ClearScreen();
	if('L'== state )
	{
		LCD_displayStringRowColumn(0,0,"e:ENABLE LEDS");
		LCD_displayStringRowColumn(1,0,"d:Disable LEDS");
		Get_Status=UART_RecieveByte();
		_delay_ms(15);
		if (Get_Status == 'e')
		{
			GPIO_writePin(PORTA_ID,PIN1_ID,LOGIC_HIGH);
			GPIO_writePin(PORTC_ID,PIN2_ID,LOGIC_HIGH);
			GPIO_writePin(PORTC_ID,PIN3_ID,LOGIC_HIGH);
			GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_HIGH);
			GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_HIGH);

		}
		else
		{
			GPIO_writePin(PORTA_ID,PIN1_ID,LOGIC_LOW);
			GPIO_writePin(PORTC_ID,PIN2_ID,LOGIC_LOW);
			GPIO_writePin(PORTC_ID,PIN3_ID,LOGIC_LOW);
			GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_LOW);
			GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_LOW);
		}

	}
	else if ('H' == state)
	{
		PWM_config configg={PWM_TIMER0,FAST_PWM,NON_INVERTED,CLK_64};
		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,0,"e:Enter Brightness");
		LCD_displayStringRowColumn(1,0,"d:Disable Lamp");
		Brightness_Control=UART_RecieveByte();
		if(Brightness_Control == 'e')
		{
		LCD_displayStringRowColumn(0,0,"EnabledHLamp");
		FAST_PWM_init(&configg,brightness_control);
		}
		else if(Brightness_Control == 'd')
		{
			LCD_displayStringRowColumn(0,0,"DisabledH.Lamp");
			FAST_PWM_init(&configg,10);
		}


	}
	else if ('O' == state)
	{

		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,0,"O:Open Door");
		LCD_displayStringRowColumn(1,0,"C:Close Door");
		Door_Status=UART_RecieveByte();
		_delay_ms(500);
		LCD_ClearScreen();
		if(Door_Status == 'o')
		{
			Servo_Motor(Door_Open_Duty);
			LCD_displayStringRowColumn(0,0,"Opening");
		}
		else
		{
			LCD_ClearScreen();
			Servo_Motor(Door_Close_Duty);
			LCD_displayStringRowColumn(0,0,"Closing");
		}
		LCD_ClearScreen();
	}
	else if ('T' == state)
	{
		LCD_displayStringRowColumn(0,0,"FAN STATUS: ");
		LCD_displayStringRowColumn(1,0,"TEMP VAL=     C");
		UART_Config config={Eight_bit,Disapled,One_Bit,double_speed_Mode,Rate_9600,Receiver_Interrupt};
		Uart_Init(&config);
		SREG|=(1<<7);
		do
		{
			temp_value=Get_Temp();
			if (temp_value <21)
			{
				Dc_MotorRotate(stopped,0);
				Move_Cursor(1,10);
				LCD_IntgerToString(temp_value);
				Move_Cursor(0,12);
				LCD_DisplayString("Off ");
			}
			else if(temp_value>=21 && temp_value <28)
			{
				Dc_MotorRotate(clockwise,128);
				Move_Cursor(1,10);
				LCD_IntgerToString(temp_value);
				Move_Cursor(0,12);
				LCD_DisplayString("ON ");
			}
			else if(temp_value >=28 )
			{
				Dc_MotorRotate(clockwise,250);
				Move_Cursor(1,10);
				LCD_IntgerToString(temp_value);
				Move_Cursor(0,12);
				LCD_DisplayString("ON ");
			}
			_delay_ms(500);
		}
		while('C'!=UART_RecieveByte());
	}
}
/*********************************************************************************************
 *   Function Name : Admin_Control_Uint(void)
 *   Description   : This Function Used To Acess The Control System
 *   Parameters    : void
 *   Return Value  : uint8 State : To Indicate The State of which Element Nedded To Be Controlled
 *   Remarks       : 1 : Enable LEDS inside The System
 *                   0: Disable LEDs inside system
 *                   4 : To Control Hih Power Lamp
 *                   5 : To Check The Temperature of room and control Ac
 *           Important Noe : Users Not Able To Access The Door .. This Condition Handeled In App For loop
 **********************************************************************************************/
static void User_Control_Unit(uint8 state )
{
	uint8 Get_Status;
	uint8 Brightness_Control=0;
	uint8 temp_value=0;
	LCD_ClearScreen();
	if( 2 == state )
	{
		LCD_displayStringRowColumn(0,0,"1:ENABLE LEDS");
		LCD_displayStringRowColumn(1,0,"0:Disable LEDS");
		Get_Status=GET_PressedKey();
		_delay_ms(15);
		if (Get_Status == 1)
		{
			GPIO_writePin(PORTA_ID,PIN1_ID,LOGIC_HIGH);
			GPIO_writePin(PORTC_ID,PIN2_ID,LOGIC_HIGH);
			GPIO_writePin(PORTC_ID,PIN3_ID,LOGIC_HIGH);
			GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_HIGH);
			GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_HIGH);

		}
		else if(Get_Status == 0)
		{
			GPIO_writePin(PORTA_ID,PIN1_ID,LOGIC_LOW);
			GPIO_writePin(PORTC_ID,PIN2_ID,LOGIC_LOW);
			GPIO_writePin(PORTC_ID,PIN3_ID,LOGIC_LOW);
			GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_LOW);
			GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_LOW);
		}

	}
	else if (4 == state )
	{	PWM_config configg={PWM_TIMER0,FAST_PWM,NON_INVERTED,CLK_64};

		LCD_ClearScreen();
		LCD_displayStringRowColumn(0,0,"1:enable ");
		LCD_displayStringRowColumn(1,0,"0:Disable ");
		_delay_ms(70);
		Brightness_Control=GET_PressedKey();
		if (1==Brightness_Control)
		{
			LCD_displayStringRowColumn(0,0,"EnabledHLamp");
			FAST_PWM_init(&configg,brightness_control);


		}
		else if (0==Brightness_Control)
		{
			LCD_displayStringRowColumn(0,0,"DisabledHLamp");
			FAST_PWM_init(&configg,10);
		}


	}
	else if (5 == state)
	{
		LCD_displayStringRowColumn(0,0,"FAN STATUS: ");
		LCD_displayStringRowColumn(1,0,"TEMP VAL=     C");
		do
		{
			temp_value=Get_Temp();
			if (temp_value <21)
			{
				Dc_MotorRotate(stopped,0);
				Move_Cursor(1,10);
				LCD_IntgerToString(temp_value);
				Move_Cursor(0,12);
				LCD_DisplayString("Off ");
			}
			else if(temp_value>=21 && temp_value <28)
			{
				Dc_MotorRotate(clockwise,128);
				Move_Cursor(1,10);
				LCD_IntgerToString(temp_value);
				Move_Cursor(0,12);
				LCD_DisplayString("ON ");
			}
			else if(temp_value >=28 )
			{
				Dc_MotorRotate(clockwise,250);
				Move_Cursor(1,10);
				LCD_IntgerToString(temp_value);
				Move_Cursor(0,12);
				LCD_DisplayString("ON ");
			}
			_delay_ms(500);
		}
		while('*'!=GET_PressedKey());
	}

}
