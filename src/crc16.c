// The CRC-16s the DS and 3DS saves check their blocks with.

#include "spec_internal.h"

constexpr uint16_t CRC_POLYNOMIAL = 0x1021;
// 0x8005, reflected.
constexpr uint16_t CRC_USB_POLYNOMIAL = 0xA001;

// As MATH_CalcCRC16CCITT; Gen 6 checks its blocks with it too.
uint16_t spec_crc16(const uint8_t *bytes, size_t size) {
    uint16_t crc = 0xFFFF;
    for (size_t index = 0; index < size; ++index) {
        crc ^= (uint16_t)(bytes[index] << 8);
        for (unsigned bit = 0; bit < 8; ++bit) {
            bool is_top_bit_set = (crc & 0x8000) != 0;
            crc = (uint16_t)(crc << 1);
            if (is_top_bit_set) {
                crc ^= CRC_POLYNOMIAL;
            }
        }
    }
    return crc;
}

// CRC-16/USB, which Gen 7 checks its blocks with.
uint16_t spec_crc16_usb(const uint8_t *bytes, size_t size) {
    uint16_t crc = 0xFFFF;
    for (size_t index = 0; index < size; ++index) {
        crc ^= bytes[index];
        for (unsigned bit = 0; bit < 8; ++bit) {
            bool is_low_bit_set = (crc & 1) != 0;
            crc = (uint16_t)(crc >> 1);
            if (is_low_bit_set) {
                crc ^= CRC_USB_POLYNOMIAL;
            }
        }
    }
    return (uint16_t)~crc;
}
