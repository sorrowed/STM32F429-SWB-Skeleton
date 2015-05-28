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
#include "Lcd.h"

static xTaskHandle TskMainHandle;
static xTaskHandle TskBlinkHandle;
static xTaskHandle TskSdRamHandle;

static void TskMain( void * parms );
static void TskBlink( void * parms );
static void TskSdRam( void * parms );

int main( void )
{
	LedInit();
	SdRamInit();

	LcdInit();

	LcdSetLayer( LCD_FOREGROUND_LAYER );
	LcdClear( LCD_COLOR_BLUE );
	LcdSetBackColor( LCD_COLOR_BLUE );
	LcdSetTextColor( LCD_COLOR_WHITE );

	xTaskCreate( TskMain, TskMainDesc, TskMainStack, NULL, TskMainPrio,
			&TskMainHandle );
	xTaskCreate( TskBlink, TskBlinkDesc, TskBlinkStack, NULL, TskBlinkPrio,
			&TskBlinkHandle );
	xTaskCreate( TskSdRam, TskSdRamDesc, TskSdRamStack, NULL, TskSdRamPrio,
			&TskSdRamHandle );

	vTaskStartScheduler();

	return -1;
}

void AlignCenter( char* buffer, int maxlen, const char* string )
{
	int len = strlen( string );
	if( len > maxlen )
		len = maxlen;

	int margin = ( maxlen - len ) / 2;

	memset( buffer, ' ', margin );
	strncpy( buffer + margin, string, len );
	memset( buffer + margin + len, ' ', margin );
}

void TskMain( void * parms )
{
	int seconds = 0;
	int minutes = 0;
	int hours = 0;

	while( true )
	{
		vTaskDelay( 1000 / portTICK_RATE_MS );

		++seconds;
		if( seconds >= 60 )
		{
			++minutes;
			seconds = 0;
		}

		if( minutes >= 60 )
		{
			++hours;
			minutes = 0;
		}

		char buffer[17] = { 0 };
		snprintf( buffer, 16, "%02i:%02i:%02i", hours, minutes, seconds );

		char buffer2[17] = { 0 };
		AlignCenter( buffer2, 16, buffer );

		LcdDisplayString( LCD_LINE_0, buffer2 );

	}
}

static uint32_t buffer[1025];

/*
 * This writes into the framebuffer for the LCD background layer i guess
 */
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
			while( RNG_GetFlagStatus( RNG_FLAG_DRDY ) == RESET )
				;

			buffer[i] = RNG_GetRandomNumber();
		}

		CRC_ResetDR();
		CRC_CalcBlockCRC( buffer, 1024 );
		buffer[1024] = CRC_GetCRC();

		SdRamWrite( buffer, 0, 1025 );

		memset( buffer, 0, sizeof buffer );

		SdRamRead( buffer, 0, 1025 );
		CRC_ResetDR();
		CRC_CalcBlockCRC( buffer, 1024 );
		volatile uint32_t crc = CRC_GetCRC();
		if( buffer[1024] != crc )
		{
			while( true )
			{
				vTaskDelay( 10 / portTICK_RATE_MS );
				LedToggle( LED4 );
			}
		}
	}
}

void TskBlink( void * parms )
{
	while( true )
	{
		vTaskDelay( 500 / portTICK_RATE_MS );
		LedToggle( LED3 );
	}
}

void vApplicationMallocFailedHook( void )
{
	while( true )
		;
}
