#ifndef _APP_FREERTOS_H_
#define _APP_FREERTOS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"

/**
 *@brief  启动FreeRTOS
 */
void APP_FreeRTOS_Start(void);

/**
 * @brief Task1
 */
void task1( void *args );

/*STM32F103C8T6 => SRAM 20k, 
操作系统使用configTOTAL_HEAP_SIZE = 17k，
实际使用中根据任务需求调整
*/

//堆栈大小最小推荐128,字节数128*4，实际使用中根据任务需求调整
#define TASK1_STACK_SIZE 128

/*任务优先级，数值越大优先级越高，实际使用中根据任务需求调整
 *@note 0是最低优先级，往往被用作空闲任务，1是正常优先级，2以上是高优先级
 */
#define TASK1_PRIORITY 1

//任务句柄
TaskHandle_t task1_handle;

#endif
