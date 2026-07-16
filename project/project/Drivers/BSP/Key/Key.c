/**
 ****************************************************************************************************
 * @file        led.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2021-10-14
 * @brief       LED 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 探索者 F407开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20211014
 * 第一次发布
 *
 ****************************************************************************************************
 */
 
#include "./BSP/Key/Key.h"
#include "./BSP/MY_USART/usart_test.h"


#include <stdio.h>


#define MAX_DOUBLE_THRESHOLD 30						//按键双击间隔
#define MAX_SIGNAL_DEBOUNCE_THRESHOLD 3		//消抖间隔
#define KEY_NUMBER 3											//按键数量

GPIO_InitTypeDef led_gpio_init_struct;

//按键参数结构体初始化
key_struct_params key_params[KEY_NUMBER];

static uint8_t get_key_state(uint8_t index);
static uint8_t read_key_down_state(uint8_t index);

static key_click_callback_t key_click_callback = NULL;
static key_click_callback_t key_double_click_callback = NULL;

void key_click_register_task(key_click_callback_t callback)
{
	key_click_callback = callback;
}
void key_double_click_register_task(key_click_callback_t callback)
{
	key_double_click_callback = callback;
}




//初始化一下KEY的GPIO引脚
void key_gpio_init(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();       //打开时钟
    led_gpio_init_struct.Mode = GPIO_MODE_INPUT;    //输入模式
    led_gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2;
    led_gpio_init_struct.Pull = GPIO_PULLUP;
    led_gpio_init_struct.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(GPIOE,&led_gpio_init_struct);
}
/**
 * Key软件参数初始化
 */
void key_soft_init(void)
{
	//KEY0按键	
	key_params[0].key_port = GPIOE;
	key_params[0].key_gpio_pin = GPIO_PIN_4;
	key_params[0].key_activate_power = GPIO_PIN_RESET;		//按下是低电平

	key_params[0].key_current_state = key_state_none;					//当前按键状态
	key_params[0].key_current_pin_state = key_pin_up;	//当前按键的电平状态

	key_params[0].key_debounce_count = 0;				//消抖次数
	key_params[0].key_debounce_threshold = MAX_SIGNAL_DEBOUNCE_THRESHOLD;		//消抖阈值

	key_params[0].key_double_state = 0;					//按键状态，1代表按下一次，0代表按下0次
	key_params[0].key_double_debounce_count = 0;			//双击间隔计数
	key_params[0].key_double_debounce_threhold = MAX_DOUBLE_THRESHOLD;		//双击最大间隔

	key_params[1].key_port = GPIOE;
	key_params[1].key_gpio_pin = GPIO_PIN_3;
	key_params[1].key_activate_power = GPIO_PIN_RESET;		//按下是低电平

	key_params[1].key_current_state = key_state_none;					//当前按键状态
	key_params[1].key_current_pin_state = key_pin_up;	//当前按键的电平状态

	key_params[1].key_debounce_count = 0;				//消抖次数
	key_params[1].key_debounce_threshold = MAX_SIGNAL_DEBOUNCE_THRESHOLD;		//消抖阈值

	key_params[1].key_double_state = 0;					//按键状态，1代表按下一次，0代表按下0次
	key_params[1].key_double_debounce_count = 0;			//双击间隔计数
	key_params[1].key_double_debounce_threhold = MAX_DOUBLE_THRESHOLD;		//双击最大间隔

}

