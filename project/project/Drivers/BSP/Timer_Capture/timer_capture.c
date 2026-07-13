#include "./BSP/Timer_Capture/timer_capture.h"
#include "./BSP/Temp_Humi/temp_humi.h"
#include "./BSP/MY_USART/usart_test.h"
#include "./BSP/Timer/Timer.h"

#include <stdio.h>

TIM_HandleTypeDef TIM9_Handle;
TIM_IC_InitTypeDef TIM9_Channel_Handle;

static volatile DHT11_Data_Struct DHT11_Data_Buffer[DHT11_Data_MaxCount];
volatile uint8_t DHT11_Capture_Done = 0; // 捕获完成
volatile uint16_t DHT11_Edge_Count = 0;  // 上升、下降沿数量
static volatile uint16_t DHT11_Last_Capture = 0;
static volatile uint8_t DHT11_Capture_Started = 0;

extern volatile uint8_t usart_send_done; // 发送结束标志位,1代表发送完成，0代表发送未完成
uint8_t Timer_Wait_DHT11Back_HandleFlag = 0;

void DHT11_Wait_DataBack(void);

// 定时器输入捕获引脚初始化
void temp_humi_Capture_Init(void)
{
    dht11_timercapture_rcc_enable;

    TIM9_Handle.Instance = TIM9;
    TIM9_Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 是否开启自动重载的功能
    TIM9_Handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;             // 采样时钟分频，但是不是计数时钟分频
    TIM9_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;                   // 向上计数的模式
    TIM9_Handle.Init.Period = 0xFFFF;                                    // 自动重状态寄存器
    TIM9_Handle.Init.Prescaler = 168 - 1;                                // 预分频器的值
    HAL_TIM_IC_Init(&TIM9_Handle);                                       // 初始化定时器9

    __HAL_TIM_CLEAR_FLAG(&TIM9_Handle, TIM_FLAG_UPDATE);

    // 定时器9的通道参数初始化
    TIM9_Channel_Handle.ICFilter = 0;
    TIM9_Channel_Handle.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
    TIM9_Channel_Handle.ICPrescaler = TIM_ICPSC_DIV1;
    TIM9_Channel_Handle.ICSelection = TIM_ICSELECTION_DIRECTTI;

    HAL_TIM_IC_ConfigChannel(&TIM9_Handle, &TIM9_Channel_Handle, TIM_CHANNEL_1);

    // //开始输入捕获
    // HAL_TIM_IC_Start_IT(&TIM9_Handle,TIM_CHANNEL_1);
}

void temp_humi_capture_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    dht11_gpio_rcc_enable;

    gpio_init_struct.Alternate = GPIO_AF3_TIM9;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pin = dht11_gpio_pin;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(dht11_gpio_port, &gpio_init_struct);
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM9) // 如果是TIM9的话
    {
        GPIO_InitTypeDef gpio_init_struct;

        dht11_timercapture_rcc_enable;
        dht11_gpio_rcc_enable;

        gpio_init_struct.Alternate = GPIO_AF3_TIM9;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pin = dht11_gpio_pin;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FAST;

        HAL_GPIO_Init(dht11_gpio_port, &gpio_init_struct);

        HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
    }
}

// 定时器9的中断服务函数
void TIM1_BRK_TIM9_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM9_Handle);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    uint16_t capture_count = 0;
    uint16_t interval_time = 0;
    uint16_t temp_level = 0;
    // 如果是定时器9，并且是通道1的话，那么我这里应该要去捕捉一下
    if (htim->Instance == TIM9 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        capture_count = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        temp_level = HAL_GPIO_ReadPin(dht11_gpio_port, dht11_gpio_pin) ? 0 : 1;

        if (DHT11_Capture_Started == 0)
        {
            DHT11_Last_Capture = capture_count;
            DHT11_Capture_Started = 1;
            return;
        }

        interval_time = capture_count - DHT11_Last_Capture;
        DHT11_Last_Capture = capture_count;

        if (DHT11_Edge_Count < DHT11_Data_MaxCount)
        {
            DHT11_Data_Buffer[DHT11_Edge_Count].time = interval_time;
            DHT11_Data_Buffer[DHT11_Edge_Count].Voltage_Level = temp_level;

            DHT11_Edge_Count++;
        }

        if (DHT11_Edge_Count >= DHT11_Data_MaxCount) // 如果收到的数量大于最大数量
        {
            HAL_TIM_IC_Stop_IT(&TIM9_Handle, TIM_CHANNEL_1);
            DHT11_Capture_Done = 1;
        }
    }
}

