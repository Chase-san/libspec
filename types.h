/*
 * types.h
 *
 *  Created on: Mar 11, 2014
 *      Author: Chase
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

//Don't use stdbool.h, we have to make sure bool is only a byte in size!
typedef uint8_t bool;
#define true 1
#define false 0
#define TRUE 1
#define FALSE 0

typedef uint8_t char8_t;
typedef uint16_t char16_t;



#endif /* TYPES_H_ */
