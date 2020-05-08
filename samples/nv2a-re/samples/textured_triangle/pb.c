#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "pbkit/pbkit.h"

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

#if 1
  /* Set up some default GPU state (should be done in xgux_init maybe? currently partially done in pb_init) */
  p = pb_begin();

  p = xgu_set_skin_mode(p, XGU_SKIN_MODE_OFF);
  //FIXME: p = xgu_set_normalization(p, false);
#if 1
  p = xgu_set_lighting_enable(p, false);
#endif

  for(int i = 0; i < XGU_TEXTURE_COUNT; i++) {
      p = xgu_set_texgen_s(p, i, XGU_TEXGEN_DISABLE);
      p = xgu_set_texgen_t(p, i, XGU_TEXGEN_DISABLE);
      p = xgu_set_texgen_r(p, i, XGU_TEXGEN_DISABLE);
      p = xgu_set_texgen_q(p, i, XGU_TEXGEN_DISABLE);
      p = xgu_set_texture_matrix_enable(p, i, false);
      p = xgu_set_texture_matrix_enable(p, i, m_identity);
  }

  for(int i = 0; i < XGU_WEIGHT_COUNT; i++) {
#if 1
      p = xgu_set_model_view_matrix(p, i, m_identity); //FIXME: Not sure when used?
#endif
#if 1
      p = xgu_set_inverse_model_view_matrix(p, i, m_identity); //FIXME: Not sure when used?
#endif
  }

  pb_end(p);

#endif
#if 1
  int width = 640;
  int height = 480;

  /* Set up all states for hardware vertex pipeline */
  p = pb_begin();
  p = xgu_set_transform_execution_mode(p, XGU_FIXED, XGU_RANGE_MODE_PRIVATE);
  //FIXME: p = xgu_set_fog_enable(p, false);
  p = xgu_set_projection_matrix(p, m_identity); //FIXME: Unused in XQEMU
  p = xgu_set_composite_matrix(p, m_identity); //FIXME: Always used in XQEMU?
  p = xgu_set_viewport_offset(p, 320.0f, 240.0f, 0.0f, 0.0f);
  p = xgu_set_clip_min(p, (float)0x000000);
  p = xgu_set_clip_max(p, (float)0xFFFFFF);
  p = xgu_set_viewport_scale(p, 1.0f, 1.0f, 1.0f, 0.0f); //FIXME: Ignored?!
  pb_end(p);
#endif


#if 1
  p = pb_begin();
  p = xgu_set_cull_face_enable(p, false);
  p = xgu_set_depth_test_enable(p, false);
  p = xgu_set_dither_enable(p, false);
  p = xgu_set_lighting_enable(p, false);
  p = xgu_set_stencil_test_enable(p, false);
  p = xgu_set_alpha_test_enable(p, false);
  p = xgu_set_blend_enable(p, false);
  pb_end(p);
#endif

}

static void generate_clears() {
  uint32_t* p;

  p = pb_begin();

  p = xgu_set_color_clear_value(p, 0xFF333333);
  p = xgu_set_zstencil_clear_value(p, 0xFFFFFFFF);
  p = xgu_set_clear_rect_horizontal(p, 0, 640);
  p = xgu_set_clear_rect_vertical(p, 0, 480);
  p = xgu_clear_surface(p, XGU_CLEAR_STENCIL | XGU_CLEAR_Z | XGU_CLEAR_COLOR);



  srand(time(NULL));
  for(int i = 0; i < rand() % 30; i++) {
    p = xgu_set_color_clear_value(p, 0xFF00FF00);
    p = xgu_set_clear_rect_horizontal(p, 64 + i * 30, 64 + i * 30 + 20);
    p = xgu_set_clear_rect_vertical(p, 64, 64 + 20);
    p = xgu_clear_surface(p, XGU_CLEAR_COLOR);
  }

  pb_end(p);
}

