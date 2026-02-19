#ifndef _APP_FREERTOS_H_
#define _APP_FREERTOS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
#include "main.h"
#include "Int_motor.h"
#include "Int_led.h"
#include "Int_SI24R1.h"
#include "Com_config.h"

/**
 *@brief  FreeRTOS启动函数
 */
void APP_FreeRTOS_Start(void);

/**
 * @brief Task1声明
 */
void task1( void *args );


#endif
