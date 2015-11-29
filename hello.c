#include <rom.h>
#include <rom_map.h>
#include <stdio.h>
#include <stdlib.h>
#include <hw_types.h>
#include <hw_memmap.h>
#include <hw_ints.h>
#include <gpio.h>
#include <pin.h>
#include <prcm.h>
#include <utils.h>
#include <timer.h>
#include <adc.h>
#include <gpio_if.h>
#include <uart_if.h>
#include "iodriver.h"


#define TIMER_LOAD 40000
#define MULTIPLY 400
#define ReadADCValue(uiPinNo) (((float)((ADCFIFORead(ADC_BASE,ADC_GetChannel(uiPinNo)) >> 2 ) & 0x0FFF))*1.4)/4096

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
volatile int g_iCounter = 0;

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************




void Led_Init ()
{
    //
    // Enable Peripheral Clocks
    //
    PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_64 for GPIO Output
    //
    PinTypeGPIO(PIN_64, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA1_BASE, 0x2, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_01 for GPIO Output
    //
    PinTypeGPIO(PIN_01, PIN_MODE_0, false);
    GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

    //
    // Configure PIN_02 for Timer PWM
    //
    PRCMPeripheralClkEnable(PRCM_TIMERA3,PRCM_RUN_MODE_CLK);
    PinTypeTimer(PIN_02,PIN_MODE_3);
    //GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);
    TimerDisable(TIMERA3_BASE,TIMER_BOTH);
    TimerConfigure(TIMERA3_BASE,TIMER_CFG_SPLIT_PAIR|TIMER_CFG_B_PWM);
    TimerControlLevel(TIMERA3_BASE,TIMER_B,1);
    TimerPrescaleSet(TIMERA3_BASE,TIMER_B,0);
    TimerLoadSet(TIMERA3_BASE,TIMER_B,TIMER_LOAD);
    TimerMatchSet(TIMERA3_BASE,TIMER_B,20000);
    TimerEnable(TIMERA3_BASE,TIMER_B);
}
//*****************************************************************************
//
//!  \brief This function sets given pin number as a PWM output
//!
//!  \param[in] uiPinNo is the pin number with PWM capability : 1,2,17,19,21,64
//!
//!	 \param[in] ucPercent is the percent of PWM Load 0-100%
//!
//!  \return none
//!
//
//*****************************************************************************
void SetPWMLoad(unsigned int uiPinNo, float fPercent)
{
	unsigned int uiTimerBase, uiTimer, uiLoadGet;
	switch (uiPinNo)
	{
			case PIN_01:
				uiTimerBase=TIMERA3_BASE;
				uiTimer=TIMER_A;
				//PWM06, TIMER3A, MODE03
			case PIN_02:
				uiTimerBase=TIMERA3_BASE;
				uiTimer=TIMER_B;
				//PWM07, TIMER3B, MODE03
			case PIN_17:
				uiTimerBase=TIMERA0_BASE;
				uiTimer=TIMER_A;
				//PWM00, TIMER0A, MODE05
			case PIN_19:
				uiTimerBase=TIMERA1_BASE;
				uiTimer=TIMER_B;
				//PWM03, TIMER1B, MODE08
			case PIN_21:
				uiTimerBase=TIMERA1_BASE;
				uiTimer=TIMER_A;
				//PWM02, TIMER1A, MODE09
			case PIN_64:
				uiTimerBase=TIMERA2_BASE;
				uiTimer=TIMER_B;
				//PWM05, TIMER2B, MODE03
	}
	if (fPercent>100)fPercent=100;
	if (fPercent<0)fPercent=0;
	uiLoadGet=TimerLoadGet(uiTimerBase,uiTimer)*fPercent/100;
	TimerMatchSet(uiTimerBase,uiTimer,uiLoadGet);
}
//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();

    //TERMINAL DEFINITIONS
    //
    // Enable Peripheral Clocks
    //
    MAP_PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);

    //
    // Configure PIN_55 for UART0 UART0_TX
    //
    MAP_PinTypeUART(PIN_55, PIN_MODE_3);

    //
    // Configure PIN_57 for UART0 UART0_RX
    //
    MAP_PinTypeUART(PIN_57, PIN_MODE_3);

}

/*
 * hello.c
 */
int main() {

	BoardInit();
	/*PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
	PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
	PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);
	PRCMPeripheralClkEnable(PRCM_GPIOA4, PRCM_RUN_MODE_CLK);*/
	GPIO_Init(PIN_01,GPIO_DIR_MODE_OUT);
	GPIO_Init(PIN_02,GPIO_DIR_MODE_OUT);
	GPIO_Init(PIN_15,GPIO_DIR_MODE_IN);
	GPIO_Init(PIN_04,GPIO_DIR_MODE_IN);
	PWM_Init(PIN_64,20000,1);
	InitTerm();
	Report("Terminal initialized \n");
	//ADC_Init(PIN_58);

    float uiLoop = 0;

	/* Led Blink */
	while(1)
	{

		TimerMatchSet(TIMERA2_BASE,TIMER_B,uiLoop);
		SetPWMLoad(PIN_64,uiLoop);
		GPIOPinWrite(GPIOA1_BASE, GPIO_PIN_2|GPIO_PIN_3, 0XFF);
		UtilsDelay(1000000);
		Report("Wartosc : %f\n",uiLoop);

		//Increase pwm time
		if(GPIO_ReadValue(PIN_15))
		{
			uiLoop = uiLoop + 0.67;
			GPIOPinWrite(GPIOA1_BASE, GPIO_PIN_2|GPIO_PIN_3, 0);
		}
		//decrease pwm time
		if(GPIO_ReadValue(PIN_04))
				{
					uiLoop = uiLoop - 0.67;
					GPIOPinWrite(GPIOA1_BASE, GPIO_PIN_2|GPIO_PIN_3, 0);
				}
		if (uiLoop >100) uiLoop=0;
		if (uiLoop <0) uiLoop=100;

		//value = GPIO_ReadValue(PIN_15);
		//Report("Wartosc : %f\n",ReadADCValue(PIN_58));
		//while (ADCFIFOLvlGet(ADC_BASE,ADC_GetChannel(PIN_58)))
		//{ADCFIFORead(ADC_BASE,ADC_GetChannel(PIN_58));}
	}
	return 0;
}



