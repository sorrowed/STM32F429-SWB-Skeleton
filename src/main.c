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

typedef struct
{
	int seconds;
	int minutes;
	int hours;
} RunningTimer_t;

static RunningTimer_t RunningTimer;

static xTaskHandle TskMainHandle;
static xTaskHandle TskBlinkHandle;
static xTaskHandle TskSdRamHandle;
static xTaskHandle TskLcdHandle;

static void TskMain( void * parms );
static void TskBlink( void * parms );
static void TskSdRam( void * parms );
static void TskLcd( void * parms );

static void RunningTimerRun( RunningTimer_t* timer );
static void OutputTime( const Point* position );

int main( void )
{
	LedInit();

	xTaskCreate( TskMain, TskMainDesc, TskMainStack, NULL, TskMainPrio,
			&TskMainHandle );
	xTaskCreate( TskBlink, TskBlinkDesc, TskBlinkStack, NULL, TskBlinkPrio,
			&TskBlinkHandle );
	xTaskCreate( TskSdRam, TskSdRamDesc, TskSdRamStack, NULL, TskSdRamPrio,
			&TskSdRamHandle );
	xTaskCreate( TskLcd, TskLcdDesc, TskLcdStack, NULL, TskLcdPrio,
			&TskLcdHandle );

	vTaskStartScheduler();

	return -1;
}

void TskMain( void * parms )
{
	while( true )
	{
		vTaskDelay( 1000 / portTICK_RATE_MS );

		RunningTimerRun( &RunningTimer );
	}
}

void TskLcd( void* parms )
{
	Point position =
	{ 0, 0 };

	LcdInit();

	LcdSetLayer( LCD_FOREGROUND_LAYER );
	LcdClear( LCD_COLOR_BLUE );
	LcdSetBackColor( LCD_COLOR_BLUE );
	LcdSetTextColor( LCD_COLOR_WHITE );

	int change = 0;

	while( true )
	{
		vTaskDelay( 1000 / portTICK_RATE_MS );

		if( change % 10 == 0 )
		{
			position.Y = ( (float) RNG_GetRandomNumber() / 0xFFFFFFFF ) * 13;
			position.X = ( (float) RNG_GetRandomNumber() / 0xFFFFFFFF ) * 8;
		}
		++change;

		OutputTime( &position );
	}
}

/*
 * This writes into the framebuffer for the LCD background layer i guess
 */
void TskSdRam( void * parms )
{
	SdRamInit();

	RCC_AHB2PeriphClockCmd( RCC_AHB2Periph_RNG, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_CRC, ENABLE );
	RNG_Cmd( ENABLE );

	while( true )
	{
		static uint32_t buffer[1025];

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

void OutputTime( const Point* position )
{
	static Point p = { 0, 0 };

	char buffer[17] = { 0 };

	memset( buffer, ' ', position->X );

	snprintf( buffer + position->X, 16-position->X, "%02i:%02i:%02i", RunningTimer.hours,
			RunningTimer.minutes, RunningTimer.seconds );

	if( p.X != position->X || p.Y != position->Y )
		LcdClearLine( LCD_LINE( p.Y ) );
	p = *position;

	LcdDisplayString( LCD_LINE( p.Y ), buffer );
}

void RunningTimerRun( RunningTimer_t* timer )
{
	++timer->seconds;
	if( timer->seconds >= 60 )
	{
		++timer->minutes;
		timer->seconds = 0;
	}
	if( timer->minutes >= 60 )
	{
		++timer->hours;
		timer->minutes = 0;
	}
}

void vApplicationMallocFailedHook( void )
{
	while( true )
		;
}