/*
	获得当前按下状态，1代表按下，0代表释放，-1代表错误
*/
static uint8_t read_key_down_state(uint8_t index)
{
	uint8_t temp_state = HAL_GPIO_ReadPin(key_params[index].key_port,
																				key_params[index].key_gpio_pin);
	if(key_params[index].key_activate_power == GPIO_PIN_RESET)		//如果按下之后是低电平
	{
		if(temp_state == GPIO_PIN_RESET)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else			//如果按下之后是高电平
	{
		if(temp_state == GPIO_PIN_RESET)	
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
}
/*
	在定时函数中，10ms扫描一次
*/
void bsp_key_10ms_scan(void)
{
	uint8_t my_key_state = 0;
	uint8_t temp_index = 0;

	for(temp_index =0;temp_index<KEY_NUMBER;temp_index++)
	{
		my_key_state = 0;
		my_key_state = get_key_state(temp_index);

		//单击事件
		if(my_key_state & key_state_click)
		{
			if(key_click_callback != NULL)
			{
				key_click_callback(temp_index);
			}
			key_params[temp_index].key_current_state &= ~key_state_click;
		}
		//双击事件
		if(my_key_state & key_state_double_click)
		{
			if(key_double_click_callback != NULL)
			{
				key_double_click_callback(temp_index);
			}
			key_params[temp_index].key_current_state &= ~key_state_double_click;
		}

		if(my_key_state & key_state_down) //按下
		{
			key_params[temp_index].key_current_state &= ~key_state_down;
		}
		if(my_key_state & key_state_up)	//弹起
		{
			key_params[temp_index].key_current_state &= ~key_state_up;
		}
	}
}
/*
	更新按键状态
*/
static uint8_t get_key_state(uint8_t index)
{

	uint8_t temp_state = 0;

	temp_state = read_key_down_state(index);

	if(temp_state == 1)		//如果按下了
	{
		if(key_params[index].key_current_pin_state == key_pin_up)	//如果之前状态是抬起状态的话
		{

			key_params[index].key_debounce_count++;


			if(key_params[index].key_debounce_count >=
			   key_params[index].key_debounce_threshold)		//等待稳定
			{
				key_params[index].key_debounce_count = 0;

				//更新稳定状态
				key_params[index].key_current_pin_state = key_pin_down;	//稳定后，将其设置为按下状态

				//产生按下事件
				key_params[index].key_current_state |= key_state_down;	//按下事件
			}
		}
		else	//如果之前状态是按下状态的话
		{
			key_params[index].key_debounce_count = 0;		//继续清空数据
		}
	}
	else	//如果当前按键电平是释放状态
	{
		if(key_params[index].key_current_pin_state == key_pin_down)	//如果之前状态是按下
		{

			key_params[index].key_debounce_count++;


			if(key_params[index].key_debounce_count >=
			   key_params[index].key_debounce_threshold)	//等待稳定
			{
				key_params[index].key_debounce_count = 0;
				//更新稳定状态
				key_params[index].key_current_pin_state = key_pin_up;	//变成抬起状态
				key_params[index].key_current_state |= key_state_up;	//发生抬起事件
				//如果
				if(key_params[index].key_double_state == 0)
				{
					//第一次释放
					key_params[index].key_double_state = 1;
					key_params[index].key_double_debounce_count = 0;

				}
				else if(key_params[index].key_double_state == 1)
				{
					//第二次释放
					key_params[index].key_double_state = 2;
				}
			}
		}
		else		//如果之前状态就是抬起
		{
			key_params[index].key_debounce_count = 0;
		}
	}


	if(key_params[index].key_double_state == 1)	//单击计时
	{
		if(key_params[index].key_current_pin_state == key_pin_up)	//如果之前状态为抬起
		{
			key_params[index].key_double_debounce_count++;
			if(key_params[index].key_double_debounce_count >=
			   key_params[index].key_double_debounce_threhold)		//等待超时
			{
				key_params[index].key_double_state = 0;
				key_params[index].key_double_debounce_count = 0;
				//单击
				key_params[index].key_current_state |= key_state_click;
			}
		}
	}
	else if(key_params[index].key_double_state == 2)	//双击确认
	{
		key_params[index].key_double_state = 0;
		key_params[index].key_double_debounce_count = 0;
		key_params[index].key_current_state |= key_state_double_click;
	}

	return key_params[index].key_current_state;

}
