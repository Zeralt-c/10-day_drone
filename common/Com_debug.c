#include "Com_debug.h"

#if (COM_DEBUG_CONTROL_MODE == COM_DEBUG_CONTROL_MODE_RUNTIME)
// 运行时日志开关：默认关闭（正常飞行不输出）
volatile uint8_t g_com_debug_enable = 0;

void ComDebug_SetEnable(uint8_t en)
{
    g_com_debug_enable = (en != 0U) ? 1U : 0U;
}
#endif

//重定向fputc函数
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 1000);
    return ch;
}
