#include "./BSP/MY_WWDG/wwdg.h"
#include "./BSP/LED/led.h"

//窗口看门狗处理权柄
WWDG_HandleTypeDef wwdg_handle;
  
//窗口看门狗初始化
void wwdg_init(void)
{
	//使能时钟
	__HAL_RCC_WWDG_CLK_ENABLE();

	wwdg_handle.Instance = WWDG;
	wwdg_handle.Init.Counter = 0x7F;
	wwdg_handle.Init.EWIMode = WWDG_EWI_ENABLE;
	wwdg_handle.Init.Prescaler = WWDG_PRESCALER_8;
	wwdg_handle.Init.Window = 0x50;
	
	HAL_WWDG_Init(&wwdg_handle);
}

//进行初始化
void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg)
{
	//如果是WWDG的话，因为前面开启了中断，所以这里我需要开启一下NVIC
	if(hwwdg->Instance == WWDG)
	{
		__HAL_RCC_WWDG_CLK_ENABLE();

		HAL_NVIC_SetPriority(WWDG_IRQn,0,1);
		HAL_NVIC_EnableIRQ(WWDG_IRQn);
	}
}


//接下来是中断服务函数
void WWDG_IRQHandler(void)
{
	HAL_WWDG_IRQHandler(&wwdg_handle);
}


//最后是回调函数，其实只要进入到HAL_WWDG_IRQHandler当中，就会自动刷新
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg)
{
	static uint16_t count = 0;
	//窗口看门狗，怎么处理呢？
	if(hwwdg->Instance == WWDG)
	{
		HAL_WWDG_Refresh(&wwdg_handle);
		//这里不需要再处理什么东西了
		count++;
		if(count >= 15)
		{
			LED0_TOGGLE();
			LED1_TOGGLE();
			count = 0;
		}
	}
}
