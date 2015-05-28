/*
 * Led.c
 *
 *  Created on: May 28, 2015
 *      Author: tom
 */

#include "Led.h"
#include "stm32f4xx.h"

#define LED3_PIN	GPIO_Pin_13
#define LED4_PIN	GPIO_Pin_14

void LedInit( void )
{

	GPIO_InitTypeDef GpioCfg;

	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG, ENABLE );

	GpioCfg.GPIO_Pin = LED3_PIN | LED4_PIN;
	GpioCfg.GPIO_Mode = GPIO_Mode_OUT;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_UP;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOG, &GpioCfg );
}

void LedOn( Led led )
{
	uint16_t pin = led == LED3 ? LED3_PIN : LED4_PIN;

	GPIOG->BSRRL = pin;
}

void LedOff( Led led )
{
	uint16_t pin = led == LED3 ? LED3_PIN : LED4_PIN;

	GPIOG->BSRRH = pin;
}

void LedToggle( Led led )
{
	uint16_t pin = led == LED3 ? LED3_PIN : LED4_PIN;

	GPIOG->ODR ^= pin;
}
