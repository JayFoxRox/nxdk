// This shows how to set up a LUT via texture-shaders.
//
// Normally you can use the gamma ramps to do a LUT for free.
// However, the gamma ramp is not affecting the framebuffer.
// So if you still have to re-use parts of the frame as texture, then you have
// to use a LUT like this.
//
// This could be used for mid-frame tone-mapping, if you still have to re-use
// parts of the image as a texture.
//
// This LUT is also more advanced because you can combine different components.
// Due to hardware limitations, this is only possible between AR and GB.
//
// In many applications, however, you don't need a full LUT.
// In such cases, you should use a simple register combiner instead.

#include <stdbool.h>
#include <stdint.h>

#include <hal/video.h>

#include <pbkit/pbkit.h>

#include <xgu/xgu.h>
#include <xgu/xgux.h>

#include <windows.h>

//FIXME: Create a proper lib for this
#include "swizzle.c"

//FIXME: Add to XGU
//#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8  0x1B
#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8   0x13
#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8 0x16
#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8 0x17

#define XGU_TEXTURE_FORMAT_Y8 NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8
#define XGU_TEXTURE_FORMAT_R8G8B8A8 NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8
#define XGU_TEXTURE_FORMAT_R8B8 NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8
#define XGU_TEXTURE_FORMAT_G8B8 NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8

#define MAP_R(r) r //(r-0xFF)
#define MAP_G(g) g //(g-0xFF)
#define MAP_B(b) b //(b-0xFF)
#define MAP_A(a) a //(a-0xFF)

static void dummy_init() {
  uint32_t* p;

  /* Basic setup */
  int width = pb_back_buffer_width();
  int height = pb_back_buffer_height();

  /* A generic identity matrix */
  const float m_identity[4*4] = {
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
  };

  /* A viewport matrix; this maps:
   *  - X from [-1, 1] to [0, width]
   *  - Y from [-1, 1] to [height, 0]
   *  - Z from [ 0, 1] to [0, 0xFFFF]
   *  - W to 1
   * This scales: X from [-1, 1] to [0, width]
   */
  float m_viewport[4*4] = {
      width/2.0f, 0.0f,         0.0f,          width/2.0f,
      0.0f,       height/-2.0f, 0.0f,          height/2.0f,
      0.0f,       0.0f,         (float)0xFFFF, 0.0f,
      0.0f,       0.0f,         0.0f,          1.0f
  };

  /* Set up some default GPU state (should be done in xgux_init maybe? currently partially done in pb_init) */
  {
      p = pb_begin();

      p = xgu_set_skin_mode(p, XGU_SKIN_MODE_OFF);
      p = xgu_set_normalization_enable(p, false);
      p = xgu_set_lighting_enable(p, false);

      for(int i = 0; i < XGU_TEXTURE_COUNT; i++) {
          p = xgu_set_texgen_s(p, i, XGU_TEXGEN_DISABLE);
          p = xgu_set_texgen_t(p, i, XGU_TEXGEN_DISABLE);
          p = xgu_set_texgen_r(p, i, XGU_TEXGEN_DISABLE);
          p = xgu_set_texgen_q(p, i, XGU_TEXGEN_DISABLE);
          p = xgu_set_texture_matrix_enable(p, i, false);
          p = xgu_set_texture_matrix(p, i, m_identity);
      }

      for(int i = 0; i < XGU_WEIGHT_COUNT; i++) {
          p = xgu_set_model_view_matrix(p, i, m_identity); //FIXME: Not sure when used?
          p = xgu_set_inverse_model_view_matrix(p, i, m_identity); //FIXME: Not sure when used?
      }

      pb_end(p);
  }

  /* Set up all states for hardware vertex pipeline */
  p = pb_begin();
  p = xgu_set_transform_execution_mode(p, XGU_FIXED, XGU_RANGE_MODE_PRIVATE);
  //FIXME: p = xgu_set_fog_enable(p, false);
  p = xgu_set_projection_matrix(p, m_identity); //FIXME: Unused in XQEMU
  p = xgu_set_composite_matrix(p, m_viewport); //FIXME: Always used in XQEMU?
  p = xgu_set_viewport_offset(p, 0.0f, 0.0f, 0.0f, 0.0f);
  p = xgu_set_viewport_scale(p, 1.0f / width, 1.0f / height, 1.0f / (float)0xFFFF, 1.0f); //FIXME: Ignored?!
  pb_end(p);
}


