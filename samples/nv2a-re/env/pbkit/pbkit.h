#include "pbkit/nv_regs.h"
#include "pbkit/outer.h"

#include <stdint.h>

typedef uint32_t DWORD;

#define SUBCH_3D                0

uint32_t* pb_begin();
void pb_end(uint32_t* p);
void pb_push(uint32_t *p, DWORD command, DWORD nparam);
