#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <math.h>

#include <hal/input.h>
#include <hal/video.h>
#include <hal/xbox.h>

#include <xboxrt/debug.h>

#include <pbkit/pbkit.h>

#include <xboxkrnl/xboxkrnl.h>

// Include nxdk-rdt files
#include <net.h>
#include <dbgd.h>

#define BUTTON_DEADZONE 0x20

static uint32_t *alloc_vertices;
static uint32_t  num_vertices;

typedef struct {
  float pos[3];
  float uv[2];
} __attribute__((packed)) Vertex;

static const Vertex vertices[] = {
  //  X     Y     Z       U     V
  {{ -1.0,  1.0, 1.0 }, { 0.0, 0.0 }}, 
  {{  1.0,  1.0, 1.0 }, { 1.0, 0.0 }}, 
  {{  1.0, -1.0, 1.0 }, { 1.0, 1.0 }}, 
  {{ -1.0, -1.0, 1.0 }, { 0.0, 1.0 }}
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max) {
  memset(out, 0, 4*4*sizeof(float));
  out[0][0] = width/2.0f;
  out[1][1] = height/-2.0f;
  out[2][2] = z_max - z_min;
  out[3][3] = 1.0f;
  out[3][0] = x + width/2.0f;
  out[3][1] = y + height/2.0f;
  out[3][2] = z_min;
}

static void init_shader(void) {
  uint32_t *p;

  // Setup vertex shader
  uint32_t vs_program[] = {
#include "vs.inl"
  };

  p = pb_begin();

  // Set run address of shader
  pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);
  p += 2;

  // Set execution mode
  pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE, 
      MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
      | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));
  p += 2;

  pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);
  p += 2;

  // Set cursor and begin copying program
  pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);
  p += 2;

  for (int i = 0; i < sizeof(vs_program) / 8; i++) {
    pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
    memcpy(p, &vs_program[i*4], 4*4);
    p+=4;
  }

  pb_end(p);

  // Setup fragment shader
  p = pb_begin();
//FIXME: Set up manually
#include "ps.inl"
  pb_end(p);
}

static void set_attrib_pointer(unsigned int index, unsigned int format, unsigned int size, unsigned int stride, const void* data) {
  uint32_t *p = pb_begin();
  pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + index*4, 
      MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, format) | \
      MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, size) | \
      MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, stride));
  p += 2;
  pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + index*4, (uint32_t)data & 0x03ffffff);
  p += 2;
  pb_end(p);
}

static void draw_arrays(unsigned int mode, int start, int count) {
  uint32_t *p = pb_begin();
  pb_push1(p, NV097_SET_BEGIN_END, mode); p += 2;

  pb_push(p++, 0x40000000 | NV097_DRAW_ARRAYS, 1); //bit 30 means all params go to same register 0x1810
  *(p++) = MASK(NV097_DRAW_ARRAYS_COUNT, (count-1)) | MASK(NV097_DRAW_ARRAYS_START_INDEX, start);

  // Due to bad design in pbkit, we can't return to pb_push1 after pb_push
  // As a workaround, we use pb_push
  pb_push(p++, NV097_SET_BEGIN_END, 1);
  *(p++) = NV097_SET_BEGIN_END_OP_END;

  pb_end(p);
}

