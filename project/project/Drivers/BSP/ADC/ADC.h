#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"

void adc_init(void);
void adc_init_it(void);
void adc_init_dma(void);

void adc3_init_dma(void);


uint16_t adc_read_once(void);
uint16_t adc_read_average(uint8_t times);

#endif
