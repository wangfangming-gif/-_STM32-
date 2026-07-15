#ifndef __MYBUZZER_H
#define __MYBUZZER_H

#include "./SYSTEM/sys/sys.h"

#define BUZZER_RCC __HAL_RCC_GPIOF_CLK_ENABLE()
#define BUZZER_GPIO GPIOF
#define BUZZER_PIN GPIO_PIN_8

//´ò¿ª·äÃùÆ÷
#define BUZZER_ON HAL_GPIO_WritePin(BUZZER_GPIO,BUZZER_PIN,GPIO_PIN_SET);
//¹Ø±Õ·äÃùÆ÷
#define BUZZER_OFF HAL_GPIO_WritePin(BUZZER_GPIO,BUZZER_PIN,GPIO_PIN_RESET);

void buzzer_hardware_init(void);


#endif