// DHT11开始采集回调函数
void DHT11_StartCapture_Callback(void)
{
    DHT11_Get_Temp_Humi_Start();
    Timer_Wait_DHT11Back_HandleFlag = Timer6_RegisterTask(DHT11_Wait_DataBack, 3);
}

void DHT11_Get_Temp_Humi_Stop(void)
{
    HAL_TIM_IC_Stop_IT(&TIM9_Handle, TIM_CHANNEL_1);

    __HAL_TIM_DISABLE_IT(&TIM9_Handle, TIM_IT_UPDATE);
    __HAL_TIM_SET_COUNTER(&TIM9_Handle, 0);
    __HAL_TIM_CLEAR_FLAG(&TIM9_Handle, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG(&TIM9_Handle, TIM_FLAG_UPDATE);

    temp_humi_gpio_ouput();
}

void DHT11_Wait_DataBack(void)
{
    // 释放总线
    HAL_GPIO_WritePin(dht11_gpio_port, dht11_gpio_pin, GPIO_PIN_SET);

    // 初始化输入捕获功能的引脚
    temp_humi_capture_gpio_init();

    // 初始化输入捕获功能
    temp_humi_Capture_Init();

    // 清除输入捕获定时器的计数器值
    __HAL_TIM_SET_COUNTER(&TIM9_Handle, 0);
    __HAL_TIM_CLEAR_FLAG(&TIM9_Handle, TIM_FLAG_CC1);
    __HAL_TIM_CLEAR_FLAG(&TIM9_Handle, TIM_FLAG_UPDATE);

    // 开启输入捕获中断
    HAL_TIM_IC_Start_IT(&TIM9_Handle, TIM_CHANNEL_1);
    __HAL_TIM_ENABLE_IT(&TIM9_Handle, TIM_IT_UPDATE);

    Timer6_UnRegisterTask(Timer_Wait_DHT11Back_HandleFlag);
}

void DHT11_Get_Temp_Humi_Start(void)
{
    HAL_TIM_IC_Stop_IT(&TIM9_Handle, TIM_CHANNEL_1);
    DHT11_Capture_Done = 0;
    DHT11_Edge_Count = 0;
    DHT11_Last_Capture = 0;
    DHT11_Capture_Started = 0;

    temp_humi_gpio_ouput();
    // 拉低电平
    HAL_GPIO_WritePin(dht11_gpio_port, dht11_gpio_pin, GPIO_PIN_RESET);
}

int DHT11_Data_Handle(uint8_t *humidity, uint8_t *temperature)
{
    uint8_t data[5] = {0};
    uint16_t high_width[45];
    uint8_t high_count = 0;

    // 提取所有高电平宽度
    for (uint8_t i = 0; i < DHT11_Edge_Count; i++)
    {
        if (DHT11_Data_Buffer[i].Voltage_Level == 1)
        {
            high_width[high_count] = DHT11_Data_Buffer[i].time;
            high_count++;

            if (high_count >= 45)
                break;
        }
    }

    // high_width[0] 通常是 DHT11 响应阶段的 80us 高电平
    // 后面 40 个高电平才是数据位
    if (high_count < 41)
    {
        return -1;
    }

    for (uint8_t i = 0; i < 40; i++)
    {
        data[i / 8] <<= 1;

        // 跳过 high_width[0] 响应高电平
        if (high_width[i + 1] > 50)
        {
            data[i / 8] |= 1;
        }
    }

    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
    {
        return -2;
    }

    *humidity = data[0];
    *temperature = data[2];

    return 0;
}

void DHT11_Loop_Handle(void)
{	
	uint8_t main_temp;
	uint8_t main_humi;

	static uint8_t nihao_print_buffer[30];
	uint8_t nihao_print_count = 0;

    if (DHT11_Capture_Done == 1)
    {
        // 获取温湿度后我们就可以关掉了
        DHT11_Get_Temp_Humi_Stop();

        DHT11_Data_Handle(&main_temp, &main_humi);

        DHT11_Capture_Done = 0;
        DHT11_Edge_Count = 0;

        nihao_print_count = sprintf((char *)nihao_print_buffer,
                                    "Temp:%d , Humi:%d\r\n",
                                    main_temp, main_humi);
        my_usart_transmit_data(nihao_print_buffer, nihao_print_count);
    }
}
