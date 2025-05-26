#ifndef SINE_TABLE_H
#define SINE_TABLE_H

#include <stdint.h>

#define QSPI_MEM __attribute__((section(".extflash_text"), aligned(4)))

extern const uint16_t sine_table[];

#endif // SINE_TABLE_H
