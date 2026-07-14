/**
 ****************************************************************************************************
 * @file        led.h
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
#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"

//单击回调函数
typedef void (*key_click_callback_t)(uint8_t key_id);
//双击回调函数
typedef void (*key_double_click_callback_t)(uint8_t key_id);

typedef enum
{
	KEY0 = 0,
	KEY1 = 1,
	KEY2 = 2,
}MYKEY;

typedef enum 
{
	key_state_none = 1,		//无
	key_state_down = 1<<1,		//按下
	key_state_up = 1<<2,			//弹起
	key_state_click = 1<<3,	//点击
	key_state_double_click = 1<<4,		//双击
}key_state;

typedef enum
{
	key_pin_down,
	key_pin_up,
}key_pin_state;

typedef struct
{
	GPIO_TypeDef * key_port;
	uint16_t key_gpio_pin;
	uint8_t key_gpio_state;
	uint8_t key_activate_power;

	uint8_t key_current_state;					//当前按键状态
	key_pin_state key_current_pin_state;	//当前按键的电平状态
	
	uint8_t key_debounce_count;				//消抖次数
	uint8_t key_debounce_threshold;		//消抖阈值

	uint8_t key_double_state;					//按键状态，1代表按下一次，0代表按下0次
	uint8_t key_double_debounce_count;			//双击间隔计数
	uint8_t key_double_debounce_threhold;		//双击最大间隔
}key_struct_params;


void key_click_register_task(key_click_callback_t callback);
void key_double_click_register_task(key_click_callback_t callback);



void key_gpio_init(void);
void key_soft_init(void);
void bsp_key_10ms_scan(void);


#endif
