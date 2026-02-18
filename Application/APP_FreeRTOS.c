#include "APP_FreeRTOS.h"

/**
 * STM32F103C8T6 => SRAM 20k, 
 * 操作系统使用configTOTAL_HEAP_SIZE = 17k，
 * 实际使用中根据任务需求调整
 * TASK_STACK_SIZE定义任务堆栈大小，最小推荐128，字节数128*4，实际使用中根据任务需求调整
 */
//内存管理 => C中结构体通常保存在堆栈中，不会自动回收，可以循环使用同个结构体

/**
 * TASK_PRIORITY 定义各个任务优先级，数值越大优先级越高，实际使用中根据任务需求调整
 *@note 0是最低优先级，往往被用作空闲任务，1是正常优先级，2以上是高优先级
 */

//电源管理任务，负责监控电源状态，控制电源开关等
void power_task( void *args );
#define POWER_TASK_STACK_SIZE 128
#define POWER_TASK_PRIORITY 4   //维持电源启动状态，10s执行一次，优先级较高
TaskHandle_t power_task_handle;
#define POWER_TASK_PERIOD_MS 10000  //定义任务周期，单位为ms

//飞行任务，负责飞行控制算法的执行
Motor_Struct motor_left_top = {.htim = &htim3, .channel = TIM_CHANNEL_1, .speed = 30}; 
Motor_Struct motor_left_bottom = {.htim = &htim4, .channel = TIM_CHANNEL_4, .speed = 200};
Motor_Struct motor_right_top = {.htim = &htim2, .channel = TIM_CHANNEL_2, .speed = 200};
Motor_Struct motor_right_bottom = {.htim = &htim1, .channel = TIM_CHANNEL_3, .speed = 200};
void flight_task( void *args );
#define FLIGHT_TASK_STACK_SIZE 128
#define FLIGHT_TASK_PRIORITY 3
TaskHandle_t flight_task_handle;
#define FLIGHT_TASK_PERIOD_MS 6  //定义任务周期，单位为ms

//连接状态
Remote_State remote_state = REMOTE_DISCONNECTED; //初始状态为遥控器未连接

//飞行状态
Flight_State flight_state = IDLE; //初始状态为IDLE

//LED任务，负责LED的控制
LED_Struct led_left_top = {.GPIO_Port = LED1_GPIO_Port, .GPIO_Pin = LED1_Pin};
LED_Struct led_right_top = {.GPIO_Port = LED2_GPIO_Port, .GPIO_Pin = LED2_Pin};
LED_Struct led_right_bottom = {.GPIO_Port = LED3_GPIO_Port, .GPIO_Pin = LED3_Pin};
LED_Struct led_left_bottom = {.GPIO_Port = LED4_GPIO_Port, .GPIO_Pin = LED4_Pin};
void led_task( void *args );
#define LED_TASK_STACK_SIZE 128
#define LED_TASK_PRIORITY 1
TaskHandle_t led_task_handle;
#define LED_TASK_PERIOD_MS 100  //定义任务周期，单位为ms
//
 void APP_FreeRTOS_Start(void){
    //在这里创建任务、队列、信号量等FreeRTOS对象
    //电源管理任务，优先级较高，确保及时响应电源事件
    xTaskCreate(
        power_task,       // 任务函数
        "PowerTask", // 任务名称
        POWER_TASK_STACK_SIZE,         // 堆栈大小
        NULL,        // 任务参数
        POWER_TASK_PRIORITY,           // 任务优先级
        &power_task_handle         // 任务句柄
    );

    //创建飞行任务，优先级较高，确保飞行控制算法的及时执行
    xTaskCreate(
        flight_task,       // 任务函数
        "FlightTask", // 任务名称
        FLIGHT_TASK_STACK_SIZE,         // 堆栈大小
        NULL,        // 任务参数
        FLIGHT_TASK_PRIORITY,           // 任务优先级
        &flight_task_handle         // 任务句柄
    );
    //创建LED任务，优先级较低，确保不干扰飞行控制和电源管理
    xTaskCreate(
        led_task,       // 任务函数
        "LedTask", // 任务名称
        LED_TASK_STACK_SIZE,         // 堆栈大小
        NULL,        // 任务参数
        LED_TASK_PRIORITY,           // 任务优先级
        &led_task_handle         // 任务句柄
    );
    //启动调度器，开始执行任务
    vTaskStartScheduler();
}
void power_task( void *args ){
    //获取当前基准时间
    TickType_t current_tick = xTaskGetTickCount();
    while(1){
        /**每10s执行一次维持电源启动任务，
         * 为了防止机械启动干扰任务，先延时10s，确保机械启动完成后再执行电源维持操作
         */
        vTaskDelayUntil(&current_tick, pdMS_TO_TICKS(POWER_TASK_PERIOD_MS));

        //启动电源
        HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
        //根据IP5305T的规格书，保持POWER_KEY引脚低电平至少100ms可以启动电源
        vTaskDelay(pdMS_TO_TICKS(100)); // 延时100ms，确保满足启动条件
        //之后可以根据需要继续保持低电平，或者设置为高电平
        HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
        
    }
}

