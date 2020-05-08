#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

static FILE* f = NULL;

void _pb_emit(void* data, size_t size) {
  fwrite(data, size, 1, f);
}

__attribute__((constructor))
void begin_pb_emit() {
  f = fopen("tmp/pb.bin", "wb");
  assert(f != NULL);
}

__attribute__((destructor))
void end_pb_emit() {
  assert(f != NULL);
  fclose(f);
}
