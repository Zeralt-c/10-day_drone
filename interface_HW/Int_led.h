#ifndef __INT_LED_H__
#define __INT_LED_H__
#include "main.h"
#include "gpio.h"
#include "Com_debug.h"

typedef struct{
    GPIO_TypeDef *GPIO_Port;  // GPIO端口
    uint16_t GPIO_Pin;        // GPIO引脚
} LED_Struct;

void LED_On(LED_Struct *led);
void LED_Off(LED_Struct *led);
void LED_Toggle(LED_Struct *led);

#endif // __INT_LED_H__