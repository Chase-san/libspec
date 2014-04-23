/**
 * @file types.h
 * @brief Contains basic types used by libSPEC.
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

/**
 * @brief Type used for 8 bit characters (gb, gba).
 */
typedef uint8_t char8_t;

/**
 * @brief Type used for 16 bit characters (nds, dsi).
 */
typedef uint16_t char16_t;

/** Global Definitions */
enum {
	POKEMON_IN_PARTY = 6
};

#endif /* TYPES_H_ */
