#include "./BSP/SPI/SPI.h"
#include "./BSP/LED/led.h"

SPI_HandleTypeDef spi1_handletypedef;
DMA_HandleTypeDef spi1_tx;
DMA_HandleTypeDef spi1_rx;

/*
	uint8_t spi_read_buffer[20];
	uint8_t spi_write_buffer[20] =
	{
		0x09, 0x81, 0x32, 0x31
	};
	uint8_t spi_readwrite_count = 0;
	

	//1、SPI的主机、阻塞模式的使用方法
	SPI1_Init();
	if(HAL_SPI_Transmit(&spi1_handletypedef,spi_write_buffer,4,50) != HAL_OK)
	{
		printf("SPI输出失败!");
	}
	else
	{
		printf("SPI输出成功!");
	}

*/

/*
	//2、这个是中断的使用方法，不过需要注意的是，如果想要进入中断的话，要
	//设置中断优先级、并且需要调用中断服务函数
	SPI1_Init();		//初始化SPI1
	spi_readwrite_count = 4;
	//不知道是不是这样使用
	if(HAL_SPI_TransmitReceive_IT(&spi1_handletypedef,spi_write_buffer,
								spi_read_buffer,spi_readwrite_count) != HAL_OK)
	{
		//调用失败
		 printf("SPI IT调用失败!");
	}
	else
	{
		printf("SPI IT调用成功!");
	}
	if(HAL_SPI_Transmit_IT(&spi1_handletypedef,spi_write_buffer,spi_readwrite_count)!= HAL_OK)
	{
		printf("SPI发送调用成功");
		
	}
	//
	while(HAL_SPI_GetState(spi1_handletypedef) != HAL_SPI_STATE_READY)
	{}
*/

/*
	//3、这是DMA的使用方法
    SPI1_DMA_Init();
    if(HAL_SPI_Transmit_DMA(&spi1_handletypedef,spi_write_buffer,4) != HAL_OK)
    {
		printf("SPI dma发送失败!");
    }
	while (HAL_SPI_GetState(&spi1_handletypedef) != HAL_SPI_STATE_READY)
	{
	} 
*/


void SPI1_Init()
{
    //打开SPI1的时钟
    __HAL_RCC_SPI1_CLK_ENABLE();

    spi1_handletypedef.Instance = SPI1;
    spi1_handletypedef.Init.TIMode = SPI_TIMODE_DISABLE;
    spi1_handletypedef.Init.NSS = SPI_NSS_SOFT;
    spi1_handletypedef.Init.DataSize = SPI_DATASIZE_8BIT;
    spi1_handletypedef.Init.Direction = SPI_DIRECTION_2LINES;
    spi1_handletypedef.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi1_handletypedef.Init.Mode = SPI_MODE_MASTER;
    spi1_handletypedef.Init.CLKPolarity = SPI_POLARITY_HIGH;
    spi1_handletypedef.Init.CLKPhase = SPI_PHASE_2EDGE;
    spi1_handletypedef.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi1_handletypedef.Init.CRCPolynomial = 7;
    spi1_handletypedef.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    HAL_SPI_Init(&spi1_handletypedef);  //初始化SPI函数

    __HAL_SPI_ENABLE(&spi1_handletypedef);

    //设置NVIC
	HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(SPI1_IRQn);

}

void SPI1_DMA_Init()
{
    //打开SPI1的时钟
    __HAL_RCC_SPI1_CLK_ENABLE();

    spi1_handletypedef.Instance = SPI1;	//SPI
    spi1_handletypedef.Init.TIMode = SPI_TIMODE_DISABLE;	//不使用TI方式的SPI
    spi1_handletypedef.Init.NSS = SPI_NSS_SOFT;
    spi1_handletypedef.Init.DataSize = SPI_DATASIZE_8BIT;
    spi1_handletypedef.Init.Direction = SPI_DIRECTION_2LINES;
    spi1_handletypedef.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi1_handletypedef.Init.Mode = SPI_MODE_MASTER;
    spi1_handletypedef.Init.CLKPolarity = SPI_POLARITY_HIGH;
    spi1_handletypedef.Init.CLKPhase = SPI_PHASE_2EDGE;
    spi1_handletypedef.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi1_handletypedef.Init.CRCPolynomial = 7;
    spi1_handletypedef.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    HAL_SPI_Init(&spi1_handletypedef);  //初始化SPI函数

    __HAL_SPI_ENABLE(&spi1_handletypedef);

    //设置NVIC
//	HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
//	HAL_NVIC_EnableIRQ(SPI1_IRQn);

}


