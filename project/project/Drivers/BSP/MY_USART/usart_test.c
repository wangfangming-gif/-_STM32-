#include "./BSP/MY_USART/usart_test.h"
#include "./BSP/LED/led.h"

UART_HandleTypeDef usart_handle;
DMA_HandleTypeDef usart_rx_handle;
DMA_HandleTypeDef usart_tx_handle;

#define tx_rx_buffercount 128
uart_receive_struct modbus_recieve_struct;
uint8_t modbus_receive_buffer[tx_rx_buffercount];

uint8_t usart_rx_buffer[tx_rx_buffercount];
uint32_t usart_rx_count;
uint8_t flag_usart_rx_done;

extern volatile uint8_t usart_send_done;	//发送结束标志位,1代表发送完成，0代表发送未完成


/*串口初始化*/
void usart_test_init(void)
{
	__HAL_RCC_USART1_CLK_ENABLE();
	
	usart_handle.Instance = USART1;
	usart_handle.Init.BaudRate = 9600;
	usart_handle.Init.Mode = UART_MODE_TX_RX;
	usart_handle.Init.Parity = UART_PARITY_NONE;
	usart_handle.Init.StopBits = UART_STOPBITS_1;
	usart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	usart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;	
	
	HAL_UART_Init(&usart_handle);
}

void usart_test_it_init(void)
{
	__HAL_RCC_USART1_CLK_ENABLE();

	usart_handle.Instance = USART1;
	usart_handle.Init.BaudRate = 9600;
	usart_handle.Init.Mode = UART_MODE_TX_RX;
	usart_handle.Init.Parity = UART_PARITY_NONE;
	usart_handle.Init.StopBits = UART_STOPBITS_1;
	usart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	usart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;	
	HAL_UART_Init(&usart_handle);
}

void usart_test_dma_init(uint32_t Temp_BaudRate)
{
	__HAL_RCC_USART1_CLK_ENABLE();

	usart_handle.Instance = USART1;
	usart_handle.Init.BaudRate = Temp_BaudRate;
	usart_handle.Init.Mode = UART_MODE_TX_RX;
	usart_handle.Init.Parity = UART_PARITY_NONE;
	usart_handle.Init.StopBits = UART_STOPBITS_1;
	usart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	usart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;	
	HAL_UART_Init(&usart_handle);

	//这里需要开始使用DMA
	HAL_UARTEx_ReceiveToIdle_DMA(&usart_handle,usart_rx_buffer,tx_rx_buffercount);
	__HAL_DMA_DISABLE_IT(usart_handle.hdmarx, DMA_IT_HT);
}

void HAL_UART_MspInit(UART_HandleTypeDef *husart)
{
	if(husart->Instance == USART1)	//初始化GPIO
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_DMA2_CLK_ENABLE();

		GPIO_InitTypeDef gpio_init;
		gpio_init.Alternate = GPIO_AF7_USART1;
		gpio_init.Mode = GPIO_MODE_AF_PP;
		gpio_init.Pin = GPIO_PIN_9;
		gpio_init.Pull = GPIO_PULLUP;
		gpio_init.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOA,&gpio_init);

		gpio_init.Pin = GPIO_PIN_10;
		HAL_GPIO_Init(GPIOA,&gpio_init);	//10和9

		//接下来设置DMA
		usart_tx_handle.Instance = DMA2_Stream7;
		usart_tx_handle.Init.Channel = DMA_CHANNEL_4;
		usart_tx_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
		usart_tx_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		usart_tx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		usart_tx_handle.Init.MemBurst = DMA_MBURST_SINGLE;
		usart_tx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		usart_tx_handle.Init.MemInc = DMA_MINC_ENABLE;
		usart_tx_handle.Init.Mode = DMA_NORMAL;
		usart_tx_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
		usart_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		usart_tx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
		usart_tx_handle.Init.Priority = DMA_PRIORITY_MEDIUM;
		HAL_DMA_Init(&usart_tx_handle);

		usart_rx_handle.Instance = DMA2_Stream5;
		usart_rx_handle.Init.Channel = DMA_CHANNEL_4;
		usart_rx_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
		usart_rx_handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		usart_rx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
		usart_rx_handle.Init.MemBurst = DMA_MBURST_SINGLE;
		usart_rx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		usart_rx_handle.Init.MemInc = DMA_MINC_ENABLE;
		usart_rx_handle.Init.Mode = DMA_NORMAL;
		usart_rx_handle.Init.PeriphBurst = DMA_PBURST_SINGLE;
		usart_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		usart_rx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
		usart_rx_handle.Init.Priority = DMA_PRIORITY_MEDIUM;	
		HAL_DMA_Init(&usart_rx_handle);

		__HAL_LINKDMA(&usart_handle,hdmatx,usart_tx_handle);
		__HAL_LINKDMA(&usart_handle,hdmarx,usart_rx_handle);

		//随便设置一个中断优先级
		HAL_NVIC_SetPriority(USART1_IRQn,0,1);
		HAL_NVIC_EnableIRQ(USART1_IRQn);

		//接收的NVIC
		HAL_NVIC_SetPriority(DMA2_Stream5_IRQn,1,0);
		HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);
		//发送的NVIC
		HAL_NVIC_SetPriority(DMA2_Stream7_IRQn,1,1);
		HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);
	}
}

