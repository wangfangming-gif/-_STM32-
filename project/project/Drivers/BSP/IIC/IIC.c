#include "./BSP/IIC/IIC.h"

#define IIC1_Address1 0x21
#define IIC1_Address2 0x22

I2C_HandleTypeDef IIC1_handle;
DMA_HandleTypeDef IIC1_TX_DMAHandle;
DMA_HandleTypeDef IIC1_RX_DMAHandle;

extern uint8_t Flag_IIC_SendDone;
extern uint8_t Flag_IIC_ReadDone;

static at24c02_data_struct at24c02_params;

void IIC1_Init(void)
{
	__HAL_RCC_I2C1_CLK_ENABLE();
	IIC1_handle.Instance = I2C1;
	IIC1_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;//7位地址
	IIC1_handle.Init.ClockSpeed = 100000;//SCL时钟
	IIC1_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	//是否双地址模式吧?
	IIC1_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;	//占空比，应该是高电平和低电平的占空比，不过这里用不上
	IIC1_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;	//广播功能
	IIC1_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;	//时钟延长
	IIC1_handle.Init.OwnAddress1 = IIC1_Address1;
	IIC1_handle.Init.OwnAddress2 = IIC1_Address2;
	HAL_I2C_Init(&IIC1_handle);
	
}

void IIC1_IT_Init(void)
{
	__HAL_RCC_I2C1_CLK_ENABLE();
	IIC1_handle.Instance = I2C1;
	IIC1_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;//7位地址
	IIC1_handle.Init.ClockSpeed = 100000;//SCL时钟
	IIC1_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	//是否双地址模式吧?
	IIC1_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;	//占空比，应该是高电平和低电平的占空比，不过这里用不上
	IIC1_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;	//广播功能
	IIC1_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;	//时钟延长
	IIC1_handle.Init.OwnAddress1 = IIC1_Address1;
	IIC1_handle.Init.OwnAddress2 = IIC1_Address2;
	HAL_I2C_Init(&IIC1_handle);

	HAL_NVIC_SetPriority(I2C1_EV_IRQn,1,0);
	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);


}

