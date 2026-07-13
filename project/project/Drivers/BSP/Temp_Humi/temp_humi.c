#include "./BSP/Temp_Humi/temp_humi.h"
#include "./BSP/MY_USART/usart_test.h"

//DHT11温湿度传感器，引脚输出初始化
void temp_humi_gpio_ouput(void)
{
    GPIO_InitTypeDef gpio_init_struct;

    dht11_gpio_rcc_enable;   //打开时钟

    //这里暂时不需要复用
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FAST;
    gpio_init_struct.Pin = dht11_gpio_pin;
    HAL_GPIO_Init(dht11_gpio_port,&gpio_init_struct);
    
}
