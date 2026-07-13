#ifndef __LIGHT_H
#define __LIGHT_H

void Light_Init(void);
void Light_TimerCallback(void);
void Light_AdcDmaCpltCallback(void);
void Light_Handle(void);

#endif
