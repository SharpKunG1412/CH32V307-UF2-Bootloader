/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/* @Note
 * UDisk Example:
 * This program provides examples of UDisk.Supports external SPI Flash and internal
 * Flash, selected by STORAGE_MEDIUM at SW_UDISK.h.
 *  */

#include "ch32v30x_usbhs_device.h"
#include "debug.h"
#include "Internal_Flash.h"
#include "SPI_FLASH.h"
#include "SW_UDISK.h"
#include "uf2.h"
#include "board.h"
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SystemCoreClockUpdate( );
	Delay_Init( );
	USART_Printf_Init( 115200 );
		
	printf( "SystemClk:%d\r\n", SystemCoreClock );
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

	Flash_Sector_Count = MAX_BLOCKS;
    Flash_Sector_Size = DEF_UDISK_SECTOR_SIZE;

	printf( "USBHS CH32 UDisk\r\nStorage Medium: INTERNAL FLASH \r\n" );
    printf("Flash_Sector_Count: %4d\r\n", Flash_Sector_Count);
    printf("Flash_Sector_Size: %4d\r\n", Flash_Sector_Size);

    uf2_pin_init();

    if(uf2_run_app())
    {
        uf2_board_app_jump();
    }

    uf2_init();

    /* Enable Udisk */
    Udisk_Capability = Flash_Sector_Count;
    Udisk_Status |= DEF_UDISK_EN_FLAG;

    /* USB20 device init */
    USBHS_Device_Init( DISABLE );
    Delay_Ms(100);
    USBHS_RCC_Init( );
    USBHS_Device_Init( ENABLE );

	while(1)
	{
	    ;
	}
}
