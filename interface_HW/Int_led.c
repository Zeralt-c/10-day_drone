#include "Int_led.h"

/**
 * @brief  打开LED灯，低电平点亮
 * @param  led: LED结构体指针，包含LED的GPIO端口和引脚信息
 */
void LED_On(LED_Struct *led) {
    HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_RESET);
}

/**
 * @brief  关闭LED灯，高电平熄灭
 * @param  led: LED结构体指针，包含LED的GPIO端口和引脚信息
 */
void LED_Off(LED_Struct *led) {
    HAL_GPIO_WritePin(led->GPIO_Port, led->GPIO_Pin, GPIO_PIN_SET);
}

/**
 * @brief 翻转LED灯的状态
 * @param led: LED结构体指针，包含LED的GPIO端口和引脚信息
 */
void LED_Toggle(LED_Struct *led) {
    HAL_GPIO_TogglePin(led->GPIO_Port, led->GPIO_Pin);
}