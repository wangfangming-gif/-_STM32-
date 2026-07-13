

/**
 ****************************************************************************************************
 * @file				main.c
 * @brief 			ADC sampling test
 ****************************************************************************************************
 */
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/DAC/DAC.h"
#include "./BSP/ADC/ADC.h"
#include "./BSP/MY_USART/usart_test.h"
#include "./BSP/MY_USART/Modbus.h"
#include "./BSP/MY_WWDG/wwdg.h"
#include "./BSP/LIGHT/light.h"
#include "./BSP/Timer/Timer.h"

#include "./BSP/Temp_Humi/temp_humi.h"
#include "./BSP/Timer_Capture/timer_capture.h"

#include "./BSP/Key/Key.h"

#include <stdio.h>

volatile uint32_t temp_adc_value;
unsigned char flag_adc_get_done;

uint8_t Light_Loop_Time = 10;
extern volatile uint8_t DHT11_Capture_Done;      //捕获完成
extern volatile uint16_t DHT11_Edge_Count;       //上升、下降沿数量

volatile uint8_t usart_send_done = 0;	//发送结束标志位,1代表发送完成，0代表发送未完成

int main(void)
{
	uint8_t hello_print_buffer[30] = {"nihaoa,shijie!!!!!"}; 

	static uint8_t temp_printf_buffer[30];
	uint8_t temp_printf_count = 0;

	uint8_t key_pre_state = 0;
	GPIO_PinState key_current_state = GPIO_PIN_SET;	
	GPIO_PinState Temp_key_state = GPIO_PIN_SET;	

	HAL_Init();
	sys_stm32_clock_init(336, 8, 2, 7);
	delay_init(168);
	led_init();
	modbus_init();		//初始化modbus
	wwdg_init();		//初始化窗口看门狗
	adc3_init_dma();	//初始化adc3
	Timer6_Init();
	Light_Init();		//初始化光强
	key_gpio_init();	//初始化按键
	key_soft_init();

	//注册回调函数
	Timer6_RegisterTask(Light_TimerCallback,100);
	Timer6_RegisterTask(DHT11_StartCapture_Callback,100);
	Timer6_RegisterTask(bsp_key_10ms_scan,1);

	//我想下哈，肯定先从简单的来，首先就是按键的编写，然后是串口的修改。
	//接下来开始扫描的程序的编写

	while (1)
	{
		// modbus_handle();
//		 Light_Handle();
		// DHT11_Loop_Handle();

	}
}

