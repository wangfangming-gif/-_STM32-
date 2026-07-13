#include "./BSP/NorFlash/NorFlash.h"
#include "./BSP/SPI/SPI.h"

// CS使能和失能
static void norflash_enable()
{
    // 使能CS
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
}

static void norflash_disable()
{
    // 失能CS
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

void norflash_init()
{
    SPI1_Init();
    norflash_disable();
    spi1_set_speed(1);
}

uint32_t norflash_read_manu_id()
{
    uint32_t manu_id;

    norflash_enable();
    spi1_read_write_data(0x9F);
    manu_id = spi1_read_write_data(0xFF);
    manu_id <<= 8;
    manu_id |= spi1_read_write_data(0xFF);
    manu_id <<= 8;
    manu_id |= spi1_read_write_data(0xFF);
    norflash_disable();

    return manu_id;
}

uint16_t norflash_read_device_id()
{
    uint16_t device_id;

    norflash_enable();
    spi1_read_write_data(0x90);
    spi1_read_write_data(0x00);
    spi1_read_write_data(0x00);
    spi1_read_write_data(0x00);

    device_id = spi1_read_write_data(0xFF);
    device_id <<= 8;
    device_id |= spi1_read_write_data(0xFF);
    norflash_disable();

    return device_id;
}

void norflash_wait_unbusy()
{
    uint8_t read_state = 0;
    while (1)
    {
        norflash_enable();
        spi1_read_write_data(0x05);
        read_state = spi1_read_write_data(0xFF);
        norflash_disable();
        if ((read_state & 0x01) == 0x00) // 空闲状态
        {
            break;
        }
        read_state = 0;
    }
    return;
}

void Erase_one_sector(uint32_t address)
{
    norflash_write_enable(); // 写使能
    norflash_wait_unbusy();

    norflash_enable();
    spi1_read_write_data(0x20);
    spi1_read_write_data((uint8_t)((address) >> 16));
    spi1_read_write_data((uint8_t)((address) >> 8));
    spi1_read_write_data((uint8_t)(address));
    norflash_disable();

    norflash_wait_unbusy();
}

void Erase_write_data_sector(uint32_t address,
                             uint32_t write_number)
{
    uint32_t start_address;
    uint32_t end_address;
    uint32_t sector_number;
    uint16_t temp_number = 0;

    start_address = address / 4096;
    end_address = (address + write_number) / 4096;

    sector_number = end_address - start_address+1;

    for (temp_number = 0; temp_number < sector_number; temp_number++)
    {
        Erase_one_sector(address);
        address += 4096;
    }
}

void norflash_write_enable()
{
    norflash_enable();
    spi1_read_write_data(0x06);
    norflash_disable();
}

/**
 * 这里的作用是在这个地址写入一些数据，不过这里没有做扇区的检查、页的检查，所以
 * 写入的数据最好小于4096
 */
void norflash_write_address_data(uint32_t address, uint8_t *p_data, uint32_t write_number)
{
    uint32_t Temp_Number = 0;

    norflash_write_enable(); // 打开写使能

    norflash_enable();
    spi1_read_write_data(0x02);
    spi1_read_write_data((uint8_t)((address) >> 16));
    spi1_read_write_data((uint8_t)((address) >> 8));
    spi1_read_write_data((uint8_t)(address));

    while (Temp_Number < write_number)
    {
        spi1_read_write_data(p_data[Temp_Number]);
        Temp_Number++;
    }

    norflash_disable(); // 失能spi器件

    norflash_wait_unbusy();
}

// 这里的作用是从这个地址中将数据读出来
void norflash_read_address_data(uint32_t address, uint8_t *p_data, uint32_t read_number)
{
    uint32_t Temp_Number = 0;

    norflash_enable();
    spi1_read_write_data(0x03);
    spi1_read_write_data((uint8_t)((address) >> 16));
    spi1_read_write_data((uint8_t)((address) >> 8));
    spi1_read_write_data((uint8_t)(address));

    while (Temp_Number < read_number)
    {
        p_data[Temp_Number] = spi1_read_write_data(0xFF);
        Temp_Number++;
    }

    norflash_disable();
}
