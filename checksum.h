#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

#include <stdlib.h>
#include <stdint.h>

uint16_t nds_crc16(uint8_t *, size_t);
uint16_t nds_checksum(uint8_t *, size_t);
uint16_t gba_block_checksum(uint8_t *, size_t);

#endif //__CHECKSUM_H__
