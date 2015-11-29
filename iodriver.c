/*
 * iodriver.c
 *
 *  Created on: 27 lis 2015
 *      Author: kdluzynski
 */
#include "iodriver.h"
#include "rom.h"
#include "rom_map.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "adc.h"
#include "timer.h"

//*****************************************************************************
// PIN to PAD matrix
//*****************************************************************************

	static const unsigned char g_ucPinMap[64]=
	{
		10,11,12,13,14,15,16,17,255,255,18,
	19,20,21,22,23,24,40,28,29,25,255,
	255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,
	31,255,255,255,255,0,255,32,30,255,1,
	255,2,3,4,5,6,7,8,9
	};

	static const unsigned long g_ulPortReg[]=
	{
	    GPIOA0_BASE,
	    GPIOA1_BASE,
	    GPIOA2_BASE,
	    GPIOA3_BASE,
	    GPIOA4_BASE
	};

	//*****************************************************************************
	//
	//!  \brief This function sets given pin number as a GPIO input/output
	//!
	//!  \param[in] uiPinNo is the pin number with GPIO capability : 1-8,15,17,20,21,28,45,50,52-55,57-64
	//!
	//!	 \param[in] ucDirection is used to select if pin is output (1) or input (0)
	//!
	//!  \return none
	//!
	//
	//*****************************************************************************
	void GPIO_Init(unsigned long ulPinNo,unsigned char ucDirection)
	{
		unsigned long ulPort;
		unsigned char ucGPIOPin;
	    //
	    // Get the GPIO pin from the external Pin number
	    //
	    ucGPIOPin = 1 << (g_ucPinMap[ulPinNo] % 8);

	    //
	    // Get the GPIO port from the external Pin number
	    //
	    ulPort = (g_ucPinMap[ulPinNo] / 8);
	    ulPort = g_ulPortReg[ulPort];
		//GPIO_IF_GetPortNPin(g_ucPinMap[ulPinNo],&uiPort,&ucGPIOPin);
		switch(ulPort)
		{
		case GPIOA0_BASE:
			PRCMPeripheralClkEnable(PRCM_GPIOA0,PRCM_RUN_MODE_CLK);
		case GPIOA1_BASE:
			PRCMPeripheralClkEnable(PRCM_GPIOA1,PRCM_RUN_MODE_CLK);
		case GPIOA2_BASE:
			PRCMPeripheralClkEnable(PRCM_GPIOA2,PRCM_RUN_MODE_CLK);
		case GPIOA3_BASE:
			PRCMPeripheralClkEnable(PRCM_GPIOA3,PRCM_RUN_MODE_CLK);
		case GPIOA4_BASE:
			PRCMPeripheralClkEnable(PRCM_GPIOA4,PRCM_RUN_MODE_CLK);
		}
		PinTypeGPIO(ulPinNo,PIN_MODE_0,false);
		GPIODirModeSet(ulPort,ucGPIOPin,ucDirection);

	}

	//*****************************************************************************
	//
	//!  \brief This function reads the binary state of given GPIO Pin
	//!
	//!  \param[in] ulPinNo is the pin number with GPIO capability : 1-8,15,17,20,21,28,45,50,52-55,57-64
	//!
	//!  \return Binary state of the pin
	//!
	//
	//*****************************************************************************
	unsigned char GPIO_ReadValue(unsigned long ulPinNo)
	{

		unsigned int uiPort;
		unsigned char ucGPIOPin;
		unsigned char ucValue = 0;

		//
	    // Get the GPIO pin from the external Pin number
	    //
	    ucGPIOPin = 1 << (g_ucPinMap[ulPinNo] % 8);

	    //
	    // Get the GPIO port from the external Pin number
	    //
	    uiPort = (g_ucPinMap[ulPinNo] / 8);
	    uiPort = g_ulPortReg[uiPort];

	    //
	    // Set the corresponding bit in the bitmask
	    //
	    ucValue = (MAP_GPIOPinRead(uiPort,ucGPIOPin)) >> (ucGPIOPin % 8);
		//ucValue = GPIO_IF_Get(g_ucPinMap[ulPinNo],uiPort,ucGPIOPin);
		return ucValue;

	}

	//*****************************************************************************
		//
		//!  \brief This function writes the binary state of given GPIO Pin
		//!
		//!  \param[in] ulPinNo is the pin number with GPIO capability : 1-8,15,17,20,21,28,45,50,52-55,57-64
		//!
		//!
		//!  \param[in] ucValue is the binary value for GPIO 0/1
		//!
		//!  \return Binary state of the pin
		//!
		//
		//*****************************************************************************
		void GPIO_WriteValue(unsigned long ulPinNo,unsigned char ucValue )
		{
			unsigned int uiPort;
			unsigned char ucGPIOPin;
		    //
		    // Get the GPIO pin from the external Pin number
		    //
		    ucGPIOPin = 1 << (g_ucPinMap[ulPinNo] % 8);

		    //
		    // Get the GPIO port from the external Pin number
		    //
		    uiPort = (g_ucPinMap[ulPinNo] / 8);
		    uiPort = g_ulPortReg[uiPort];

		    //
		    // Set the corresponding bit in the bitmask
		    //
		    ucValue = ucValue << (ucGPIOPin % 8);

		    //
		    // Invoke the API to set the value
		    //
		    MAP_GPIOPinWrite(uiPort,ucGPIOPin,ucValue);

		}

		//*****************************************************************************
		//
		//!  \brief This function sets given pin number as a ADC input
		//!
		//!  \param[in] uiPinNo is the address to pin with ADC ability
		//!             PIN_57, PIN_58, PIN_59, PIN_60.
		//!
		//!  \return none
		//!
		//
		//*****************************************************************************
		void ADC_Init(unsigned int uiPinNo)
		{
			unsigned int uiChannel;
			//
			//Switch pin mode to ADC for given pin
			//
			PinTypeADC(uiPinNo,PIN_MODE_255);
			//
			//Set & enable timer for optional time stamping
			//
			ADCTimerConfig(ADC_BASE,2^17);
			ADCTimerEnable(ADC_BASE);
			//
			// Globally enable ADC
			//
			ADCEnable(ADC_BASE);
		    //
		    // Convert pin number to channel number
		    //
			uiChannel=ADC_GetChannel(uiPinNo);

		    ADCChannelEnable(ADC_BASE,uiChannel);

		}



	unsigned int ADC_GetChannel(unsigned int uiPinNo)
	{
		switch(uiPinNo)
		{
			case PIN_57:
				return ADC_CH_0;

			case PIN_58:
				return ADC_CH_1;

			case PIN_59:
				return ADC_CH_2;

			case PIN_60:
				return ADC_CH_3;

			default:
				break;
		}
		return 0;
	}


	//*****************************************************************************
	//
	//!  \brief This function sets given pin number as a PWM output
	//!
	//!  \param[in] uiPinNo is the pin number with PWM capability : 1,2,17,19,21,64
	//!
	//!	 \param[in] uiPeriod is the period of PWM in microseconds [us]
	//!
	//!	 \param[in] ucInvert Sets if PWM counts up (0) or down (1)
	//!
	//!  \return none
	//!
	//
	//*****************************************************************************
	void PWM_Init(unsigned int uiPinNo, unsigned int uiPeriod , unsigned char ucInvert)
	{
		unsigned int uiPRCMTimer, uiPinMode, uiTimerBase, uiTimer, uiLoadSet;
		unsigned char ucPrescaler;
		switch (uiPinNo)
		{
				case PIN_01:
					uiPRCMTimer=PRCM_TIMERA3;
					uiTimerBase=TIMERA3_BASE;
					uiPinMode=PIN_MODE_3;
					uiTimer=TIMER_A;
					//PWM06, TIMER3A, MODE03
				case PIN_02:
					uiPRCMTimer=PRCM_TIMERA3;
					uiTimerBase=TIMERA3_BASE;
					uiPinMode=PIN_MODE_3;
					uiTimer=TIMER_B;
					//PWM07, TIMER3B, MODE03
				case PIN_17:
					uiPRCMTimer=PRCM_TIMERA0;
					uiTimerBase=TIMERA0_BASE;
					uiPinMode=PIN_MODE_5;
					uiTimer=TIMER_A;
					//PWM00, TIMER0A, MODE05
				case PIN_19:
					uiPRCMTimer=PRCM_TIMERA1;
					uiTimerBase=TIMERA1_BASE;
					uiPinMode=PIN_MODE_8;
					uiTimer=TIMER_B;
					//PWM03, TIMER1B, MODE08
				case PIN_21:
					uiPRCMTimer=PRCM_TIMERA1;
					uiTimerBase=TIMERA1_BASE;
					uiPinMode=PIN_MODE_9;
					uiTimer=TIMER_A;
					//PWM02, TIMER1A, MODE09
				case PIN_64:
					uiPRCMTimer=PRCM_TIMERA2;
					uiTimerBase=TIMERA2_BASE;
					uiPinMode=PIN_MODE_3;
					uiTimer=TIMER_B;
					//PWM05, TIMER2B, MODE03
		}

		//Enable Clocks
	    PRCMPeripheralClkEnable(uiPRCMTimer,PRCM_RUN_MODE_CLK);
		//Set Pin as Timer
	    PinTypeTimer(uiPinNo,uiPinMode);
	    //Configure Timer which belongs to configurated pin
	    TimerConfigure(uiTimerBase,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
	    TimerControlLevel(uiTimerBase,uiTimer,ucInvert);
	    //Calculate Best Prescaler for given period
	    ucPrescaler=uiPeriod*80/65535+1;
	    TimerPrescaleSet(uiTimerBase,uiTimer,ucPrescaler);
	    //Calculate LoadSet from period in us
	    uiLoadSet=uiPeriod*80/ucPrescaler;
	    TimerLoadSet(uiTimerBase,uiTimer,uiLoadSet);
	    //Initiate timer Match value with Load Set = 0% load
	    TimerMatchSet(uiTimerBase,uiTimer,uiLoadSet);
	    TimerEnable(uiTimerBase,uiTimer);

	}
