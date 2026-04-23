#include <stdint.h>
#include <string.h>
#include "board.h"
#include "ch32v30x.h"
#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_gpio.h"
#include "Internal_Flash.h"
#include "ch32v30x_flash.h"
#include "uf2.h"

#define BOOT_GPIO_APB       RCC_APB2Periph_GPIOC
#define BOOT_GPIO_Port      GPIOC
#define BOOT_GPIO_Pin       GPIO_Pin_7

#define NO_CACHE        0xffffffff
#define SECTOR_SIZE     (256) // ch32v307 sector size
static uint32_t _flash_page_addr = NO_CACHE;
static uint8_t  _flash_cache[SECTOR_SIZE];

void uf2_pin_init()
{
    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd( BOOT_GPIO_APB, ENABLE );
    GPIO_InitStructure.GPIO_Pin = BOOT_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init( BOOT_GPIO_Port, &GPIO_InitStructure );
}

uint32_t uf2_board_flash_size()
{
    return FLASH_SIZE;
}

void uf2_board_flash_read(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    memcpy((uint8_t*)buffer, (uint8_t*)addr, len);
}

void uf2_board_flash_write(uint32_t addr, uint8_t *src, uint32_t len)
{
    if ( addr < IFLASH_UDISK_START_ADDR + IFLASH_START_ADDR || \
        (addr + len) > IFLASH_UDISK_END_ADDR + IFLASH_START_ADDR )
    {
        printf("uf2_board_flash Error Address 0x%08x\r\n", addr);
        return;
    }

    uint32_t const page_addr = addr & ~(SECTOR_SIZE - 1);

    if (page_addr != _flash_page_addr)
    {
        /* Write out anything in cache before overwriting it. */
        uf2_board_flash_flush();

        _flash_page_addr = page_addr;

        /* Copy the current contents of the entire page into the cache. */
        memcpy(_flash_cache, (void *) page_addr, SECTOR_SIZE);
    }

    printf("uf2_flash Addr 0x%08x\r\n", addr);

    /* Overwrite part or all of the page cache with the src data. */
    memcpy(_flash_cache + (addr & (SECTOR_SIZE - 1)), src, len);

}

void uf2_board_flash_flush()
{
    if (_flash_page_addr == NO_CACHE)
    {
        return;
    }

    if (memcmp(_flash_cache, (void *) _flash_page_addr, SECTOR_SIZE) != 0)
    {
        //printf("uf2_flash_flush Address 0x%08x\r\n", _flash_page_addr);
        __disable_irq();
        FLASH_Unlock_Fast();
        FLASH_ErasePage_Fast(_flash_page_addr);
        FLASH_ProgramPage_Fast(_flash_page_addr, (uint32_t*)_flash_cache);
        FLASH_Lock_Fast();
        __enable_irq();
    }

    _flash_page_addr = NO_CACHE;
}

void uf2_board_dfu_complete()
{
    printf("*** uf2_board_dfu_complete ***\r\n\r\n");

    uf2_board_app_jump();
}

void uf2_board_app_jump()
{
    FLASH_Unlock_Fast();
    FLASH_ProgramWord(CalAddr, MAGIC_APP);
    FLASH->CTLR |= ((uint32_t)0x00008000);
    FLASH->CTLR |= ((uint32_t)0x00000080);

    USBHS_Device_Init( DISABLE );
    NVIC_DisableIRQ( USBHS_IRQn );
    Delay_Ms(50);
    printf("jump APP\r\n");

    GPIO_DeInit(BOOT_GPIO_Port);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_USBHS, DISABLE);

    Delay_Ms(100);
    NVIC_EnableIRQ(Software_IRQn);
    NVIC_SetPendingIRQ(Software_IRQn);
}

uint8_t uf2_run_app()
{
    uint8_t ret = 1;
    if(GPIO_ReadInputDataBit(BOOT_GPIO_Port, BOOT_GPIO_Pin) == 0)
    {
        ret = 0;
    }
    else if(*(uint32_t*)BOARD_FLASH_APP_START  != 0xe339e339 )
    {
        if(*(uint32_t*)CalAddr == MAGIC_APP)
        {
            ret = 1;
        }
    }
    else
    {
        ret = 0;
    }

    if(*(uint32_t*)CalAddr == MAGIC_IAP)
    {
        FLASH_Unlock_Fast();
        FLASH_ProgramWord(CalAddr, MAGIC_IAP);
        FLASH->CTLR |= ((uint32_t)0x00008000);
        FLASH->CTLR |= ((uint32_t)0x00000080);
    }

    return ret;
}

void jump_to_iap()
{
    FLASH_Unlock_Fast();
    FLASH_ProgramWord(CalAddr, MAGIC_IAP);
    FLASH->CTLR |= ((uint32_t)0x00008000);
    FLASH->CTLR |= ((uint32_t)0x00000080);
}
