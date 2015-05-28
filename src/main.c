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
#include "string.h"

#include "TaskConfig.h"
#include "Led.h"
#include "SdRam.h"

static xTaskHandle		TskMainHandle;
static xTaskHandle		TskBlinkHandle;
static xTaskHandle		TskSdRamHandle;

static void TskMain( void * parms );
static void TskBlinkGreen( void * parms );
static void TskBlinkRed( void * parms );
static void TskSdRam( void * parms );

int main( void )
{
	LedInit();
	SdRamInit();

	xTaskCreate( TskMain, TskMainDesc, TskMainStack, NULL,
			TskMainPrio, &TskMainHandle );
	xTaskCreate( TskBlinkGreen, TskBlinkDesc, TskBlinkStack, NULL,
			TskBlinkPrio, &TskBlinkHandle );
	xTaskCreate( TskBlinkRed, TskBlinkDesc, TskBlinkStack, NULL,
			TskBlinkPrio, &TskBlinkHandle );
	xTaskCreate( TskSdRam, TskSdRamDesc, TskSdRamStack, NULL,
			TskSdRamPrio, &TskSdRamHandle );

	vTaskStartScheduler();

	return -1;
}

void TskMain( void * parms )
{
	int seconds = 0;

	while( true )
	{
		vTaskDelay( 1000 / portTICK_RATE_MS );

		++seconds;

		printf( "%i\n", seconds );
	}
}

static uint32_t buffer[ 1025 ];

void TskSdRam( void * parms )
{
	RCC_AHB2PeriphClockCmd( RCC_AHB2Periph_RNG, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_CRC, ENABLE );
	RNG_Cmd( ENABLE );

	while( true )
	{
		vTaskDelay( 250 / portTICK_RATE_MS );

		for( int i = 0; i < 1024; ++i )
		{
			while( RNG_GetFlagStatus( RNG_FLAG_DRDY ) == RESET );

			buffer[ i ] = RNG_GetRandomNumber();
		}

		CRC_ResetDR();
		CRC_CalcBlockCRC( buffer, 1024 );
		buffer[ 1024 ] = CRC_GetCRC();

		SdRamWrite( buffer, 0, 1025 );

		memset( buffer, 0, sizeof buffer );

		SdRamRead( buffer, 0, 1025 );
		CRC_ResetDR();
		CRC_CalcBlockCRC( buffer, 1024 );
		volatile uint32_t crc = CRC_GetCRC();
		if( buffer[ 1024 ] != crc )
			;
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
