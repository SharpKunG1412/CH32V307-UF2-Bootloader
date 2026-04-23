#ifndef UF2FORMAT_H
#define UF2FORMAT_H

#include <stdint.h>
#include <stdbool.h>

#define UF2_VERSION         "0.0.4"
#define UF2_PRODUCT_NAME    "CH32V UF2"
#define UF2_VOLUME_LABEL    "SHARP BOOT"
#define UF2_BOARD_ID        "CH32V307"

#define BOARD_UF2_FAMILY_ID 0x0A424B53 /* SKB\n */

#define UF2_INDEX_URL       "https://google.com/"

#define BOARD_FLASH_APP_START   0x08005000

/* The largest flash size that is supported by the board, in bytes, default is 4MB */
/* Flash size is constrained by RAM, a 4MB size requires 2kB RAM, see MAX_BLOCKS */
/* Largest tested is 256MB, with 0x300000 blocks (1.5GB), 64 sectors per cluster */
#ifndef CFG_UF2_FLASH_SIZE
#define CFG_UF2_FLASH_SIZE          (4*1024*1024)
#endif

/* Number of 512-byte blocks in the exposed filesystem, default is just under 32MB */
/* The filesystem needs space for the current file, text files, uploaded file, and FAT */
#ifndef CFG_UF2_NUM_BLOCKS
#define CFG_UF2_NUM_BLOCKS          (0x10109)
#endif

/* Sectors per FAT cluster, must be increased proportionally for larger filesystems */
#ifndef CFG_UF2_SECTORS_PER_CLUSTER
#define CFG_UF2_SECTORS_PER_CLUSTER (1)
#endif

/* All entries are little endian. */
#define UF2_MAGIC_START0    0x0A324655UL /* "UF2\n" */
#define UF2_MAGIC_START1    0x9E5D5157UL /* Randomly selected */
#define UF2_MAGIC_END       0x0AB16F30UL /* Ditto */

/* If set, the block is "comment" and should not be flashed to the device */
#define UF2_FLAG_NOFLASH    0x00000000
#define UF2_FLAG_FAMILYID   0x00002000

#define MAX_BLOCKS (CFG_UF2_FLASH_SIZE / 256 + 100)
typedef struct {
    uint32_t numBlocks;
    uint32_t numWritten;

    bool aborted;             /* aborting update and reset */

    uint8_t writtenMask[MAX_BLOCKS / 8 + 1];
} WriteState;

typedef struct {
    /* 32 byte header */
    uint32_t magicStart0;
    uint32_t magicStart1;
    uint32_t flags;
    uint32_t targetAddr;
    uint32_t payloadSize;
    uint32_t blockNo;
    uint32_t numBlocks;
    uint32_t familyID;

    /* raw data; */
    uint8_t data[476];

    /* store magic also at the end to limit damage from partial block reads */
    uint32_t magicEnd;
} UF2_Block;

void uf2_init(void);
void uf2_read_block (uint32_t block_no, uint8_t *data);
int uf2_write_block (uint32_t block_no, uint8_t *data, WriteState *state);
void msc_write10_complete_cb(WriteState *state);

#endif