static void set_texture(int i, int width, int height, unsigned int pitch, unsigned int format, void* data) {
  uint32_t* p = pb_begin();
  bool cubemap_enable = false;
  unsigned int dimensionality = 2;

  unsigned int context_dma = 2; //FIXME: Which one did pbkit use?
  XguBorderSrc border = XGU_SOURCE_COLOR;

  unsigned int mipmap_levels = 1;
  unsigned int min_lod = 0;
  unsigned int max_lod = mipmap_levels - 1;
  unsigned int lod_bias = 0;

  p = xgu_set_texture_offset(p, i, (uintptr_t)data & 0x03ffffff);
  p = xgu_set_texture_format(p, i, context_dma, cubemap_enable, border, dimensionality,
                                   format, mipmap_levels,
                                   width / 32, height / 32, 0);
  p = xgu_set_texture_address(p, i, XGU_CLAMP_TO_EDGE, false,
                                    XGU_CLAMP_TO_EDGE, false,
                                    XGU_CLAMP_TO_EDGE, false,
                                    false);
  p = xgu_set_texture_control0(p, i, true, min_lod, max_lod);
  p = xgu_set_texture_control1(p, i, pitch);
  p = xgu_set_texture_filter(p, i, lod_bias, XGU_TEXTURE_CONVOLUTION_QUINCUNX,
                                      //FIXME: This sets up NEAREST filters
                                      1, //gl_to_xgu_texture_filter(tx->min_filter),
                                      1, //gl_to_xgu_texture_filter(tx->mag_filter),
                                      false, false, false, false);
  p = xgu_set_texture_image_rect(p, i, width, height);
  pb_end(p);
}

void prepare_lut_r_gb_a() {
  // Allocate space for 3 LUTs
  uint8_t* lut_r_gb_a = MmAllocateContiguousMemory(256+256*256*2+256);
  uint8_t* lut_r = &lut_r_gb_a[0]; // 256
  uint8_t* lut_gb = &lut_r_gb_a[0+256]; // 256*256*2
  uint8_t* lut_a = &lut_r_gb_a[0+256+256*256*2]; // 256

  // Generate masks so we can swizzle
  uint32_t mask_1x256_x;
  uint32_t mask_1x256_y;
  uint32_t mask_1x256_z;
  generate_swizzle_masks(1,256,0,&mask_1x256_x,&mask_1x256_y,&mask_1x256_z);
  uint32_t mask_256x256_x;
  uint32_t mask_256x256_y;
  uint32_t mask_256x256_z;
  generate_swizzle_masks(256,256,0,&mask_256x256_x,&mask_256x256_y,&mask_256x256_z);
  uint32_t mask_256x1_x;
  uint32_t mask_256x1_y;
  uint32_t mask_256x1_z;
  generate_swizzle_masks(256,1,0,&mask_256x1_x,&mask_256x1_y,&mask_256x1_z);

  // Set up a LUT for R
  for(int r = 0; r < 256; r++) {
    lut_r[get_swizzled_offset(0, r, 0, mask_1x256_x, mask_1x256_y, mask_1x256_z, 1)] = MAP_R(r);
  }
  // Set up a combined LUT for GB (these can interact)
  for(int g = 0; g < 256; g++) {
    for(int b = 0; b < 256; b++) {
      unsigned int offset = get_swizzled_offset(g, b, 0, mask_256x256_x, mask_256x256_y, mask_256x256_z, 2);
      lut_gb[offset+0] = MAP_G(g);
      lut_gb[offset+1] = MAP_B(b);
    }
  }
  // Set up a LUT for A
  for(int a = 0; a < 256; a++) {
    lut_a[get_swizzled_offset(a, 0, 0, mask_256x1_x, mask_256x1_y, mask_256x1_z, 1)] = MAP_A(a);
  }

  // Create textures from our LUTs
  set_texture(1, 1, 256, 1, XGU_TEXTURE_FORMAT_Y8_SWIZZLED, lut_r);
  set_texture(2, 256, 256, 256*2, XGU_TEXTURE_FORMAT_G8B8_SWIZZLED, lut_gb);
  set_texture(3, 256, 1, 256, XGU_TEXTURE_FORMAT_Y8_SWIZZLED, lut_a);

  // Load our LUT shader
  {
    uint32_t* p = pb_begin();
    #include "lut_r_gb_a.fp.inl"
    pb_end(p);
  }
}

void prepare_lut_ar_gb() {
  // Allocate space for 2 LUTs
  uint8_t* lut_ar_gb = MmAllocateContiguousMemory(256*256*2+256*256*2);
  uint8_t* lut_ar = &lut_ar_gb[0]; // 256*256*2
  uint8_t* lut_gb = &lut_ar_gb[0+256*256*2]; // 256*256*2

  // Generate masks so we can swizzle
  uint32_t mask_256x256_x;
  uint32_t mask_256x256_y;
  uint32_t mask_256x256_z;
  generate_swizzle_masks(256,256,0,&mask_256x256_x,&mask_256x256_y,&mask_256x256_z);

  // Set up a combined LUT for AR (these can interact)
  for(int a = 0; a < 256; a++) {
    for(int r = 0; r < 256; r++) {
      unsigned int offset = get_swizzled_offset(a, r, 0, mask_256x256_x, mask_256x256_y, mask_256x256_z, 2);
      lut_ar[offset+0] = MAP_A(r);
      lut_ar[offset+1] = MAP_R(a);
    }
  }
  // Set up a combined LUT for GB (these can interact)
  for(int g = 0; g < 256; g++) {
    for(int b = 0; b < 256; b++) {
      unsigned int offset = get_swizzled_offset(g, b, 0, mask_256x256_x, mask_256x256_y, mask_256x256_z, 2);
      lut_gb[offset+0] = MAP_G(g);
      lut_gb[offset+1] = MAP_B(b);
    }
  }

  // Create textures from our LUTs
  set_texture(1, 256, 256, 256*2, XGU_TEXTURE_FORMAT_R8B8_SWIZZLED, lut_ar);
  set_texture(2, 256, 256, 256*2, XGU_TEXTURE_FORMAT_G8B8_SWIZZLED, lut_gb);

  // Load our LUT shader
  {
    uint32_t* p = pb_begin();
    #include "lut_ar_gb.fp.inl"
    pb_end(p);
  }
}

