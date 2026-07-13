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

//按键参数
key_soft_param key_params[3];


GPIO_InitTypeDef led_gpio_init_struct;

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
    //首先来对按键的参数进行初始化。
    key_params[0].state = key_state_idle;       //空闲状态
    key_params[0].event = key_event_down;       //无事件
    key_params[0].debounce_count = 0;           //消抖次数
    key_params[0].debounce_threshold = 3;       //消抖阈值
    key_params[0].click_count = 0;              //点击次数  
    key_params[0].double_wait_tick = 0;         //二次点击时间
    key_params[0].double_wait_threshold = 50;   //二次点击时间间隔阈值
}

/**
 * 每隔10ms的扫描，扫描当前的
 */
void key_10ms_scan(void)
{
    
}

//状态机事件处理，感觉好麻烦的样子
//事件处理
void handle_event(uint8_t index,key_event_state temp_event)
{
    //根据状态来写处理事件
    switch(key_params[index].state)
    {
        case key_state_idle:
            handle_key_idle(index,temp_event);
            break;
        case key_state_debounce_down:
            handle_key_debounce_down(index,temp_event);
            break;
        case key_state_pressed:
            handle_key_pressed(index,temp_event);
            break;
        case key_state_debounce_up:
            handle_key_debounce_up(index,temp_event);
            break;
        case key_state_wait_doube:
            handle_key_wait_doube(index,temp_event);
            break;        
    }
}

//处理空闲状态
static void handle_key_idle(uint8_t index,key_event_state temp_event)
{
    if(temp_event == key_event_up)      //如果还是抬起状态的话，那么就不需要处理  
    {

    }
    else
    {
    }
}

//处理按下消抖状态
static void handle_key_debounce_down(uint8_t index,key_event_state temp_event)
{

}

//处理按键按下状态
static void handle_key_pressed(uint8_t index,key_event_state temp_event)
{

}

//处理按键释放消抖状态
static void handle_key_debounce_up(uint8_t index,key_event_state temp_event)
{

}

//处理按键等待双击状态
static void handle_key_wait_doube(uint8_t index,key_event_state temp_event)
{


}