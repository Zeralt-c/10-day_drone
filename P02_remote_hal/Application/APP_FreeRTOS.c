#include "APP_FreeRTOS.h"


 void APP_FreeRTOS_Start(void){
    //在这里创建任务、队列、信号量等FreeRTOS对象
    //例如，创建一个简单的任务
    xTaskCreate(
        task1,       // 任务函数
        "Task1", // 任务名称
        TASK1_STACK_SIZE,         // 堆栈大小
        NULL,        // 任务参数
        TASK1_PRIORITY,           // 任务优先级
        &task1_handle         // 任务句柄
    );

    //启动调度器，开始执行任务
    vTaskStartScheduler();
}

void task1( void *args ){
    while(1){
        LOG_DEBUG("Task1 is running");
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时1秒
    }
}