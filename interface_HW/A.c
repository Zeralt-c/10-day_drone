#include "A.h"

void IP5305_Power_On(void)
{
           //启动电源
           HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
           //根据IP5305T的规格书，保持POWER_KEY引脚低电平至少100ms可以启动电源
           vTaskDelay(pdMS_TO_TICKS(100)); // 延时100ms，确保满足启动条件
           //之后可以根据需要继续保持低电平，或者设置为高电平
           HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
   
}

void IP5305_Power_Off(void)
{
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    vTaskDelay(pdMS_TO_TICKS(100)); 
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
    
    vTaskDelay(pdMS_TO_TICKS(200)); 
    //连按两次关机
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    vTaskDelay(pdMS_TO_TICKS(100)); 
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
    
}
