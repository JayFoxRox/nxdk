#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// pbkit.c

#include <assert.h>

#include "pbkit/pbkit.h"

static FILE* f = NULL;
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
  fwrite(cp, size, 1, f);
  free(cp);
  cp = NULL;
}
 

#define EncodeMethod(subchannel,command,nparam) ((nparam<<18)+(subchannel<<13)+command)

void pb_push_to(DWORD subchannel, uint32_t *p, DWORD command, DWORD nparam)
{
    *(p+0)=EncodeMethod(subchannel,command,nparam);
}

void pb_push(uint32_t *p, DWORD command, DWORD nparam)
{
    pb_push_to(SUBCH_3D,p,command,nparam);
}

// User code

#define inline // WTF?! linker complains without this [for XGU/XGUX]

#include <assert.h>
#include <stdint.h>
#include "xgu/xgu.h"
#include "xgu/xgux.h"

static void generate_reset() {
  uint32_t* p;

  /* A generic identity matrix */
  const float m_identity[4*4] = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  };

  /* Set up some default GPU state (should be done in xgux_init maybe? currently partially done in pb_init) */
  p = pb_begin();

  //FIXME: p = xgu_set_skinning(p, XGU_SKINNING_OFF);
  //FIXME: p = xgu_set_normalization(p, false);
  p = xgu_set_lighting_enable(p, false);

  for(int i = 0; i < XGU_TEXTURE_COUNT; i++) {
      //FIXME: p = xgu_set_texgen(p, XGU_TEXGEN_OFF);
      //p = xgu_set_texture_matrix_enable(p, i, false);
  }

  for(int i = 0; i < XGU_WEIGHT_COUNT; i++) {
      p = xgu_set_model_view_matrix(p, i, m_identity); //FIXME: Not sure when used?
      p = xgu_set_inverse_model_view_matrix(p, i, m_identity); //FIXME: Not sure when used?
  }

  pb_end(p);


  int width = 640;
  int height = 480;

  float m_viewport[4*4] = {
      width/2.0f, 0.0f,         0.0f,          width/2.0f,
      0.0f,       height/-2.0f, 0.0f,          height/2.0f,
      0.0f,       0.0f,         (float)0xFFFF, 0.0f,
      0.0f,       0.0f,         0.0f,          1.0f
  };

  /* Set up all states for hardware vertex pipeline */
  p = pb_begin();
  p = xgu_set_transform_execution_mode(p, XGU_FIXED, XGU_RANGE_MODE_USER);
  //FIXME: p = xgu_set_fog_enable(p, false);
  p = xgu_set_projection_matrix(p, m_identity); //FIXME: Unused in XQEMU
  p = xgu_set_composite_matrix(p, m_viewport); //FIXME: Always used in XQEMU?
  p = xgu_set_viewport_offset(p, 0.0f, 0.0f, 0.0f, 0.0f);
  p = xgu_set_viewport_scale(p, 1.0f / width, 1.0f / height, 1.0f / (float)0xFFFF, 1.0f); //FIXME: Ignored?!
  pb_end(p);
}

static void generate_clears() {
  uint32_t* p;

  p = pb_begin();

  p = xgu_set_color_clear_value(p, 0xFF333333);
  p = xgu_set_zstencil_clear_value(p, 0xFFFFFFFF);
  p = xgu_set_clear_rect_horizontal(p, 0, 640);
  p = xgu_set_clear_rect_vertical(p, 0, 480);
  p = xgu_clear_surface(p, XGU_CLEAR_STENCIL | XGU_CLEAR_Z | XGU_CLEAR_COLOR);

  for(int i = 0; i < 10; i++) {
    p = xgu_set_color_clear_value(p, 0xFF00FF00);
    p = xgu_set_clear_rect_horizontal(p, 64 + i * 30, 64 + i * 30 + 20);
    p = xgu_set_clear_rect_vertical(p, 64, 64 + 20);
    p = xgu_clear_surface(p, XGU_CLEAR_COLOR);
  }

  pb_end(p);
}

static void generate_triangles(uint32_t tex_addr) {
  uint32_t* p;

  p = pb_begin();

  p = xgu_begin(p, XGU_TRIANGLES);

  /* Background triangle 1 */
  p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex3f(p, -1.0f, -1.0f,  1.0f);
  p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex3f(p, -1.0f,  1.0f,  1.0f);
  p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex3f(p,  1.0f,  1.0f,  1.0f);

  /* Background triangle 2 */
  p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex3f(p, -1.0f, -1.0f,  1.0f);
  p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex3f(p,  1.0f,  1.0f,  1.0f);
  p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex3f(p,  1.0f, -1.0f,  1.0f);

  p = xgu_end(p);

  /* Enable texture */
  

  p = xgu_begin(p, XGU_TRIANGLES);

  /* Foreground triangle */
  p = xgux_set_texcoord3f(p, 0, 0.5f, 0.0f, 0.0f)
  p = xgux_set_color3f(p, 1.0f, 0.0f, 0.0f); p = xgu_vertex3f(p, -1.0f, -1.0f,  1.0f);
  p = xgux_set_texcoord3f(p, 0, 0.0f, 1.0f, 0.0f)
  p = xgux_set_color3f(p, 0.0f, 1.0f, 0.0f); p = xgu_vertex3f(p,  0.0f,  1.0f,  1.0f);
  p = xgux_set_texcoord3f(p, 0, 1.0f, 1.0f, 0.0f)
  p = xgux_set_color3f(p, 0.0f, 0.0f, 1.0f); p = xgu_vertex3f(p,  1.0f, -1.0f,  1.0f);

  p = xgu_end(p);
  pb_end(p);
}

int main(int argc, char* argv[]) {
  f = fopen("pb.bin", "wb");
  assert(f != NULL);

  generate_reset();
  generate_clears();

  /* Setup texture */
  {
    uint32_t* p = pb_begin();
    pb_push(p++, NV097_WAIT_FOR_IDLE, 1);
    *p++ = 0;
    pb_end(p);
  }

  generate_triangles();

  /* Force pushbuffer to run in XQEMU */
  {
    uint32_t* p = pb_begin();
    pb_push(p++, NV097_WAIT_FOR_IDLE, 1);
    *p++ = 0;
    pb_end(p);
  }

/*
  p = xgu_begin(p, XGU_TRIANGLES);
  p = xgu_vertex3f(p, -1.0f, -1.0f, 1.0f);
  p = xgu_vertex3f(p, 1.0f, 1.0f, 1.0f);
  p = xgu_vertex3f(p, 1.0f,  -1.0f, 1.0f);
  p = xgu_end(p);
*/



  fclose(f);

  return 0;
}
