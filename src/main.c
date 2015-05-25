/*
 * main.c
 *
 *  Created on: May 20, 2015
 *      Author: tom
 */

#include "FreeRTOS.h"
#include "task.h"
#include "stdbool.h"

#define Main_Task_PRIO    (tskIDLE_PRIORITY + 1)
#define Main_Task_STACK   (configMINIMAL_STACK_SIZE)

static xTaskHandle Task_Handle;
static void Main_Task( void * parms );

int main( void )
{
	xTaskCreate( Main_Task, (signed char const* )"Main", Main_Task_STACK, NULL,
			Main_Task_PRIO, &Task_Handle );

	vTaskStartScheduler();

	return -1;
}

volatile int seconds = 0;

void Main_Task( void * parms )
{
	while( true )
	{
		vTaskDelay( 1000 / portTICK_RATE_MS );

		++seconds;

		printf( "%i\n", seconds );
	}
}

void vApplicationMallocFailedHook( void )
{
	while( true )
		;
}
