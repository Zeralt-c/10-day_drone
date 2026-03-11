#include "App_receive_data.h"


Remote_Data remote_data;//遥控器数据
extern Remote_State remote_state;//遥控器连接状态
extern Flight_State flight_state;//飞行状态
uint8_t data_buffer[TX_PLOAD_WIDTH] = {0};//接收数据缓冲区
/**
 * @brief 数据格式
 *  
    data_buffer[0] = FRAME_HEADER_CHECK_1; 
    data_buffer[1] = FRAME_HEADER_CHECK_2;
    data_buffer[2] = FRAME_HEADER_CHECK_3;
    //数据remote_data，高8位在前
    data_buffer[3] = remote_data.thr >> 8;
    data_buffer[4] = remote_data.thr & 0xFF;

    data_buffer[5] = remote_data.yaw >> 8;
    data_buffer[6] = remote_data.yaw & 0xFF;

    data_buffer[7] = remote_data.pitch >> 8;
    data_buffer[8] = remote_data.pitch & 0xFF;

    data_buffer[9] = remote_data.roll >> 8;
    data_buffer[10] = remote_data.roll & 0xFF;

    //发完清除
    taskENTER_CRITICAL();
    data_buffer[11] = remote_data.shutdown;
    remote_data.shutdown = 0;
    data_buffer[12] = remote_data.fixed_height;
    remote_data.fixed_height = 0;
    taskEXIT_CRITICAL();

    for(int i = 0; i < 13; i++)
    {
        checksum += data_buffer[i];
    }
    //13字节数据，4字节校验和
    data_buffer[13] = checksum >> 24;
    data_buffer[14] = checksum >> 16;
    data_buffer[15] = checksum >> 8;
    data_buffer[16] = checksum & 0xFF;
    
 */

 uint8_t fail_count = 0;//接收失败计数
Thr_State thr_state = FREE;//油门状态
uint32_t time_now = 0;//当前时间
uint32_t time_max_start = 0;//最大开始时间
uint32_t time_min_start = 0;//最小开始时间
uint32_t time_leave_max_start = 0;//离开最大开始时间
uint32_t time_max = 0;//最大时间
uint32_t time_min = 0;//最小时间
uint32_t time_leave_max = 0;//离开最大时间



uint8_t App_receive_data(void)
{
    memset(data_buffer, 0, TX_PLOAD_WIDTH);
    SI24R1_RxPacket(data_buffer);
    if(strlen((char *)data_buffer) == 0){
        //没有接收到数据
        return 1;
    }
    //解析数据
    //检查帧头
    if(data_buffer[0] != FRAME_HEADER_CHECK_1 || data_buffer[1] != FRAME_HEADER_CHECK_2 || data_buffer[2] != FRAME_HEADER_CHECK_3){
        return 1;
    }

    //检查校验和
    uint32_t checksum = 0;
    for(int i = 0; i < 13; i++)
    {
        checksum += data_buffer[i];
    }
    if(checksum != (data_buffer[13] << 24 | data_buffer[14] << 16 | data_buffer[15] << 8 | data_buffer[16])){
        return 1;
    }
    
    remote_data.thr = (data_buffer[3] << 8) | data_buffer[4];
    remote_data.yaw = (data_buffer[5] << 8) | data_buffer[6];
    remote_data.pitch = (data_buffer[7] << 8) | data_buffer[8];
    remote_data.roll = (data_buffer[9] << 8) | data_buffer[10];
    remote_data.shutdown = data_buffer[11];
    remote_data.fixed_height = data_buffer[12];

    LOG_DEBUG(":%d,%d,%d,%d,%d,%d\n",remote_data.thr, remote_data.yaw, remote_data.pitch, remote_data.roll,remote_data.shutdown,remote_data.fixed_height);
	return 0;
}

void App_process_connect_state(uint8_t res)
{
    //一次接收成功，则认为连接成功
    if(res == 0){
        remote_state = REMOTE_CONNECTED;
    }

    //连接失败计数
    if(res != 0){
        fail_count++;
        if(fail_count >= MAX_FAIL_COUNT){
            remote_state = REMOTE_DISCONNECTED;
            fail_count = 0;//清零
        }
    }
}

static uint8_t App_process_unlock_flight(void)
{
    time_now = xTaskGetTickCount();
    //安全问题，油门为0时才能解锁。设计为油门MAX 1s，MIN 1s才能解锁
    switch(thr_state)
    {
        case FREE:
            if(remote_data.thr > 900){
                thr_state = MAX;
                time_max_start = xTaskGetTickCount();
            }
            break;
        case MAX:
            time_max = time_now - time_max_start;
            if(remote_data.thr < 900){
                if(time_max < 1000){
                    thr_state = FREE;
                    time_max = 0;
                }
                else{
                    thr_state = LEAVE_MAX;
                    time_leave_max_start = xTaskGetTickCount();
                    time_max = 0;
                }
            }
            break;
        case LEAVE_MAX:
            time_leave_max = time_now - time_leave_max_start;
            if(remote_data.thr < 100){
                thr_state = MIN;
                time_min_start = xTaskGetTickCount();
                time_leave_max = 0;
            }
            if (time_leave_max > 100000){
                thr_state = FREE;
                time_leave_max = 0;
            }
            break;
        case MIN:
            time_min = time_now - time_min_start;
            if(time_min < 1000){
                if(remote_data.thr > 100){
                thr_state = FREE;
                time_min = 0;
                }
            }
            else{
                thr_state = UNLOCK;
                time_min = 0;
            }
            break;
        case UNLOCK:
            break;
        default:
            break;
    }
    if(thr_state == UNLOCK){
        return 0;
    }
    return 1;
}   
void App_process_flight_state(void)
{
    //处理飞行状态
    switch(flight_state)
    {
        case IDLE:
            if(App_process_unlock_flight() == 0){
                flight_state = NORMAL;
            }
            break;
        case NORMAL:
            if(remote_data.fixed_height == 1){
                flight_state = FIXED_HEIGHT;
                remote_data.fixed_height = 0;
            }
            //失联处理
            if(remote_state == REMOTE_DISCONNECTED){
                flight_state = FLIGHT_ERROR;
            }
            break;
        case FIXED_HEIGHT:
            if(remote_data.fixed_height == 1){
                flight_state = NORMAL;
                remote_data.fixed_height = 0;
            }
            //失联处理
            if(remote_state == REMOTE_DISCONNECTED){
                flight_state = FLIGHT_ERROR;
            }
            break;
        case FLIGHT_ERROR:
            //处理失联，缓慢停止电机
            //TODO
            vTaskDelay(pdMS_TO_TICKS(1000));
            flight_state = IDLE;
            break;
        default:
            break;
    }
}
