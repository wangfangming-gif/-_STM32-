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

typedef enum 
{
    key_state_idle,           //空闲状态
    key_state_debounce_down,  //按下消抖状态
    key_state_pressed,        //按下状态
    key_state_debounce_up,    //释放消抖状态
    key_state_wait_doube,     //等待双击
}key_state;

typedef enum 
{
    key_event_down,     //按下事件
    key_event_up,       //抬起事件
    key_event_click,    //点击事件
    key_event_double_click,     //双击事件
}key_event_state;

typedef struct key_soft_param_struct
{
    key_state state;           //按键状态
    key_event_state event;     //按键事件

    uint16_t debounce_count;        //消抖次数
    uint16_t debounce_threshold;    //消抖阈值
    
    uint32_t click_count;           //点击次数
    uint16_t double_wait_tick;      //双击等待事件
    uint16_t double_wait_threshold; //双击触发阈值
}key_soft_param;


void key_gpio_init(void);
void key_soft_init(void);
void key_10ms_scan(void);


#endif
