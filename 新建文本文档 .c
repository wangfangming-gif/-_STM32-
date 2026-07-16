#include "at24c02.h"
#include "string.h"

extern I2C_HandleTypeDef i2c1_handle;

#define AT24C02_I2C_HANDLE          (&i2c1_handle)
#define AT24C02_DEV_ADDR            0xA0
#define AT24C02_TOTAL_SIZE          256
#define AT24C02_PAGE_SIZE           8
#define AT24C02_WRITE_WAIT_MS       6

typedef enum
{
	AT24C02_STATE_IDLE = 0,
	AT24C02_STATE_WRITE_TX,
	AT24C02_STATE_WRITE_WAIT,
	AT24C02_STATE_READ_RX,
	AT24C02_STATE_ERROR
}AT24C02_StateEnum;

static volatile AT24C02_StateEnum at24c02_state = AT24C02_STATE_IDLE;

static uint8_t at24c02_write_cache[AT24C02_TOTAL_SIZE];
static uint8_t at24c02_tx_buffer[AT24C02_PAGE_SIZE + 1];

static uint8_t at24c02_write_addr = 0;
static uint16_t at24c02_write_len = 0;
static uint16_t at24c02_write_offset = 0;
static uint32_t at24c02_write_wait_tick = 0;

static uint8_t *at24c02_read_buffer = 0;
static uint8_t at24c02_read_addr = 0;
static uint16_t at24c02_read_len = 0;

static uint8_t AT24C02_StartWritePage(void)
{
	uint8_t now_addr;
	uint16_t remain_len;
	uint16_t page_remain;
	uint16_t send_len;
	uint16_t i;

	now_addr = at24c02_write_addr + at24c02_write_offset;
	remain_len = at24c02_write_len - at24c02_write_offset;
	page_remain = AT24C02_PAGE_SIZE - (now_addr % AT24C02_PAGE_SIZE);

	if(remain_len > page_remain)
	{
		send_len = page_remain;
	}
	else
	{
		send_len = remain_len;
	}

	at24c02_tx_buffer[0] = now_addr;

	for(i = 0; i < send_len; i++)
	{
		at24c02_tx_buffer[i + 1] = at24c02_write_cache[at24c02_write_offset + i];
	}

	at24c02_state = AT24C02_STATE_WRITE_TX;

	if(HAL_I2C_Master_Transmit_DMA(AT24C02_I2C_HANDLE, AT24C02_DEV_ADDR, at24c02_tx_buffer, send_len + 1) != HAL_OK)
	{
		at24c02_state = AT24C02_STATE_ERROR;
		return AT24C02_ERROR;
	}

	at24c02_write_offset += send_len;

	return AT24C02_OK;
}

void AT24C02_Init(void)
{
	at24c02_state = AT24C02_STATE_IDLE;
	at24c02_write_addr = 0;
	at24c02_write_len = 0;
	at24c02_write_offset = 0;
	at24c02_write_wait_tick = 0;
	at24c02_read_buffer = 0;
	at24c02_read_addr = 0;
	at24c02_read_len = 0;
}

uint8_t AT24C02_IsBusy(void)
{
	if(at24c02_state == AT24C02_STATE_IDLE)
	{
		return 0;
	}

	return 1;
}

uint8_t AT24C02_Write(uint8_t addr, uint8_t *data, uint16_t len)
{
	if(data == 0 || len == 0)
	{
		return AT24C02_PARAM_ERROR;
	}

	if((addr + len) > AT24C02_TOTAL_SIZE)
	{
		return AT24C02_PARAM_ERROR;
	}

	if(at24c02_state != AT24C02_STATE_IDLE)
	{
		return AT24C02_BUSY;
	}

	memcpy(at24c02_write_cache, data, len);

	at24c02_write_addr = addr;
	at24c02_write_len = len;
	at24c02_write_offset = 0;

	return AT24C02_StartWritePage();
}

uint8_t AT24C02_Read(uint8_t addr, uint8_t *data, uint16_t len)
{
	if(data == 0 || len == 0)
	{
		return AT24C02_PARAM_ERROR;
	}

	if((addr + len) > AT24C02_TOTAL_SIZE)
	{
		return AT24C02_PARAM_ERROR;
	}

	if(at24c02_state != AT24C02_STATE_IDLE)
	{
		return AT24C02_BUSY;
	}

	at24c02_read_addr = addr;
	at24c02_read_buffer = data;
	at24c02_read_len = len;
	at24c02_state = AT24C02_STATE_READ_RX;

	if(HAL_I2C_Mem_Read_DMA(AT24C02_I2C_HANDLE,
													AT24C02_DEV_ADDR,
													at24c02_read_addr,
													I2C_MEMADD_SIZE_8BIT,
													at24c02_read_buffer,
													at24c02_read_len) != HAL_OK)
	{
		at24c02_state = AT24C02_STATE_ERROR;
		return AT24C02_ERROR;
	}

	return AT24C02_OK;
}

void AT24C02_Process(void)
{
	if(at24c02_state == AT24C02_STATE_WRITE_WAIT)
	{
		if((HAL_GetTick() - at24c02_write_wait_tick) >= AT24C02_WRITE_WAIT_MS)
		{
			if(at24c02_write_offset >= at24c02_write_len)
			{
				at24c02_state = AT24C02_STATE_IDLE;
				AT24C02_WriteCpltCallback(AT24C02_OK);
			}
			else
			{
				if(AT24C02_StartWritePage() != AT24C02_OK)
				{
					at24c02_state = AT24C02_STATE_IDLE;
					AT24C02_WriteCpltCallback(AT24C02_ERROR);
				}
			}
		}
	}
}

void AT24C02_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c != AT24C02_I2C_HANDLE)
	{
		return;
	}

	if(at24c02_state == AT24C02_STATE_WRITE_TX)
	{
		at24c02_write_wait_tick = HAL_GetTick();
		at24c02_state = AT24C02_STATE_WRITE_WAIT;
	}
}

void AT24C02_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c != AT24C02_I2C_HANDLE)
	{
		return;
	}

	if(at24c02_state == AT24C02_STATE_READ_RX)
	{
		at24c02_state = AT24C02_STATE_IDLE;
		AT24C02_ReadCpltCallback(AT24C02_OK);
	}
}

void AT24C02_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c != AT24C02_I2C_HANDLE)
	{
		return;
	}

	if(at24c02_state == AT24C02_STATE_WRITE_TX || at24c02_state == AT24C02_STATE_WRITE_WAIT)
	{
		at24c02_state = AT24C02_STATE_IDLE;
		AT24C02_WriteCpltCallback(AT24C02_ERROR);
	}
	else if(at24c02_state == AT24C02_STATE_READ_RX)
	{
		at24c02_state = AT24C02_STATE_IDLE;
		AT24C02_ReadCpltCallback(AT24C02_ERROR);
	}
	else
	{
		at24c02_state = AT24C02_STATE_IDLE;
	}
}

__weak void AT24C02_WriteCpltCallback(uint8_t result)
{
}

__weak void AT24C02_ReadCpltCallback(uint8_t result)
{
}