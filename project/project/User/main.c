

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
#include "./BSP/Buzzer/Buzzer.h"

#include "./BSP/IIC/IIC.h"

#include <stdio.h>

#define IIC_Address_Write 0xA0
#define IIC_Address_Read 0xA1

void temp_iic_write_data(void);
uint8_t temp_iic_read_data(void);
extern I2C_HandleTypeDef IIC1_handle;
uint8_t Flag_IIC_SendDone = 0;
uint8_t Flag_IIC_ReadDone = 0;



volatile uint32_t temp_adc_value;
unsigned char flag_adc_get_done;

uint8_t Light_Loop_Time = 10;
extern volatile uint8_t DHT11_Capture_Done;      //捕获完成
extern volatile uint16_t DHT11_Edge_Count;       //上升、下降沿数量

static uint8_t main_nihao_print_buffer[30];
uint8_t main_nihao_print_count = 0;

void key_click_handle(uint8_t key_id);
void key_double_click_handle(uint8_t key_id);

int main(void)
{
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
	buzzer_hardware_init();
	IIC1_DMA_Init();		//初始化IIC，来对EEPROM进行一下操作

		//注册回调函数
	Timer6_RegisterTask(Light_TimerCallback,100);
	Timer6_RegisterTask(DHT11_StartCapture_Callback,100);
	Timer6_RegisterTask(bsp_key_10ms_scan,1);
	Timer6_RegisterTask(uart_send_check,1);

	key_click_register_task(key_click_handle);
	key_double_click_register_task(key_double_click_handle);
	

//	temp_iic_write_data();
//	HAL_Delay(10);
//	temp_iic_read_data();
	
	while (1)
	{
		 modbus_handle();
		 Light_Handle();
		 DHT11_Loop_Handle();
	}
}




//IIC写操作
void temp_iic_write_data(void)
{
	static uint8_t IIC_write_buffer[9] = "helloworl";
	IIC_write_buffer[0] = 0x00;
	IIC_write_buffer[8] = '\0';
	Flag_IIC_SendDone = 0;
	HAL_I2C_Master_Transmit_DMA(&IIC1_handle,IIC_Address_Write,IIC_write_buffer,6);
	while(!Flag_IIC_SendDone)	//等待写入完成
	{
		
	}
	Flag_IIC_SendDone = 0;
}

//IIC读操作
uint8_t temp_iic_read_data(void)
{
	static uint8_t iic_read_write_address[2];
	iic_read_write_address[0] = 0x01;
	static uint8_t IIC_Read_buffer[9];
	Flag_IIC_ReadDone =0;

	HAL_I2C_Mem_Read_DMA(&IIC1_handle,
								IIC_Address_Read,
								0x00,
								I2C_MEMADD_SIZE_8BIT,
								IIC_Read_buffer,
								8);
								
	while(!Flag_IIC_ReadDone)	//等待写入完成
	{
	
	}
	Flag_IIC_ReadDone = 0;
	
	IIC_Read_buffer[8] = '\0';
	
	main_nihao_print_count = sprintf((char *)main_nihao_print_buffer,
													"IIC_D:%s\r\n",IIC_Read_buffer);
	uart_send_data(main_nihao_print_buffer, main_nihao_print_count);

	return 0;
}

void key_click_handle(uint8_t key_id)
{
	if(key_id == KEY0)
	{
		main_nihao_print_count = sprintf((char *)main_nihao_print_buffer,
		                        "KEY0_Click\r\n");
		uart_send_data(main_nihao_print_buffer, main_nihao_print_count);
	}
}

void key_double_click_handle(uint8_t key_id)
{
	if(key_id == KEY1)
	{
		main_nihao_print_count = sprintf((char *)main_nihao_print_buffer,
		                        "KEY1_D_Click\r\n");
		uart_send_data(main_nihao_print_buffer, main_nihao_print_count);
	}
}

