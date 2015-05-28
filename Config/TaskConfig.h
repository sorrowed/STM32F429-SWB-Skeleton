/*
 * TaskConfig.h
 *
 *  Created on: May 28, 2015
 *      Author: tom
 */

#ifndef TASKCONFIG_H_
#define TASKCONFIG_H_

#define TskMainPrio		(tskIDLE_PRIORITY + 3)
#define TskMainStack	(512)
#define TskMainDesc		((const signed char* const)"Main")

#define TskBlinkPrio	(tskIDLE_PRIORITY + 1)
#define TskBlinkStack	(configMINIMAL_STACK_SIZE)
#define TskBlinkDesc	((const signed char* const)"Blink")

#define TskSdRamPrio	(tskIDLE_PRIORITY + 2)
#define TskSdRamStack	(configMINIMAL_STACK_SIZE)
#define TskSdRamDesc	((const signed char* const)"SdRam")

#endif /* TASKCONFIG_H_ */
