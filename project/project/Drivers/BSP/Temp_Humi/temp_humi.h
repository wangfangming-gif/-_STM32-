#ifndef __TEMP_HUMI_H
#define __TEMP_HUMI_H

#define dht11_gpio_rcc_enable    __HAL_RCC_GPIOE_CLK_ENABLE()
#define dht11_gpio_pin           GPIO_PIN_5
#define dht11_gpio_port          GPIOE

void temp_humi_gpio_ouput(void);

#endif
