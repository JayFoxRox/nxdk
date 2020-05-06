#include <assert.h>
#include <stdlib.h>

#include "pbkit.h"

static uint32_t* cp = NULL;
static const size_t pb_size = 1 * 1024 * 1024;

uint32_t* pb_begin() {
  assert(cp == NULL);
  cp = malloc(pb_size);
  return cp;
}
void pb_end(uint32_t* p) {
  size_t size = (uintptr_t)p - (uintptr_t)cp;
  assert(size <= pb_size);
  _pb_emit(cp, size);
  free(cp);
  cp = NULL;
}