static void prepare_pass_through() {
  // Load our pass-through shader
  {
    uint32_t* p = pb_begin();
    #include "pass_through.fp.inl"
    pb_end(p);
  }
}

int main() {
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  int status = pb_init();
  if (status) {
      Sleep(2000);
      return 1;
  }

  //FIXME: Hack for XQEMU
  Sleep(1000);

  pb_show_front_screen();

  dummy_init();

  //FIXME: Set up an image texture
  uint32_t* image = MmAllocateContiguousMemory(640*480*4);
  for(int y = 0; y < 480; y++) {
    for(int x = 0; x < 640; x++) {
      if (y < 120) {
        image[y * 640 + x] = (x & 0xFF) << 0;
      } else if (y < 240) {
        image[y * 640 + x] = (x & 0xFF) << 8;
      } else if (y < 360) {
        image[y * 640 + x] = (x & 0xFF) << 16;
      } else {
        image[y * 640 + x] = (x & 0xFF) << 24;
      }
    }
  }
  set_texture(0, 640, 480, 640*4, XGU_TEXTURE_FORMAT_A8R8G8B8, image);
  
  // Prepare pass-through
  prepare_pass_through();

#if 0
  // Prepare a LUT
  prepare_lut_ar_gb();
#endif
#if 1
  // Prepare a LUT
  prepare_lut_r_gb_a();
#endif

  while(1) {
    pb_wait_for_vbl();
    pb_reset();
    pb_target_back_buffer();

    /* Clear depth & stencil buffers */
    pb_erase_depth_stencil_buffer(0, 0, 640, 480); //FIXME: Do in XGU
    pb_fill(0, 0, 640, 480, 0x00000000); //FIXME: Do in XGU
    pb_erase_text_screen();

    while(pb_busy()) {
        /* Wait for completion... */
    }

    {
      uint32_t* p = pb_begin();

      p = xgu_begin(p, XGU_TRIANGLES);

      /* Background triangle 1 */
      p = xgu_set_vertex_data4f(p, XGU_TEXCOORD0_ARRAY,   0.0f,   0.0f, 0.0f, 1.0f);
      p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex3f(p, -1.0f, -1.0f,  1.0f);
      p = xgu_set_vertex_data4f(p, XGU_TEXCOORD0_ARRAY,   0.0f, 480.0f, 0.0f, 1.0f);
      p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex3f(p, -1.0f,  1.0f,  1.0f);
      p = xgu_set_vertex_data4f(p, XGU_TEXCOORD0_ARRAY, 640.0f, 480.0f, 0.0f, 1.0f);
      p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex3f(p,  1.0f,  1.0f,  1.0f);

      /* Background triangle 2 */
      p = xgu_set_vertex_data4f(p, XGU_TEXCOORD0_ARRAY,   0.0f,   0.0f, 0.0f, 1.0f);
      p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex3f(p, -1.0f, -1.0f,  1.0f);
      p = xgu_set_vertex_data4f(p, XGU_TEXCOORD0_ARRAY, 640.0f, 480.0f, 0.0f, 1.0f);
      p = xgux_set_color3f(p, 0.0f, 0.0f, 0.0f); p = xgu_vertex3f(p,  1.0f,  1.0f,  1.0f);
      p = xgu_set_vertex_data4f(p, XGU_TEXCOORD0_ARRAY, 640.0f,   0.0f, 0.0f, 1.0f);
      p = xgux_set_color3f(p, 0.1f, 0.1f, 0.6f); p = xgu_vertex3f(p,  1.0f, -1.0f,  1.0f);

      p = xgu_end(p);

      pb_end(p);
    }

    /* Draw some text on the screen */
    pb_print("color-lut demo\n");
    pb_draw_text_screen();

    while(pb_busy()) {
        /* Wait for completion... */
    }

    /* Swap buffers (if we can) */
    while (pb_finished()) {
        /* Not ready to swap yet */
    }
  }
}