void modbus_loop_buffer_init()
{
	loop_buffer_init(&modbus_recieve_struct,modbus_receive_buffer,tx_rx_buffercount);
}
uint8_t modbus_loop_buffer_read(uint8_t *data)
{
	return loop_buffer_pop(&modbus_recieve_struct,data); 
}
uint16_t modbus_loop_buffer_available()
{
	return loop_buffer_available(&modbus_recieve_struct);
}

void my_usart_transmit_data(uint8_t *data,uint16_t len)
{
	HAL_UART_Transmit_DMA(&usart_handle,data,len);
	return;
}

//首先是环形缓冲区的初始化
void loop_buffer_init(uart_receive_struct *temp_value,uint8_t *temp_buffer,uint16_t temp_size)
{
	temp_value->buffer = temp_buffer;
	temp_value->size = temp_size;

	temp_value->head = 0;	//头部是0
	temp_value->tail = 0;	//然后尾部也是0
}
//这里得到下一个index
uint16_t loop_buffer_next_index(uint16_t temp_index,uint16_t temp_size)
{
	temp_index++;
	if(temp_index >= temp_size)
	{
		return 0;
	}
	return temp_index;
}
//环形缓冲区
uint16_t loop_buffer_available(const uart_receive_struct *temp_value)
{
	if(temp_value->head >= temp_value->tail)
	{
		return (temp_value->head - temp_value->tail);
	}
	else
	{
		return (temp_value->size - temp_value->tail + temp_value->head);
	}
}
//环形缓冲区已经有的字节
uint16_t loop_buffer_free(const uart_receive_struct *temp_value)
{
	return temp_value->size - loop_buffer_available(temp_value) - 1;
}
//环形缓冲区写入
uint8_t loop_buffer_push(uart_receive_struct *temp_value,const uint8_t insert_data)
{
	//得到下一个index
	uint16_t push_temp_index = loop_buffer_next_index(temp_value->head,temp_value->size);

	//如果next之后，头和尾相等，那么就不能在环形缓冲区添加
	if(push_temp_index == temp_value->tail)
	{
		return 0;
	}
	else
	{
		temp_value->buffer[temp_value->head] = insert_data;
		temp_value->head = push_temp_index;
		return 1;
	}
}
//环形缓冲区批量写入
uint16_t loop_buffer_push_array(uart_receive_struct *rb, const uint8_t *data, uint16_t len)
{
	uint16_t temp_len = 0;
	for(temp_len = 0;temp_len < len;temp_len++)
	{
		if(loop_buffer_push(rb,data[temp_len]) == 0)
		{
			break;
		}
	}
	return temp_len;
}
//环形缓冲区读取
uint8_t loop_buffer_pop(uart_receive_struct *temp_value, uint8_t *data)
{
	uint16_t push_temp_index = loop_buffer_next_index(temp_value->tail,temp_value->size);

	if(temp_value->tail == temp_value->head)
	{
		return 0;
	}
	else
	{
		*data = temp_value->buffer[temp_value->tail];
		temp_value->tail = push_temp_index;
		return 1;
	}

}
//环形缓冲区批量读取
uint16_t loop_buffer_pop_array(uart_receive_struct *rb, uint8_t *data, uint16_t len)
{
	uint16_t temp_len = 0;

	for(temp_len = 0;temp_len < len;temp_len++)
	{
		if(loop_buffer_pop(rb,&data[temp_len]) == 0)
		{
			break;
		}

	}
	return temp_len;
}


//等待发送完毕
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)		//如果是USART1的话
	{
		usart_send_done = 1;
		//因为不是485，所以我们发送完毕之后没有必要进入这里，这里就先空着
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance  == USART1)
	{
		loop_buffer_push_array(&modbus_recieve_struct,usart_rx_buffer,Size);

		HAL_UARTEx_ReceiveToIdle_DMA(&usart_handle,usart_rx_buffer,tx_rx_buffercount);

		__HAL_DMA_DISABLE_IT(usart_handle.hdmarx, DMA_IT_HT);
	}
}
