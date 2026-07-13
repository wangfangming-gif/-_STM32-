#include "./BSP/ADC/ADC.h"
#include "./BSP/LIGHT/light.h"

DMA_HandleTypeDef dma_adc_handle;
ADC_HandleTypeDef adc_handle;

DMA_HandleTypeDef dma_adc3_handle;
ADC_HandleTypeDef adc3_handle;


extern volatile uint32_t temp_adc_value;
extern unsigned char flag_adc_get_done;
extern volatile uint8_t flag_adc3_get_done;

void adc_init(void)
{
    ADC_ChannelConfTypeDef channel_temp;

    __HAL_RCC_ADC1_CLK_ENABLE();

    adc_handle.Instance = ADC1;
    adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
    adc_handle.Init.Resolution = ADC_RESOLUTION_12B;
    adc_handle.Init.ScanConvMode = DISABLE;
    adc_handle.Init.ContinuousConvMode = DISABLE;
    adc_handle.Init.DiscontinuousConvMode = DISABLE;
    adc_handle.Init.NbrOfDiscConversion = 1;
    adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle.Init.NbrOfConversion = 0;
    adc_handle.Init.DMAContinuousRequests = DISABLE;
    adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    HAL_ADC_Init(&adc_handle);

    channel_temp.Channel = ADC_CHANNEL_5;          /* ADC1_IN1: PA1 */
    channel_temp.Rank = 1;
    channel_temp.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    channel_temp.Offset = 0;

    HAL_ADC_ConfigChannel(&adc_handle, &channel_temp);
}

//adc的中断初始化函数
void adc_init_it(void)
{
	ADC_ChannelConfTypeDef channel_temp;
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	adc_handle.Instance = ADC1;
	adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;	//时钟决定了速率
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;	//12位的分辨率
	adc_handle.Init.ScanConvMode = DISABLE;						//扫描模式
	adc_handle.Init.ContinuousConvMode = DISABLE;			//连续模式
	adc_handle.Init.DiscontinuousConvMode = DISABLE;	//间断模式
	adc_handle.Init.NbrOfDiscConversion = 1;					//
	adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;	//外部转换触发边沿
	adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;		//外部转换触发源
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;					//数据的对齐方向
	adc_handle.Init.NbrOfConversion = 0;											//转换通道的数量
	adc_handle.Init.DMAContinuousRequests = DISABLE;				//是否使用DMA
	adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;		//规则通道转换结束标志
	
	HAL_ADC_Init(&adc_handle);

	HAL_NVIC_SetPriority(ADC_IRQn,0,1);		//使能NVIC
	HAL_NVIC_EnableIRQ(ADC_IRQn);


	channel_temp.Channel = ADC_CHANNEL_5; 				 /* ADC1_IN1: PA1 */
	channel_temp.Rank = 1;
	channel_temp.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	channel_temp.Offset = 0;
	
	HAL_ADC_ConfigChannel(&adc_handle, &channel_temp);

}

//adc的中断初始化函数
void adc_init_dma(void)
{
	ADC_ChannelConfTypeDef channel_temp;
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	adc_handle.Instance = ADC1;
	adc_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;	//时钟决定了速率
	adc_handle.Init.Resolution = ADC_RESOLUTION_12B;	//12位的分辨率
	adc_handle.Init.ScanConvMode = DISABLE;						//扫描模式
	adc_handle.Init.ContinuousConvMode = ENABLE;			//连续模式
	adc_handle.Init.DiscontinuousConvMode = DISABLE;	//间断模式
	adc_handle.Init.NbrOfDiscConversion = 1;					//
	adc_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;	//外部转换触发边沿
	adc_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;		//外部转换触发源
	adc_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;					//数据的对齐方向
	adc_handle.Init.NbrOfConversion = 0;											//转换通道的数量
	adc_handle.Init.DMAContinuousRequests = ENABLE;				//是否使用DMA
	adc_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;		//规则通道转换结束标志
	
	HAL_ADC_Init(&adc_handle);

	channel_temp.Channel = ADC_CHANNEL_5; 				 /* ADC1_IN1: PA1 */
	channel_temp.Rank = 1;
	channel_temp.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	channel_temp.Offset = 0;
	
	HAL_ADC_ConfigChannel(&adc_handle, &channel_temp);

}

//使用adc3来采集光照信息
void adc3_init_dma(void)
{
	ADC_ChannelConfTypeDef capture_light_channel;
	__HAL_RCC_ADC3_CLK_ENABLE();		//打开ADC3的通道

	adc3_handle.Instance = ADC3;
	adc3_handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	adc3_handle.Init.ContinuousConvMode = DISABLE;		//连续转换模式
	adc3_handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;				//数据对齐
	adc3_handle.Init.DiscontinuousConvMode = DISABLE;	//间断模式
	adc3_handle.Init.DMAContinuousRequests = ENABLE;	//应该是是否使用DMA
	adc3_handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;			//规则通道转换结束标志
	adc3_handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;	//外部转换触发边沿
	adc3_handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;		//外部触发转换源
	adc3_handle.Init.NbrOfConversion = 1;		//转换通道数量
	adc3_handle.Init.NbrOfDiscConversion = 0;	//间断模式转换数量
	adc3_handle.Init.Resolution = ADC_RESOLUTION_12B;				//分辨率
	adc3_handle.Init.ScanConvMode = DISABLE;			//扫描模式
	
	HAL_ADC_Init(&adc3_handle);

	capture_light_channel.Channel = ADC_CHANNEL_5;
	capture_light_channel.Offset = 0;
	capture_light_channel.Rank = 1;
	capture_light_channel.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	
	HAL_ADC_ConfigChannel(&adc3_handle,&capture_light_channel);
}

