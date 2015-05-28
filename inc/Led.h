/*
 * Led.h
 *
 *  Created on: May 28, 2015
 *      Author: tom
 */

#ifndef LED_H_
#define LED_H_

typedef enum
{
	LED3 = 0,
	LED4
} Led;

void LedInit( void );
void LedOn( Led led );
void LedOff( Led led );
void LedToggle( Led led );

#endif /* LED_H_ */
