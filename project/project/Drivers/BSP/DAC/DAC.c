#include "./BSP/IIC/IIC.h"

DAC_HandleTypeDef dac_handle;


//dac³õÊ¼»¯
void dac_init(void)
{
	DAC_ChannelConfTypeDef channel_temp;

	__HAL_RCC_DAC_CLK_ENABLE();
	dac_handle.Instance = DAC1;
	HAL_DAC_Init(&dac_handle);

	channel_temp.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
	channel_temp.DAC_Trigger = DAC_TRIGGER_NONE;
	
	HAL_DAC_ConfigChannel(&dac_handle,&channel_temp,DAC_CHANNEL_1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
	if(hdac->Instance == DAC1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitTypeDef gpio_init;
		gpio_init.Mode = GPIO_MODE_ANALOG;
		gpio_init.Pin = GPIO_PIN_4;
		gpio_init.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA,&gpio_init);
	}
}




