#include <stdio.h>

#define XVideoSetMode(...)

uint8_t fb[1920*1280*4];

static void* XVideoGetFB() {
  FILE* f = fopen("out.rgb565", "wb");
  fwrite(fb, sizeof(fb), 1, f);
  fclose(f);
  return fb;
}

