#ifndef COM_CONFIG_H
#define COM_CONFIG_H

#include "Com_debug.h"
#include "stdio.h"
#include "main.h"

//遥控连接状态
typedef enum {
    REMOTE_CONNECTED = 0,
    REMOTE_DISCONNECTED,
} Remote_State;

//飞行状态
typedef enum {
    IDLE = 0,
    NORMAL,
    FIXED_HEIGHT,
    FLIGHT_ERROR,
} Flight_State;

#endif // COM_CONFIG_H