static void generate_triangles() {
  uint32_t* p;

  p = pb_begin();

#if 0
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
#endif

#if 1
  float s = 1.0f;
  p = xgu_begin(p, XGU_TRIANGLES);
  p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex4f(p,    0.0f * s, -100.0f * s, 0.5f, 1.0f);
  p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex4f(p, -100.0f * s,  100.0f * s, 0.5f, 1.0f);
  p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex4f(p,  100.0f * s,  100.0f * s, 0.5f, 1.0f);
  p = xgu_end(p);
#endif

#if 0
  /* Enable texture */
  //FIXME: !!!
  float w = 64.0f;
  float h = 64.0f;

  p = xgu_begin(p, XGU_TRIANGLES);

  /* Foreground triangle */
  int i = 1; //FIXME: Texture index in xgux_set_texcoord3f is bad!
  p = xgux_set_texcoord3f(p, i, 0.5f * w, 0.0f * h, 1.0f);
  p = xgux_set_color3f(p, 1.0f, 1.0f, 1.0f); p = xgu_vertex3f(p, -1.0f, -1.0f,  1.0f);
  p = xgux_set_texcoord3f(p, i, 0.0f * w, 1.0f * h, 1.0f);
  p = xgux_set_color3f(p, 1.0f, 1.0f, 1.0f); p = xgu_vertex3f(p,  0.0f,  1.0f,  1.0f);
  p = xgux_set_texcoord3f(p, i, 1.0f * w, 1.0f * h, 1.0f);
  p = xgux_set_color3f(p, 1.0f, 1.0f, 1.0f); p = xgu_vertex3f(p,  1.0f, -1.0f,  1.0f);

  p = xgu_end(p);
#endif
  pb_end(p);
}

int main(int argc, char* argv[]) {

#if 1
  generate_reset();
#endif
#if 1
  generate_clears();

#if 1
  /* Setup texture */
  {
    unsigned int texture_index = 0;
    uint32_t tex_addr = atoll(argv[2]);
    uint32_t* p = pb_begin();

    bool enable = true;
    uint16_t min_lod = 0;
    uint16_t max_lod = 0;
    bool r_signed = false;
    bool g_signed = false;
    bool b_signed = false;
    bool a_signed = false;
    uint16_t lod_bias = 0;
    XguTexConvolution convolution = XGU_TEXTURE_CONVOLUTION_QUINCUNX;
    uint8_t filter_min = 1; //FIXME: This is very unintuitive
    uint8_t filter_mag = 1; //FIXME: This is very unintuitive
    uint8_t context_dma = 0x2; //FIXME: This is very unintuitive
    bool cubemap_enable = 0;
    XguBorderSrc border_src = XGU_SOURCE_TEXTURE;
    uint8_t dimensionality = 2;
    XguTexFormatColor format = XGU_TEXTURE_FORMAT_A8B8G8R8;
    uint8_t mipmap_levels = 1;
    uint8_t u_size = 6;
    uint8_t v_size = 6;
    uint8_t p_size = 0;
    unsigned int width = 64;
    unsigned int height = 64;
    unsigned int pitch = 64 * 4;


    p = xgu_set_texture_offset(p, texture_index, tex_addr & 0x7FFFFFFF);
    p = xgu_set_texture_format(p, texture_index, context_dma, cubemap_enable, border_src, dimensionality, format, mipmap_levels, u_size, v_size, p_size);
    //p = xgu_set_texture_address(p, texture_index, );
    p = xgu_set_texture_control0(p, texture_index, enable, min_lod, max_lod);
    p = xgu_set_texture_control1(p, texture_index, pitch);
    p = xgu_set_texture_filter(p, texture_index, lod_bias, convolution, filter_min, filter_mag, r_signed, b_signed, g_signed, a_signed);
    p = xgu_set_texture_image_rect(p, texture_index, width, height);    

    pb_end(p);
  }
#endif

  /* Setup fragment program */
  {
    uint32_t* p = pb_begin();
#include "tmp/fp.inl"
    pb_end(p);
  }

#if 1
  generate_triangles();
#endif
#endif
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

  return 0;
}
