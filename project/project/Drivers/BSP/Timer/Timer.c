#include "./BSP/Timer/Timer.h"
#include "./BSP/MY_USART/usart_test.h"

extern volatile uint8_t flag_timer6_done;
TIM_HandleTypeDef timer6_handle;
extern volatile uint8_t DHT11_Capture_Done;      //捕获完成

#define TIMER6_TASK_MAX 10	//最高10个任务

typedef struct 
{
	uint16_t period_ticks;
	uint16_t counter;
	timer6_callback_t callback;
	uint8_t enable;
}timer6_task_t;

static timer6_task_t timer6_task[TIMER6_TASK_MAX];

//基本定时器初始化
void Timer6_Init(void)
{
	//
	__HAL_RCC_TIM6_CLK_ENABLE();

	timer6_handle.Instance = TIM6;
	timer6_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;	//自动重载
	timer6_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;	//4分频
	timer6_handle.Init.CounterMode = TIM_COUNTERMODE_UP;		//向上计数模式
	timer6_handle.Init.Period = 100-1;
	timer6_handle.Init.Prescaler = 8400-1;		
	timer6_handle.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&timer6_handle);

	HAL_NVIC_SetPriority(TIM6_DAC_IRQn,1,0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	HAL_TIM_Base_Start_IT(&timer6_handle);
}

/**
 * 定时器6任务注册
 */
uint8_t Timer6_RegisterTask(timer6_callback_t callback,uint16_t period_ticks)
{
	uint8_t i;

	if((callback == 0) || (period_ticks == 0))
	{
		return 0;
	}

	for(i=0;i<TIMER6_TASK_MAX;i++)
	{
		if(timer6_task[i].enable == 0)
		{
			timer6_task[i].period_ticks = period_ticks;
			timer6_task[i].counter = 0;
			timer6_task[i].callback = callback;
			timer6_task[i].enable = 1;
			return i;
		}
	}

	return 0;
}

/**
 * 注销定时器6的任务
 */
void Timer6_UnRegisterTask(uint8_t task_id)
{
	//如果大于最大的任务数的话，退出
	if(task_id >= TIMER6_TASK_MAX)
	{
		return;
	}

	//注销其实就是把所有的东西全部清除掉
	timer6_task[task_id].enable = 0;
	timer6_task[task_id].callback = 0;
	timer6_task[task_id].counter = 0;
	timer6_task[task_id].period_ticks = 0;
}

//暂停任务
void Timer6_PauseTask(uint8_t task_id)
{
	if(task_id >= TIMER6_TASK_MAX)
	{
		return;
	}

	timer6_task[task_id].enable = 0;
}

void Timer6_ResumeTask(uint8_t task_id)
{
	if(task_id >= TIMER6_TASK_MAX)
	{
		return;
	}
	if(timer6_task[task_id].callback != 0)
	{
		timer6_task[task_id].enable = 1;
	}
}

//中断服务函数
void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&timer6_handle);
}

//回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint8_t i;
	// static uint16_t temp_tim6_count = 0;
	if(htim->Instance == TIM6)	//如果是定时器6的话
	{
		// temp_tim6_count++;
		// if(temp_tim6_count >= 200)	//2秒一次
		// {
		// 	temp_tim6_count = 0;
		// 	flag_timer6_done = 1;
		// }

		for(i=0;i<TIMER6_TASK_MAX;i++)
		{
			if(timer6_task[i].enable == 0)
			{
				continue;
			}

			timer6_task[i].counter++;
			if(timer6_task[i].counter >= timer6_task[i].period_ticks)
			{
				timer6_task[i].counter = 0;
				if(timer6_task[i].callback != 0)
				{
					//运行回调函数
					timer6_task[i].callback();
				}
			}
		}
	}
	else if(htim->Instance == TIM9)
	{
		if(DHT11_Capture_Done == 0)
        {
            DHT11_Capture_Done = 1;
        }
	}
}
