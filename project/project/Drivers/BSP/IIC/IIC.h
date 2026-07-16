#ifndef __IIC_H
#define __IIC_H

#include "./SYSTEM/sys/sys.h"

#define AT24C02_SINGLE_WRITE_MAXBYTE 1024-1
#define AT24C02_SINGLE_READ_MAXBYTE 1024

typedef enum 
{
	at24c02_state_none,
	at24c02_state_write,
	at24c02_state_delay,
	at24c02_state_read,
}at24c02_state;

typedef struct
{
	at24c02_state state;
	uint8_t write_buffer[AT24C02_SINGLE_WRITE_MAXBYTE];
	uint8_t read_buffer[AT24C02_SINGLE_READ_MAXBYTE];
}at24c02_data_struct;


void IIC1_Init(void);	//IIC≥ı ºªØ
void IIC1_IT_Init(void);
void IIC1_DMA_Init(void);



#endif
