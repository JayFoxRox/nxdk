// Header

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint32_t address;
  uint8_t* buffer;
  size_t size;
  const char* name;
} Reference;


static Reference** references = NULL;
uint32_t reference_count = 0;

static Reference* GetReference(const char* name) {
  //FIXME: Add to table of references if it didn't exist yet
  for(int i = 0; i < reference_count; i++) {
    Reference* reference = references[i];
    if (reference->buffer != 0) {
      if (!strcmp(reference->name, name)) {
        return reference;
      }
    }
  }
  references = realloc(references, sizeof(Reference*) * ++reference_count);
  Reference* reference = malloc(Reference);
  reference->name = strdup(name);
  reference->buffer = 0;
  reference->size = 0;
  references[reference_count - 1] = reference;
  return NULL;
}

#if 0
//FIXME: Decide where we'll write our pushbuffer to
static void Emit(void* data, size_t size) {
    fwrite(data, size, 1, stdout)
}

static void EmitU32(uint32_t v) { 
    Emit(&v, 4);
}

static void EmitString(const char* s) {
    Emit(s, strlen(s) + 1);
}

__attribute__((constructor))
static void EmitFooter(void) {
  uint32_t reference_count = ConsumeU32();
  for(int i = 0; i < reference_count; i++) {
    reference->name = ConsumeString();
    reference->offset = ConsumeU32();
    reference->size = ConsumeU32();
  }
}

__attribute__((destructor))
static void EmitFooter(void) {
  uint32_t reference_count;
  EmitU32(&reference_count, 4);
  for(int i = 0; i < reference_count; i++) {
    EmitString(reference->name);
    EmitU32(reference->offset);
    EmitU32(reference->size);
  }
}
#endif 

// pbkit.c

uint32_t* pb_begin() {
  return NULL;
}
void pb_end(uint32_t* p) {
}
uint32_t* pb_push(uint32_t* a, uint32_t b, int c) {
  return NULL;
}

// User code

#define inline // WTF?! linker complains without this [for XGU/XGUX]

#include <assert.h>
#include <stdint.h>
#include "xgu/xgu.h"
#include "xgu/xgux.h"

int main() {
  Reference* tip_x = GetReference("Tip x"); // This will be shown in the GUI
  assert(tip_x->size == sizeof(float));

  uint32_t* p;

  p = pb_begin();

  p = xgu_begin(p, XGU_TRIANGLES);
  p = xgu_vertex3f(p, -1.0f, -1.0f, 1.0f);
  p = xgu_vertex3f(p, *(float*)tip_x->buffer, 1.0f, 1.0f);
  p = xgu_vertex3f(p, 1.0f,  -1.0f, 1.0f);
  p = xgu_end(p);

  pb_end(p);

  return 0;
}