void main(void) {
  uint32_t *p;

  debugPrint("Setting video mode\n");

  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  debugPrint("Initializing nxdk-rdt\n");

  net_init();
  dbgd_init();

  debugPrint("Initializing pbkit\n");

  int status = pb_init();
  if (status != 0) {
    debugPrint("pb_init Error %d\n", status);
    XSleep(2000);
    XReboot();
    return;
  }

  pb_show_front_screen();
//  pb_show_debug_screen(); //FIXME: Remove.. hack..

//while(1);

  // Initialize input
  XInput_Init();

  // Basic setup
  unsigned int width = pb_back_buffer_width();
  unsigned int height = pb_back_buffer_height();

  // Load constant rendering things (shaders, geometry)
  init_shader();
  alloc_vertices = MmAllocateContiguousMemoryEx(sizeof(vertices), 0, 0x3ffb000, 0, 0x404);
  memcpy(alloc_vertices, vertices, sizeof(vertices));
  num_vertices = ARRAY_SIZE(vertices);

  //Generate a texture
  int texture_width = 256;
  int texture_height = 256;
  uint8_t* pixels = NULL;

#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8    0x16
#define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8    0x17

  int texture_fmt = NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4; //FIXME: Does NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8; work?

  //FIXME: NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8
  //FIXME: NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8
  //FIXME: NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5
  //FIXME: NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4

  // Allocates 32bpp, for worst case
  pixels = MmAllocateContiguousMemoryEx(texture_width * texture_height * 4, 0, 0x3ffb000, 0, 0x404);

  uint8_t* cursor = pixels;
  for(int y = 0; y < texture_height; y++) {
    for(int x = 0; x < texture_width; x++) {
      uint32_t color = 0;

      uint8_t r = x;
      uint8_t g = y;
      uint8_t b = x ^ y;
      uint8_t a = x + y;

#if 1
      // Checkerboard is interesting to see interpolation
      int checker_x = 1 << 4;
      int checker_y = checker_x;
      uint8_t checker_mask = ((x + (y & checker_y)) & checker_x) ? 0xFF : 0x00;
/*
      r ^= checker_mask;
      g ^= checker_mask;
      b ^= checker_mask;
*/
      a ^= checker_mask;
#endif

#if 0
      bool hit = false;
      hit |= ((x == 3) && (y == 10));
      hit |= ((x == 11) && (y == 10));
      hit |= ((x == 11) && (y == 5));

      r = hit ? 0xFF : 0x00;
      g = 0;
      b = 0;
      a = 0;
#endif

      if (texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8) {
        *(uint32_t*)cursor = (a << 24) | (r << 16) | (g << 8) | b;
        cursor += 4;
      } else if (texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8) {
        *(uint16_t*)cursor = (a << 8) | r;
        cursor += 2;
      } else if (texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8) {
        *(uint16_t*)cursor = (r << 8) | b;
        cursor += 2;
      } else if (texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8) {
        *(uint16_t*)cursor = (g << 8) | b;
        cursor += 2;
      } else if ((texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5) ||
                 (texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5)) {
        a >>= 8 - 1;
        r >>= 8 - 5;
        g >>= 8 - 5;
        b >>= 8 - 5;
        *(uint16_t*)cursor = (a << 15) | (r << 10) | (g << 5) | b;
        cursor += 2;
      } else if (texture_fmt == NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4) {
        a >>= 8 - 4;
        r >>= 8 - 4;
        g >>= 8 - 4;
        b >>= 8 - 4;
        *(uint16_t*)cursor = (a << 12) | (r << 8) | (g << 4) | b;
        cursor += 2;
      } else {
        assert(false);
      }

    }
  }

  // Calculate texture pitch
  int texture_pitch = (cursor - pixels) / texture_height;

  while(1) {
    pb_wait_for_vbl();
    pb_reset();
    pb_target_back_buffer();

    // Clear depth & stencil buffers
    pb_erase_depth_stencil_buffer(0, 0, width, height);
    pb_fill(0, 0, width, height, 0x33333333);
    pb_erase_text_screen();

    //FIXME: WHY?!
    while(pb_busy()) {
      // Wait for completion...
    }

    // Check for input
    XInput_GetEvents();

    // Prepare new matrix
    float viewport[4][4];
    matrix_viewport(viewport, 0, 0, width, height, 0, 65536.0f);

    // A bias added to texture values
    float color_scale = 0.0f;
    float color_bias = 0.0f;
    float texture_scale[2] = { texture_width, texture_height };
    float texture_bias[2] = { 0.0f, 0.0f };

    // Send shader constants (must match locations from *.inl files)
    p = pb_begin();

    // Set shader constants cursor at c0
    pb_push1(p, NV097_SET_TRANSFORM_CONSTANT_LOAD, 96); p+=2;

    // Send the transformation matrix
    float c[][4] = {
      { viewport[0][0], viewport[0][1], viewport[0][2], viewport[0][3] }, // c0
      { viewport[1][0], viewport[1][1], viewport[1][2], viewport[1][3] }, // c1
      { viewport[2][0], viewport[2][1], viewport[2][2], viewport[2][3] }, // c2
      { viewport[3][0], viewport[3][1], viewport[3][2], viewport[3][3] }, // c3
      { texture_scale[0], texture_scale[1], 0.0f, 0.0f },                 // c4
      { texture_bias[0],  texture_bias[1],  0.0f, 0.0f }                  // c5
    };
    int float_count = ARRAY_SIZE(c) * 4;
    assert(float_count % 4 == 0);
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, float_count);
    memcpy(p, c, float_count * 4); p+=float_count;

    // Set constants for combiner stages
    //FIXME: var float4 scale :  : COMBINER_STAGE_CONST0[0] : 1 : 1
    //FIXME: var float4 bias :  : COMBINER_STAGE_CONST1[0] : 2 : 1

    pb_end(p);

    // Set up texture
    p=pb_begin();

    pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0),(DWORD)(MmGetPhysicalAddress(pixels) & 0x03ffffff), (0x0001002a | (texture_fmt << 8))); p+=3; //set stage 0 texture address & format
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),texture_pitch<<16); p+=2; //set stage 0 texture pitch (pitch<<16)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(texture_width<<16)|texture_height); p+=2; //set stage 0 texture width & height ((witdh<<16)|height)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),0x00030303); p+=2;//set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc0); p+=2; //set stage 0 texture enable flags

    //FIXME: Set up signed stuff
    bool texture_signed[4] = { false, true, false, false };

    // Use button color for assignments
    texture_signed[0] = (g_Pads[0].CurrentButtons.ucAnalogButtons[XPAD_B] > BUTTON_DEADZONE);
    texture_signed[1] = (g_Pads[0].CurrentButtons.ucAnalogButtons[XPAD_A] > BUTTON_DEADZONE);
    texture_signed[2] = (g_Pads[0].CurrentButtons.ucAnalogButtons[XPAD_X] > BUTTON_DEADZONE);
    texture_signed[3] = (g_Pads[0].CurrentButtons.ucAnalogButtons[XPAD_Y] > BUTTON_DEADZONE);