void IIC1_DMA_Init(void)
{
	__HAL_RCC_I2C1_CLK_ENABLE();
	IIC1_handle.Instance = I2C1;
	IIC1_handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;//7位地址
	IIC1_handle.Init.ClockSpeed = 100000;//SCL时钟
	IIC1_handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	//是否双地址模式吧?
	IIC1_handle.Init.DutyCycle = I2C_DUTYCYCLE_2;	//占空比，应该是高电平和低电平的占空比，不过这里用不上
	IIC1_handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;	//广播功能
	IIC1_handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;	//时钟延长
	IIC1_handle.Init.OwnAddress1 = IIC1_Address1;
	IIC1_handle.Init.OwnAddress2 = IIC1_Address2;
	HAL_I2C_Init(&IIC1_handle);

	__HAL_I2C_ENABLE(&IIC1_handle);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	if(hi2c->Instance == I2C1)	//如果是IIC1的话
	{
		__HAL_RCC_I2C1_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_DMA1_CLK_ENABLE();

		// GPIO全部初始化完成
	  GPIO_InitTypeDef iic_gpio_init_struct;
	  iic_gpio_init_struct.Alternate = GPIO_AF4_I2C1;
	  iic_gpio_init_struct.Mode = GPIO_MODE_AF_OD;
	  iic_gpio_init_struct.Pin = GPIO_PIN_8;
	  iic_gpio_init_struct.Pull = GPIO_PULLUP;
	  iic_gpio_init_struct.Speed = GPIO_SPEED_FAST;

	  HAL_GPIO_Init(GPIOB,&iic_gpio_init_struct);

	  iic_gpio_init_struct.Pin = GPIO_PIN_9;
	  HAL_GPIO_Init(GPIOB,&iic_gpio_init_struct);

	  /*
	  	接下来一部分用于使用DMA
	  */
	  IIC1_TX_DMAHandle.Instance = DMA1_Stream6;	 
	  IIC1_TX_DMAHandle.Init.Channel = DMA_CHANNEL_1;
	  IIC1_TX_DMAHandle.Init.Direction = DMA_MEMORY_TO_PERIPH;
	  IIC1_TX_DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	  IIC1_TX_DMAHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	  IIC1_TX_DMAHandle.Init.MemBurst = DMA_MBURST_SINGLE;
	  IIC1_TX_DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	  IIC1_TX_DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
	  IIC1_TX_DMAHandle.Init.Mode = DMA_NORMAL;
	  IIC1_TX_DMAHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;
	  IIC1_TX_DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	  IIC1_TX_DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	  IIC1_TX_DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
	  
	  HAL_DMA_Init(&IIC1_TX_DMAHandle);

	  __HAL_LINKDMA(&IIC1_handle,hdmatx,IIC1_TX_DMAHandle);
	  
	  IIC1_RX_DMAHandle.Instance = DMA1_Stream5;	 
	  IIC1_RX_DMAHandle.Init.Channel = DMA_CHANNEL_1;
	  IIC1_RX_DMAHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	  IIC1_RX_DMAHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	  IIC1_RX_DMAHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	  IIC1_RX_DMAHandle.Init.MemBurst = DMA_MBURST_SINGLE;
	  IIC1_RX_DMAHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	  IIC1_RX_DMAHandle.Init.MemInc = DMA_MINC_ENABLE;
	  IIC1_RX_DMAHandle.Init.Mode = DMA_NORMAL;
	  IIC1_RX_DMAHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;
	  IIC1_RX_DMAHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	  IIC1_RX_DMAHandle.Init.PeriphInc = DMA_PINC_DISABLE;
	  IIC1_RX_DMAHandle.Init.Priority = DMA_PRIORITY_MEDIUM;
	  
	  HAL_DMA_Init(&IIC1_RX_DMAHandle);

	  __HAL_LINKDMA(&IIC1_handle,hdmarx,IIC1_RX_DMAHandle);


		//发送
		HAL_NVIC_SetPriority(DMA1_Stream6_IRQn,2,1);
		HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
		//接收
		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn,2,0);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
		//IIC
		HAL_NVIC_SetPriority(I2C1_EV_IRQn,1,0);
		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);	  

			//IIC错误中断
	HAL_NVIC_SetPriority(I2C1_ER_IRQn,1,1);
	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
	}
}

uint8_t AT24C02_Write_Data(uint16_t DeviceAddress,
																	uint16_t Registeraddress,
																	uint8_t *Data,
																	uint16_t len)
{
	if((len > AT24C02_SINGLE_WRITE_MAXBYTE) || (at24c02_params.state))
	{

	}
}

uint8_t AT24C02_Read_Data()
{

}


void DMA1_Stream6_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&IIC1_TX_DMAHandle);
}

void DMA1_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&IIC1_RX_DMAHandle);
}

void I2C1_EV_IRQHandler(void)
{
	//补充一下中断服务函数
	HAL_I2C_EV_IRQHandler(&IIC1_handle);
}

 void I2C1_ER_IRQHandler(void)
 {
	 HAL_I2C_ER_IRQHandler(&IIC1_handle);
 }

 void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
 {
	 if(hi2c->Instance == I2C1)
	 {
		 Flag_IIC_SendDone = 1;
		 Flag_IIC_ReadDone = 1;
	 }
 }


 //发送完毕回调函数
 void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
 {
 	//如果是IIC1的话
 	if(hi2c->Instance == I2C1)
 	{
 		Flag_IIC_SendDone = 1;
 	}
 }
 
// //接收完毕回调函数
// void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
// {
// 	//如果是IIC1的话
// 	if(hi2c->Instance == I2C1)
// 	{
// 		Flag_IIC_ReadDone = 1;
// 	}
// }

//void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
//{
// 	//如果是IIC1的话
// 	if(hi2c->Instance == I2C1)
// 	{
// 		Flag_IIC_SendDone = 1;
// 	}
//}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
 	//如果是IIC1的话
 	if(hi2c->Instance == I2C1)
 	{
 		Flag_IIC_ReadDone = 1;
 	}
}




