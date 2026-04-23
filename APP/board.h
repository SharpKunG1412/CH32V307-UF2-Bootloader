#ifndef BOARD_H
#define BOARD_H

#define FLASH_SIZE (1*1024*1024)

#define CalAddr             (0x08078000-4)
#define MAGIC_APP           0xFFFFFFFF
#define MAGIC_IAP           0x0000FFFF
#define MAGIC_FLASHING      0x00000000

void uf2_pin_init();
uint32_t uf2_board_flash_size();
void uf2_board_flash_read(uint32_t addr, uint8_t *buffer, uint32_t len);
void uf2_board_flash_write(uint32_t addr, uint8_t *src, uint32_t len);
void uf2_board_flash_flush();
void uf2_board_dfu_complete();
void uf2_board_app_jump();
uint8_t uf2_run_app();

#endif
