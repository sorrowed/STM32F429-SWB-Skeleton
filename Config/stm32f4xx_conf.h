/*
 * stm32f4xx_conf.h
 *
 *  Created on: May 20, 2015
 *      Author: tom
 */

#ifndef STM32F4XX_CONF_H_
#define STM32F4XX_CONF_H_

#undef HSE_VALUE
#define HSE_VALUE	8

/* Configure PLL values. To do this here i had to edit system_stm32f4xx.c too because these values are redefined there */
#define PLL_M		8
#define PLL_N		336
#define PLL_P		2
#define PLL_Q		7

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define assert_param(a)

#endif /* STM32F4XX_CONF_H_ */
