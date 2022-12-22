#ifndef CONFIG_H
#define CONFIG_H

#define SHARKYOS_TOTAL_INTERRUPTS 512
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

// 100MB Heap Size
#define SHARKYOS_HEAP_SIZE_BYTES 104857600
#define SHARKYOS_HEAP_BLOCK_SIZE 4096
#define SHARKYOS_HEAP_ADDRESS 0x1000000
#define SHARKYOS_HEAP_TABLE_ADDRESS 0x7e00
#endif
