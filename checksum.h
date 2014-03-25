/**
 * @file checksum.h
 * @brief Containes functions for calculation of checksums.
 */

#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__

#include <stdlib.h>
#include <stdint.h>

uint16_t nds_crc16(const uint8_t *, size_t);
uint16_t gba_block_checksum(const uint8_t *, size_t);
uint8_t gb_rby_checksum(const uint8_t *, size_t);
uint16_t gb_gsc_checksum(const uint8_t *, size_t);
uint16_t pkm_checksum(const uint8_t *, size_t);
uint16_t pk3_checksum(const uint8_t *, size_t);

#endif //__CHECKSUM_H__
