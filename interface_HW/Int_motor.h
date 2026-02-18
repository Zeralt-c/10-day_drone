#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__

#include "main.h"
#include "tim.h"
#include "Com_debug.h"
typedef struct{
    TIM_HandleTypeDef *htim;  // 定时器句柄
    uint16_t channel;         // 通道号
    uint16_t speed;           // 电机转速范围为0-1000，默认值为200，对应PWM占空比20%       
} Motor_Struct;


/**
 * @brief 电机控制接口，设置电机转速
 * @param *motor 电机结构体指针，包含定时器句柄、通道号和转速
 * @note 该函数通过设置定时器的比较寄存器来控制电机转速
 */
void Motor_SetSpeed(Motor_Struct *motor);

/**
 * @brief 电机启动接口，启动电机
 * @param *motor 电机结构体指针，包含定时器句柄、通道号和转速
 */
void Motor_Start(Motor_Struct *motor);
#endif
