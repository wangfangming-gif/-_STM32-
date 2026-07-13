#ifndef __NORFLASH_H
#define __NORFLASH_H

#include "./SYSTEM/sys/sys.h"

void norflash_init(void);
uint16_t norflash_read_device_id(void);
uint32_t norflash_read_manu_id(void);


void norflash_write_enable(void);
void norflash_write_address_data(uint32_t address, uint8_t *p_data, uint32_t write_number);
//这里的作用是从这个地址中将数据读出来
void norflash_read_address_data(uint32_t address, uint8_t *p_data, uint32_t read_number);
void Erase_write_data_sector(uint32_t address,
                             uint32_t write_number);

#endif

