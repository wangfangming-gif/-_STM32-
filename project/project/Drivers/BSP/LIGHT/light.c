#include "./BSP/LIGHT/light.h"
#include "./BSP/ADC/ADC.h"
#include "./BSP/MY_USART/usart_test.h"
#include <stdio.h>
#include <string.h>

extern ADC_HandleTypeDef adc3_handle;

static uint32_t light_adc_buffer[1];
static volatile uint8_t light_sample_done = 0;

void Light_Init(void)
{
	light_sample_done = 0;
}

void Light_TimerCallback(void)
{
	HAL_ADC_Start_DMA(&adc3_handle, light_adc_buffer, 1);
}


void Light_AdcDmaCpltCallback(void)
{
	light_sample_done = 1;
}

//处理光照强度
void Light_Handle(void)
{
	static uint8_t temp_printf_buffer[30];
	uint8_t temp_printf_count = 0;

	if(light_sample_done == 1)
	{
		temp_printf_count = sprintf((char *)temp_printf_buffer,
			"Light:%d\r\n",
		light_adc_buffer[0]);
		uart_send_data(temp_printf_buffer,temp_printf_count);
		light_sample_done = 0;
	}
}
