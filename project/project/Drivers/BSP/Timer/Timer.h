#ifndef __MY_TIMER_H
#define __MY_TIMER_H

#include "./SYSTEM/sys/sys.h"

typedef void (*timer6_callback_t)(void);

void Timer6_Init(void);
uint8_t Timer6_RegisterTask(timer6_callback_t callback,uint16_t period_ticks);
void Timer6_UnRegisterTask(uint8_t task_id);    //注销任务
void Timer6_PauseTask(uint8_t task_id);     //暂停任务
void Timer6_ResumeTask(uint8_t task_id);    //恢复任务

#endif
