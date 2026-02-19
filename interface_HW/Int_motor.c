#include "Int_motor.h"

void Motor_SetSpeed(Motor_Struct *motor){
    if(motor->speed > 1000) {
        motor->speed = 1000; // 限制转速最大值为1000
        LOG_DEBUG("motor speed too high");
    }
    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, motor->speed); // 设置定时器比较寄存器，控制PWM占空比
}

void Motor_Start(Motor_Struct *motor){
    HAL_TIM_PWM_Start(motor->htim, motor->channel); // 启动定时器PWM输出
}


