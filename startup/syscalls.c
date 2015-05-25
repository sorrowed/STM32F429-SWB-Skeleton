/*
 * system.c
 *
 *  Created on: May 25, 2015
 *      Author: tom
 */
#include "stm32f4xx.h"
#include "core_cm4.h"

#include <sys/stat.h>
#include <errno.h>

int _close( int file )
{
	return -1;
}

int _fstat( int file, struct stat *st )
{
	return -1;
}

int _lseek( int file, int ptr, int dir )
{
	return -1;
}

int _isatty( int file )
{
	errno = EBADF;
	return 0;
}

int _read( int file, char *ptr, int len )
{
	errno = EBADF;
	return -1;
}

int _write( int file, char *ptr, int len )
{
	for( int i = 0; i < len; ++i )
		ITM_SendChar( *ptr++ );

	return len;
}

