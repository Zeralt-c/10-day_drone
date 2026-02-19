#ifndef __COM_DEBUG_H
#define __COM_DEBUG_H

#include "usart.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

/* 使用宏定义的方式实现打印日志前，添加文件名和行号
日志输出打印CPU占用资源过多，因此在飞机正常飞行时需要关闭打印功能，因此需要一个开关来控制日志输出
*/

//日志输出打印开关
#define LOG_DEBUG_ENABLE 1

#ifdef LOG_DEBUG_ENABLE

//文件名宏定义，不打印路径
#define FILE_NAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
//冗余处理，文件路径使用/，因此需要相应处理
#define FILE_NAME_REPLACED (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(format, ...) printf("[DEBUG] %s:%d: " format "\n", FILE_NAME, __LINE__, ##__VA_ARGS__)

#else
#define LOG_DEBUG(format, ...)
#endif


#endif
