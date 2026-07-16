#include "./BSP/MY_USART/Modbus.h"
#include "./BSP/MY_USART/usart_test.h"


uint8_t modbus_handle_buffer[128];
uint16_t modbus_receive_count;
uint8_t modbus_receive_flag_done;

void modbus_init(void)
{
	modbus_loop_buffer_init();
	//硬件上也初始化一下
	usart_test_dma_init(9600);
	uart_send_fifo_init();
}

void modbus_handle(void)
{
	uint16_t available_buffer_size;
	uint16_t temp_count;

	available_buffer_size = modbus_loop_buffer_available();
	modbus_receive_count = available_buffer_size;
	if(available_buffer_size != 0)
	{
		modbus_receive_flag_done = 1;
		for(temp_count = 0;temp_count < available_buffer_size;temp_count++)
		{
			modbus_loop_buffer_read(&modbus_handle_buffer[temp_count]);
		}
		uart_send_data(modbus_handle_buffer,modbus_receive_count);
//		my_usart_transmit_data(modbus_handle_buffer,modbus_receive_count);
	}
	else
	{

	}
	modbus_receive_flag_done = 0;
	modbus_receive_count = 0;
}

