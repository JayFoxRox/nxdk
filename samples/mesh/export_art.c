#include <stdint.h>
#include <stdio.h>
#include <assert.h>

// We declare attributes as double to lose as little precision as possible
typedef struct Vertex {
  double pos[3];
  double normal[3];
  double texcoord[2];
} Vertex;

#include "verts.h"
#include "texture.h"

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

// The indices are stored as pairs; this getter splits them
static int index16(int index) {
  uint32_t index_pair = indices[index / 2];
  if (index % 2) {
    return (index_pair >> 16) & 0xFFFF;
  }
  return index_pair & 0xFFFF;
}

int main() {

  FILE* f = fopen("verts.obj", "wb");
  for(int i = 0; i < ARRAY_SIZE(vertices); i++) {
    const Vertex* v = &vertices[i];
    fprintf(f, "v %f %f %f\n", v->pos[0], v->pos[1], v->pos[2]);
    fprintf(f, "vn %f %f %f\n", v->normal[0], v->normal[1], v->normal[2]);
    fprintf(f, "vt %f %f\n", v->texcoord[0], v->texcoord[1]);
  }
  int index_count = ARRAY_SIZE(indices) * 2;
  assert(index_count % 3 == 0);
  for(int i = 0; i < index_count / 3; i++) {
    int a = 1 + index16(i * 3 + 0);
    int b = 1 + index16(i * 3 + 1);
    int c = 1 + index16(i * 3 + 2);
    fprintf(f, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", a,a,a, b,b,b, c,c,c);
  }
  fclose(f);

  FILE* f_rgb = fopen("texture_rgb.ppm", "wb");
  fprintf(f_rgb, "P3 %d %d 255\n", texture_width, texture_height);
  FILE* f_a = fopen("texture_a.pgm", "wb");
  fprintf(f_a, "P2 %d %d 255\n", texture_width, texture_height);
  for(int y = 0; y < texture_height; y++) {
    for(int x = 0; x < texture_width; x++) {
      int pixel_offset = y * texture_pitch + x * 4;
      int r = texture_rgba[pixel_offset + 0];
      int g = texture_rgba[pixel_offset + 1];
      int b = texture_rgba[pixel_offset + 2];
      int a = texture_rgba[pixel_offset + 3];
      fprintf(f_rgb, "%d %d %d\n", r,g,b);
      fprintf(f_a, "%d\n", a);
    }
    fprintf(f_rgb, "\n");
    fprintf(f_a, "\n");
  }
  fclose(f_rgb);
  fclose(f_a);

  return 0;
}
