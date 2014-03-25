/**
 * @file types.h
 * @brief Contains basic types used by libSPEC.
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

/**
 * A boolean value, defined ourselves since we have to be sure it is only 1 byte in size.
 */
typedef uint8_t bool;

/**
 * Enum of boolean values
 */
typedef enum {
	true = 1,
	false = 0,
	TRUE = 1,
	FALSE = 0
} boolean_value_t;

/**
 * Type used for 8 bit characters (gb,gba).
 */
typedef uint8_t char8_t;
/**
 * Type used for 16 bit characters (nds,dsi).
 */
typedef uint16_t char16_t;

#endif /* TYPES_H_ */