uint16_t adc_read_once(void)
{
    uint16_t value = 0;
		
    HAL_ADC_Start(&adc_handle);

    if (HAL_ADC_PollForConversion(&adc_handle, 20) == HAL_OK)
    {
        value = (uint16_t)HAL_ADC_GetValue(&adc_handle);
    }

    HAL_ADC_Stop(&adc_handle);

    return value;
}

uint16_t adc_read_average(uint8_t times)
{
    uint8_t i;
    uint32_t sum = 0;

    if (times == 0)
    {
        return 0;
    }

    adc_read_once();
    HAL_Delay(1);

    for (i = 0; i < times; i++)
    {
        sum += adc_read_once();
        HAL_Delay(1);
    }

    return (uint16_t)(sum / times);
}


void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    GPIO_InitTypeDef gpio_init;

    if (hadc->Instance == ADC1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        gpio_init.Pin = GPIO_PIN_5;
        gpio_init.Mode = GPIO_MODE_ANALOG;
        gpio_init.Pull = GPIO_NOPULL;

        HAL_GPIO_Init(GPIOA, &gpio_init);

        //现在开始使能DMA
        dma_adc_handle.Instance = DMA2_Stream0;		//ADC1的DMA
        dma_adc_handle.Init.Channel = DMA_CHANNEL_0;	//ADC1的DMA通道
        dma_adc_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
        dma_adc_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        dma_adc_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        dma_adc_handle.Init.MemBurst = DMA_MBURST_SINGLE;
        dma_adc_handle.Init.MemDataAlignment =DMA_MDATAALIGN_HALFWORD;
        dma_adc_handle.Init.MemInc = DMA_MINC_ENABLE;
        dma_adc_handle.Init.Mode = DMA_CIRCULAR;
        dma_adc_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
        dma_adc_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        dma_adc_handle.Init.PeriphInc = DMA_PINC_DISABLE;
        dma_adc_handle.Init.Priority = DMA_PRIORITY_MEDIUM;

		HAL_DMA_Init(&dma_adc_handle);

		__HAL_LINKDMA(&adc_handle,DMA_Handle,dma_adc_handle);

        HAL_NVIC_SetPriority(ADC_IRQn,1,1);
        HAL_NVIC_EnableIRQ(ADC_IRQn);

        HAL_NVIC_SetPriority(DMA2_Stream0_IRQn,1,0);
        HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
        
    }
	else if(hadc->Instance == ADC3)	//如果是ADC3的话，我这里同样需要初始化DMA
	{
		// __HAL_RCC_ADC3_CLK_ENABLE();	//打开ADC3
		__HAL_RCC_GPIOF_CLK_ENABLE();
		__HAL_RCC_DMA2_CLK_ENABLE();	//打开DMA2的时钟

		gpio_init.Mode = GPIO_MODE_ANALOG;	//模拟模式
		gpio_init.Pin = GPIO_PIN_7;			//PF7
		gpio_init.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOF,&gpio_init);

		dma_adc3_handle.Instance = DMA2_Stream1;		//ADC3
		dma_adc3_handle.Init.Channel = DMA_CHANNEL_2;
		dma_adc3_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
		dma_adc3_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		dma_adc3_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		dma_adc3_handle.Init.MemBurst =  DMA_MBURST_SINGLE;
		dma_adc3_handle.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
		dma_adc3_handle.Init.MemInc = DMA_MINC_ENABLE;
		dma_adc3_handle.Init.Mode = DMA_CIRCULAR;
		dma_adc3_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
		dma_adc3_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		dma_adc3_handle.Init.PeriphInc = DMA_PINC_DISABLE;
		dma_adc3_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
		HAL_DMA_Init(&dma_adc3_handle);		//DMA初始化

		__HAL_LINKDMA(&adc3_handle,DMA_Handle,dma_adc3_handle);

		HAL_NVIC_SetPriority(ADC_IRQn,1,1);
        HAL_NVIC_EnableIRQ(ADC_IRQn);

        HAL_NVIC_SetPriority(DMA2_Stream1_IRQn,1,0);
        HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
	}
}

void DMA2_Stream0_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&dma_adc_handle);
}

void DMA2_Stream1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&dma_adc3_handle);
}

//ADC的中断服务函数
void ADC_IRQHandler(void)
{
	HAL_ADC_IRQHandler(&adc_handle);
}

//然后这里需要添加
//转换完成
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)	//如果是ADC1的话
	{
//		temp_adc_value = HAL_ADC_GetValue(hadc);
		flag_adc_get_done = 1;
	}
	else if(hadc->Instance == ADC3)	//ADC3转换完成
	{
		Light_AdcDmaCpltCallback();	//ADC3转换完成
	}
}