//FIXME: Use other ones.. this doesn't write pgraph
#   define NV_PGRAPH_TEXFILTER0_ASIGNED                         (1 << 28)
#   define NV_PGRAPH_TEXFILTER0_RSIGNED                         (1 << 29)
#   define NV_PGRAPH_TEXFILTER0_GSIGNED                         (1 << 30)
#   define NV_PGRAPH_TEXFILTER0_BSIGNED                         (1 << 31)

    uint32_t texture_filter = 0x0;

#   define NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX             0x1
#   define NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_GAUSSIAN_3           0x2

    texture_filter |= NV_097_SET_TEXTURE_FILTER_CONVOLUTION_KERNEL_QUINCUNX << 13;

#   define NV_097_SET_TEXTURE_FILTER_MIN_BOX_LOD0              0x01
#   define NV_097_SET_TEXTURE_FILTER_MIN_TENT_LOD0             0x02
#   define NV_097_SET_TEXTURE_FILTER_MIN_BOX_NEARESTLOD        0x03
#   define NV_097_SET_TEXTURE_FILTER_MIN_TENT_NEARESTLOD       0x04
#   define NV_097_SET_TEXTURE_FILTER_MIN_BOX_TENT_LOD          0x05
#   define NV_097_SET_TEXTURE_FILTER_MIN_TENT_TENT_LOD         0x06
#   define NV_097_SET_TEXTURE_FILTER_MIN_CONVOLUTION_2D_LOD0   0x07