void flight_task( void *args ){
    //获取当前基准时间
    TickType_t current_tick = xTaskGetTickCount();
    while(1){
        //每6ms执行一次飞行控制任务，确保飞行控制算法的及时执行
        vTaskDelayUntil(&current_tick, pdMS_TO_TICKS(FLIGHT_TASK_PERIOD_MS));

        //在这里执行飞行控制算法
        //启动电机以调试
        //Motor_Start(&motor_left_top);
    }
}

void led_task( void *args ){
    //获取当前基准时间
    TickType_t current_tick = xTaskGetTickCount();
    //循环次数计数器，用于实现LED闪烁效果
    uint32_t blink_counter = 0;
    while(1){
        //1.判断连接状态，以前两个LED指示连接状态
        if(remote_state == REMOTE_DISCONNECTED) {
            //遥控器未连接，前两个熄灭
            LED_Off(&led_left_top);
            LED_Off(&led_right_top);
        } else if(remote_state == REMOTE_CONNECTED) {
            //遥控器已连接，前两个点亮
            LED_On(&led_left_top);
            LED_On(&led_right_top);
        }
        
        //2.判断飞行状态，以后两个LED指示飞行状态
        switch(flight_state) {
            case IDLE:
                //空闲状态，后两个LED交替闪烁，500ms亮500ms灭
                blink_counter++;
                //设置初始状态为左侧LED亮，右侧LED灭
                if(blink_counter == 0) {
                    LED_On(&led_left_bottom);
                    LED_Off(&led_right_bottom);
                } 
                //每500ms切换一次LED状态
                else if(blink_counter % 5 == 0) {
                    LED_Toggle(&led_left_bottom);
                    LED_Toggle(&led_right_bottom);
                }
                break;
            case NORMAL:
                //正常飞行状态，后两个LED点亮
                blink_counter = 0; //重置计数器，确保状态切换时闪烁效果正常
                LED_On(&led_left_bottom);
                LED_On(&led_right_bottom);
                break;
            case FIXED_HEIGHT:
                //定高飞行状态，后两个LED慢闪烁，500ms亮500ms灭
                blink_counter++;
                //设置初始状态
                if(blink_counter == 0) {
                    LED_On(&led_left_bottom);
                    LED_On(&led_right_bottom);
                } 
                //每500ms切换一次LED状态
                else if(blink_counter % 5 == 0) {
                    LED_Toggle(&led_left_bottom);
                    LED_Toggle(&led_right_bottom);
                }
                break;
            case ERROR:
                //错误状态，所有LED快闪烁，200ms亮200ms灭
                blink_counter++;
                //设置初始状态
                if(blink_counter == 0) {
                    LED_On(&led_left_top);
                    LED_On(&led_right_top);
                    LED_On(&led_left_bottom);
                    LED_On(&led_right_bottom);
                } 
                //每200ms切换一次LED状态
                else if(blink_counter % 2 == 0) {
                    LED_Toggle(&led_left_top);
                    LED_Toggle(&led_right_top);
                    LED_Toggle(&led_left_bottom);
                    LED_Toggle(&led_right_bottom);
                }
                break;
            default:
                break;
        }
        blink_counter%=10; //防止计数器溢出，保持在0-9范围内
        //每100ms执行一次LED控制任务，确保LED状态的及时更新
        vTaskDelayUntil(&current_tick, pdMS_TO_TICKS(LED_TASK_PERIOD_MS));
    }
}




