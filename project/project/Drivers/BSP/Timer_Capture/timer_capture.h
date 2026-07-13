#ifndef __MY_TIMER_CAPTURE_H
#define __MY_TIMER_CAPTURE_H

#include "./SYSTEM/sys/sys.h"

#define DHT11_Data_MaxCount 90

typedef struct 
{
    uint16_t time;             //计数，因为定时器是1us，所以这里就是1us的计数
    uint8_t Voltage_Level;      //高低电平  
}DHT11_Data_Struct;

#define dht11_timercapture_rcc_enable __HAL_RCC_TIM9_CLK_ENABLE()
void temp_humi_Capture_Init(void);
void DHT11_Get_Temp_Humi_Start(void);
int DHT11_Data_Handle(uint8_t *humidity, uint8_t *temperature);
void DHT11_Get_Temp_Humi_Stop(void);

//DHT11开始采集回调函数
void DHT11_StartCapture_Callback(void);
void DHT11_Loop_Handle(void);

#endif
