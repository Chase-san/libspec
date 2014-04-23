%module libspec

%{
#include <stdint.h>
#include "libspec.h"
%}

%include <stdint.i>

%include "types.h"
%include "prng.h"
%include "pkm.h"
%include "stat.h"
%include "checksum.h"
%include "game_gb.h"
%include "game_gba.h"
%include "game_nds.h"
%include "game_ndsi.h"