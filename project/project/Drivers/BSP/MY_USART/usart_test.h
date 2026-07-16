#ifndef __USART_TEST_H
#define __USART_TEST_H

#include "./SYSTEM/sys/sys.h"

typedef struct 
{
    uint8_t *buffer;
    uint16_t size;
    uint16_t head;
    uint16_t tail;
}uart_receive_struct;

typedef struct
{
	uint8_t *buffer;						//数据缓冲区
	uint16_t size;
	uint16_t head;
	uint16_t tail;	

	uint16_t *buffercount;				//单次的发送数据数量
	uint16_t buffercount_size;
	uint16_t buffercount_head;
	uint16_t buffercount_tail;	
}uart_send_struct;

void usart_test_init(void);
void usart_test_it_init(void);
void usart_test_dma_init(uint32_t Temp_BaudRate);

void modbus_loop_buffer_init(void);
uint8_t modbus_loop_buffer_read(uint8_t *data);
uint16_t modbus_loop_buffer_available(void);
void my_usart_transmit_data(uint8_t *data,uint16_t len);


//-------------------------------------发送FIFO-------------------------------------------------

void uart_send_fifo_init(void);
uint8_t uart_send_data(uint8_t *data, uint16_t len);
uint8_t uart_send_string(char *str);
void uart_send_check(void);

void send_loop_buffer_init(uart_send_struct *temp_value,
													 uint8_t *temp_buffer,
													 uint16_t temp_buffer_size,
													 uint16_t *temp_buffer_count,
													 uint16_t temp_buffercount_size);
uint8_t send_loop_buffer_push(uart_send_struct *temp_value, uint8_t *buffer, uint16_t count);
uint16_t send_loop_buffer_pop(uart_send_struct *temp_value, uint8_t *buffer);


//-------------------------------------接收FIFO-------------------------------------------------

//首先是环形缓冲区的初始化
void loop_buffer_init(uart_receive_struct *temp_value,uint8_t *temp_buffer,uint16_t temp_size);
//这里得到下一个index
uint16_t loop_buffer_next_index(uint16_t temp_index,uint16_t temp_size);
//环形缓冲区
uint16_t loop_buffer_available(const uart_receive_struct *temp_value);
//环形缓冲区已经有的字节
uint16_t loop_buffer_free(const uart_receive_struct *temp_value);
//环形缓冲区写入
uint8_t loop_buffer_push(uart_receive_struct *temp_value,const uint8_t insert_data);
//环形缓冲区批量写入
uint16_t loop_buffer_push_array(uart_receive_struct *rb, const uint8_t *data, uint16_t len);
//环形缓冲区读取
uint8_t loop_buffer_pop(uart_receive_struct *temp_value, uint8_t *data);
//环形缓冲区批量读取
uint16_t loop_buffer_pop_array(uart_receive_struct *rb, uint8_t *data, uint16_t len);

#endif
