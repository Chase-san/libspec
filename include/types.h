/**
 * @file types.h
 * @brief Contains basic types used by libSPEC.
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

/* Don't define bool if it is already defined (standard in C++) */
#ifndef __cplusplus

#ifndef SWIG
/**
 * @brief A boolean value, defined ourselves since we have to be sure it is only 1 byte in size.
 */
typedef uint8_t bool;

/**
 * @brief Enum of boolean values, enums play nicer with editors/debuggers
 */
enum {
	true = 1,
	false = 0,
};
#endif

#endif

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