#   define NV_097_SET_TEXTURE_FILTER_MAG_BOX_LOD0              0x01
#   define NV_097_SET_TEXTURE_FILTER_MAG_TENT_LOD0             0x02
#   define NV_097_SET_TEXTURE_FILTER_MAG_CONVOLUTION_2D_LOD0   0x04

    texture_filter |= NV_097_SET_TEXTURE_FILTER_MIN_TENT_LOD0 << 16;
    texture_filter |= NV_097_SET_TEXTURE_FILTER_MAG_TENT_LOD0 << 24;


    if (texture_signed[0]) { texture_filter |= NV_PGRAPH_TEXFILTER0_RSIGNED; }
    if (texture_signed[1]) { texture_filter |= NV_PGRAPH_TEXFILTER0_GSIGNED; }
    if (texture_signed[2]) { texture_filter |= NV_PGRAPH_TEXFILTER0_BSIGNED; }
    if (texture_signed[3]) { texture_filter |= NV_PGRAPH_TEXFILTER0_ASIGNED; }

    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0), texture_filter); p+=2; //set stage 0 texture filters (AA!)

    pb_end(p);

    // Disable other texture stages
    p=pb_begin();

    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1),0x0003ffc0); p+=2;//set stage 1 texture enable flags (bit30 disabled)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),0x00030303); p+=2;//set stage 1 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(1),0x02022000); p+=2;//set stage 1 texture filters (no AA, stage not even used)

    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2),0x0003ffc0); p+=2;//set stage 2 texture enable flags (bit30 disabled)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),0x00030303); p+=2;//set stage 2 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(2),0x02022000); p+=2;//set stage 2 texture filters (no AA, stage not even used)

    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3),0x0003ffc0); p+=2;//set stage 3 texture enable flags (bit30 disabled)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),0x00030303); p+=2;//set stage 3 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
    pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(3),0x02022000); p+=2;//set stage 3 texture filters (no AA, stage not even used)

    pb_end(p);

    // Reset vertex attributes
    p = pb_begin();

    // Clear all attributes
    pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT, 16);
    for(int i = 0; i < 16; i++) {
      *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
    }

    pb_end(p);

    // Set vertex position attribute
    set_attrib_pointer(0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 
                       3, sizeof(Vertex), &alloc_vertices[0]);

    // Set vertex uv attribute
    set_attrib_pointer(8, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 
                       2, sizeof(Vertex), &alloc_vertices[3]);

    // Begin drawing quad
    draw_arrays(NV097_SET_BEGIN_END_OP_QUADS, 0, num_vertices);

    // Draw some text on the screen
    pb_print("Textures %d x %d (format 0x%X)\n", texture_width, texture_height, texture_fmt);
    pb_print("random: %d\n", rand());
    char signed_text[5];
    strcpy(signed_text, "    ");
    if (texture_signed[0]) { signed_text[0] = 'R'; };
    if (texture_signed[1]) { signed_text[1] = 'G'; };
    if (texture_signed[2]) { signed_text[2] = 'B'; };
    if (texture_signed[3]) { signed_text[3] = 'A'; };
    pb_print("signed: '%s'\n", signed_text);
    pb_print("pad: %d %d\n", g_Pads[0].sLThumbX,
                             g_Pads[0].sLThumbY);

    //FIXME: Print current object-size
    //FIXME: Print current texture-size
    //FIXME: Print current signedness
    //FIXME: Print current texture format

    pb_draw_text_screen();

    while(pb_busy()) {
      // Wait for completion...
    }

    // Swap buffers (if we can)
    while (pb_finished()) {
      // Not ready to swap yet
    }
  }

  // Unreachable cleanup code
  MmFreeContiguousMemory(alloc_vertices);
  pb_show_debug_screen();
  pb_kill();
  HalReturnToFirmware(HalQuickRebootRoutine);
}
