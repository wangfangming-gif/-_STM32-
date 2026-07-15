#include "./BSP/Buzzer/Buzzer.h"

GPIO_InitTypeDef buzzer_gpio_init;

/*蜂鸣器硬件初始化*/
void buzzer_hardware_init(void)
{
	BUZZER_RCC;	//使能时钟
	buzzer_gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	buzzer_gpio_init.Pin = BUZZER_PIN;
	buzzer_gpio_init.Pull = GPIO_PULLDOWN;	//下拉
	buzzer_gpio_init.Speed = GPIO_SPEED_FAST;
	
	HAL_GPIO_Init(BUZZER_GPIO,&buzzer_gpio_init);
}


