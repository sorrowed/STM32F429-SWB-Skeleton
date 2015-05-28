/*
 * main.c
 *
 *  Created on: May 20, 2015
 *      Author: tom
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"
#include "stdio.h"

#include "TaskConfig.h"
#include "Led.h"

static xTaskHandle		TskMainHandle;
static xTaskHandle		TskBlinkHandle;

static void TskMain( void * parms );
static void TskBlinkGreen( void * parms );
static void TskBlinkRed( void * parms );

int main( void )
{
	LedInit();

	xTaskCreate( TskMain, "Main", TskMainStack, NULL,
			TskMainPrio, &TskMainHandle );
	xTaskCreate( TskBlinkGreen, "BlinkGreen", TskBlinkStack, NULL,
			TskBlinkPrio, &TskBlinkHandle );
	xTaskCreate( TskBlinkRed, "BlinkRed", TskBlinkStack, NULL,
			TskBlinkPrio, &TskBlinkHandle );

	vTaskStartScheduler();

	return -1;
}

volatile int seconds = 0;

void TskMain( void * parms )
{
	while( true )
	{
		vTaskDelay( 1000 / portTICK_RATE_MS );

		++seconds;

		printf( "%i\n", seconds );
	}
}

void TskBlinkGreen( void * parms )
{
	while( true )
	{
		vTaskDelay( 500 / portTICK_RATE_MS );
		LedToggle( LED3 );
	}
}

void TskBlinkRed( void * parms )
{
	while( true )
	{
		vTaskDelay( 250 / portTICK_RATE_MS );
		LedToggle( LED4 );
	}
}

void vApplicationMallocFailedHook( void )
{
	while( true )
		;
}
