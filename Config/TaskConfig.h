/*
 * TaskConfig.h
 *
 *  Created on: May 28, 2015
 *      Author: tom
 */

#ifndef TASKCONFIG_H_
#define TASKCONFIG_H_

#define TskMainPrio    (tskIDLE_PRIORITY + 2)
#define TskMainStack   (configMINIMAL_STACK_SIZE)

#define TskBlinkPrio	(tskIDLE_PRIORITY + 1)
#define TskBlinkStack	(configMINIMAL_STACK_SIZE)

#endif /* TASKCONFIG_H_ */
