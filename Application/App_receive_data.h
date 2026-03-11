#ifndef _APP_RECEIVE_DATA_H_
#define _APP_RECEIVE_DATA_H_

#include "APP_FreeRTOS.h"

//帧头校验宏定义
#define FRAME_HEADER_CHECK_1 'z'
#define FRAME_HEADER_CHECK_2 'z'
#define FRAME_HEADER_CHECK_3 'z'

//最大重试发送次数
#define MAX_FAIL_COUNT 10


/**
 * @brief 接收数据，解析为结构体
 * 
 * @return uint8_t 0:成功，1:失败
 */
uint8_t App_receive_data(void);

/**
 * @brief 处理遥控器状态，更新飞行状态等
 * 
 * @param res 接收数据结果，0:成功，1:失败
 */
void App_process_connect_state(uint8_t res);

/**
 * @brief 处理解锁逻辑
 * 
 * @return uint8_t 0:成功，1:失败
 */
static uint8_t App_process_unlock_flight(void);

/**
 * @brief 处理飞行状态，更新飞行状态等,使用状态机处理
 * 
 */
void App_process_flight_state(void);
#endif // _APP_RECEIVE_DATA_H_

