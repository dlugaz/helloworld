/*
 * iodriver.h
 *
 *  Created on: 27 lis 2015
 *      Author: kdluzynski
 */

#ifndef IODRIVER_H_
#define IODRIVER_H_


//*****************************************************************************
//
// API Function prototypes
//
//*****************************************************************************
extern unsigned char GPIO_ReadValue(unsigned long ulPinNo);
extern void GPIO_Init(unsigned long ulPinNo,unsigned char ucDirection);
extern void GPIO_WriteValue(unsigned long ulPinNo,unsigned char ucValue );
extern unsigned int ADC_GetChannel(unsigned int uiPinNo);
extern void ADC_Init(unsigned int uiPinNo);
extern void PWM_Init(unsigned int uiPinNo, unsigned int uiPeriod , unsigned char ucInvert);

#endif /* IODRIVER_H_ */
