#ifndef COM_CONFIG_H
#define COM_CONFIG_H

#include "Com_debug.h"
#include "stdio.h"
#include "main.h"

/**
 * 遥控连接状态
 *  REMOTE_CONNECTED = 0,
    REMOTE_DISCONNECTED = 1,
*/
typedef enum {
    REMOTE_CONNECTED = 0,
    REMOTE_DISCONNECTED,
} Remote_State;

/**
 * 飞行状态
 *  IDLE = 0,
    NORMAL,
    FIXED_HEIGHT,
    FLIGHT_ERROR,
*/
typedef enum {
    IDLE = 0,
    NORMAL,
    FIXED_HEIGHT,
    FLIGHT_ERROR,
} Flight_State;

/**
 * @brief 遥控器数据结构体
 *  int16_t thr;    // 油门
    int16_t yaw;    // 航向
    int16_t pitch;  // 俯仰
    int16_t roll;   // 横滚
    uint8_t shutdown; // 关机按键
    uint8_t fixed_height; // 定高按键
 */
typedef struct {
    int16_t thr;    // 油门
    int16_t yaw;    // 航向
    int16_t pitch;  // 俯仰
    int16_t roll;   // 横滚
    uint8_t shutdown; // 关机按键，1表示关机
    uint8_t fixed_height; // 定高按键，1表示切换
} Remote_Data;

/**
 * @brief 油门状态
 *  FREE = 0,
    MAX,
    LEAVE_MAX,
    MIN,
    UNLOCK,
 */
typedef enum {
    FREE = 0,
    MAX,
    LEAVE_MAX,
    MIN,
    UNLOCK,
} Thr_State;

#endif // COM_CONFIG_H
