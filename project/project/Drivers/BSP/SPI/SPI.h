#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"

void SPI1_Init(void);       //SPI1函数初始化
void SPI1_DMA_Init(void);	//SPI的DMA进行初始化


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void SPI1_IRQHandler(void);


#endif
