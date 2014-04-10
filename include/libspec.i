%module libspec

%{
#include <stdint.h>
#include "libspec.h"
%}

typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;

%include "types.h"
%include "prng.h"
%include "pkm.h"
%include "stat.h"
%include "checksum.h"
%include "game_gb.h"
%include "game_gba.h"
%include "game_nds.h"
%include "game_ndsi.h"