//HAL_SPI_Init会调用HAL_SPI_MspInit函数，所以在这里将gpio端口进行初始化
 void HAL_SPI_MspInit(SPI_HandleTypeDef *spi_handle)
 {
//    GPIO_InitTypeDef gpio_initstruct;
//    //如果是SPI1的话，那么我们把GPIO的端口给初始化一下
//    if(spi_handle->Instance == SPI1)
//    {
//        __HAL_RCC_GPIOB_CLK_ENABLE();   //PB14的时钟
//
//        gpio_initstruct.Pin =GPIO_PIN_14;
//        gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;     //推挽输出
//        gpio_initstruct.Pull = GPIO_PULLUP;             //上拉，所以默认是高电平
//        gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  //高速
//        HAL_GPIO_Init(GPIOB,&gpio_initstruct);
//
//        //初始化SPI1的复用引脚
//        gpio_initstruct.Pin = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_5;
//        gpio_initstruct.Mode = GPIO_MODE_AF_PP;
//        gpio_initstruct.Pull = GPIO_PULLUP;
//        gpio_initstruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//        gpio_initstruct.Alternate = GPIO_AF5_SPI1;
//
//        HAL_GPIO_Init(GPIOB,&gpio_initstruct);
//    }

	//初始化SPI的DMA

    GPIO_InitTypeDef gpio_initstruct;
    
    //如果是SPI1的话，那么我们把GPIO的端口给初始化一下
    if(spi_handle->Instance == SPI1)
    {
    	//1、打开时钟
		__HAL_RCC_GPIOB_CLK_ENABLE();	//GPIOB时钟
		__HAL_RCC_DMA2_CLK_ENABLE();	//DMA2时钟

		//2、初始化GPIO口
		gpio_initstruct.Alternate = GPIO_AF5_SPI1;	
		gpio_initstruct.Mode = GPIO_MODE_AF_PP;
		gpio_initstruct.Pin = GPIO_PIN_3;
		gpio_initstruct.Pull = GPIO_PULLUP;
		gpio_initstruct.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOB,&gpio_initstruct);

		gpio_initstruct.Pin = GPIO_PIN_4;		//MISO
		HAL_GPIO_Init(GPIOB,&gpio_initstruct);

		gpio_initstruct.Pin = GPIO_PIN_5;		//MOSI	
		HAL_GPIO_Init(GPIOB,&gpio_initstruct);

		//3、初始化DMA参数

		spi1_tx.Instance = DMA2_Stream3;
		spi1_tx.Init.Channel = DMA_CHANNEL_3;
		spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		spi1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		spi1_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		spi1_tx.Init.MemBurst = DMA_MBURST_SINGLE;
		spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		spi1_tx.Init.MemInc = DMA_MINC_ENABLE;		//内存地址增加
		spi1_tx.Init.Mode = DMA_NORMAL;
		spi1_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		spi1_tx.Init.PeriphInc = DMA_MINC_DISABLE;		//外设地址不增加
		spi1_tx.Init.Priority = DMA_PRIORITY_HIGH;		

		HAL_DMA_Init(&spi1_tx);

		__HAL_LINKDMA(spi_handle,hdmatx,spi1_tx);

		
		spi1_rx.Instance = DMA2_Stream2;
		spi1_rx.Init.Channel = DMA_CHANNEL_3;
		spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		spi1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		spi1_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		spi1_rx.Init.MemBurst = DMA_MBURST_SINGLE;
		spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		spi1_rx.Init.MemInc = DMA_MINC_ENABLE;		//内存地址增加
		spi1_rx.Init.Mode = DMA_NORMAL;
		spi1_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;
		spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		spi1_rx.Init.PeriphInc = DMA_MINC_DISABLE;		//外设地址不增加
		spi1_rx.Init.Priority = DMA_PRIORITY_HIGH;		

		HAL_DMA_Init(&spi1_rx);

		__HAL_LINKDMA(spi_handle,hdmarx,spi1_rx);

		//4、最后一步貌似是打开中断

		HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 1);
		HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);

		HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 2);
		HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

		HAL_NVIC_SetPriority(SPI1_IRQn, 0, 3);
		HAL_NVIC_EnableIRQ(SPI1_IRQn);

    }

 }

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
	}
}


void SPI1_IRQHandler(void)
{
    HAL_SPI_IRQHandler(&spi1_handletypedef);
}


void DMA2_Stream3_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&spi1_tx);
}

void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&spi1_rx);
}
