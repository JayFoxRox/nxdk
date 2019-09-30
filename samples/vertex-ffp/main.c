/*
 * This sample provides a very basic demonstration of 3D rendering on the Xbox,
 * using pbkit. Based on the pbkit demo sources.
 */
#include <hal/video.h>
#include <hal/xbox.h>
#include <math.h>
#include <pbkit/pbkit.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <xboxkrnl/xboxkrnl.h>
#include <hal/debug.h>

#include <xgu/xgu.h>
#include <xgu/xgux.h>

#include "xgu_extra.h"
#include "xgux_extra.h"

#define MAX_RAM 0x3ffb000
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

static float     m_viewport[4][4];

typedef struct {
    float pos[3];
    float color[3];
} __attribute__((packed)) ColoredVertex;

static const ColoredVertex verts[] = {
    //  X     Y     Z       R     G     B
    {{-1.0, -1.0,  1.0}, { 0.1,  0.1,  0.6}}, /* Background triangle 1 */
    {{-1.0,  1.0,  1.0}, { 0.0,  0.0,  0.0}},
    {{ 1.0,  1.0,  1.0}, { 0.0,  0.0,  0.0}},
    {{-1.0, -1.0,  1.0}, { 0.1,  0.1,  0.6}}, /* Background triangle 2 */
    {{ 1.0,  1.0,  1.0}, { 0.0,  0.0,  0.0}},
    {{ 1.0, -1.0,  1.0}, { 0.1,  0.1,  0.6}},
    {{-1.0, -1.0,  1.0}, { 1.0,  0.0,  0.0}}, /* Foreground triangle */
    {{ 0.0,  1.0,  1.0}, { 0.0,  1.0,  0.0}},
    {{ 1.0, -1.0,  1.0}, { 0.0,  0.0,  1.0}},
};
static ColoredVertex *alloc_vertices;

//FIXME: Replace by glm or something?
#include "matrix.h"

/* Main program function */
int main(void)
{
    uint32_t *p;
    int       start, last, now;
    int       fps, frames, frames_total;

    /* Set display mode */
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    /* Start GPU */
    int status = pb_init();
    if (status) {
        debugPrint("pb_init Error %d\n", status);
        Sleep(2000);
        return 1;
    }

    //FIXME: Remove.
    //       This works around a race-condition in XQEMU.
    //       It looks like pb_init uses `debugPrint()` so the GPU and CPU draw
    //       at the same time. Depending on the order of operations, this
    //       breaks the XQEMU surface cache.
    Sleep(100);

    /* Display GPU result on display */
    pb_show_front_screen();

    /* Basic setup */
    int width = pb_back_buffer_width();
    int height = pb_back_buffer_height();

    /* Setup fragment shader */
    p = pb_begin();
    #include "ps.inl"
    pb_end(p);

    /* Load constant geometry */
    alloc_vertices = MmAllocateContiguousMemoryEx(sizeof(verts), 0, MAX_RAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    memcpy(alloc_vertices, verts, sizeof(verts));

    /* Setup viewport matrix */
    //FIXME: This matrix should be transposed
    matrix_viewport(m_viewport, 0, 0, width, height, 0, (float)0xFFFF);

    //FIXME: This is already set in pbkit probably?
    //p = xgu_set_viewport_offset(p, 0.0f, 0.0f, 0.0f, 0.0f);
    //p = xgu_set_viewport_scale(p, 1.0f / width, 1.0f / height, 1.0f / (float)0xFFFF, 1.0f);

    /* Setup to determine frames rendered every second */
    start = now = last = GetTickCount();
    frames_total = frames = fps = 0;

    while(1) {
        pb_wait_for_vbl();
        pb_reset();
        pb_target_back_buffer();

        /* Clear depth & stencil buffers */
        pb_erase_depth_stencil_buffer(0, 0, width, height);
        pb_fill(0, 0, width, height, 0x00000000);
        pb_erase_text_screen();

        xgux_wait_for_idle();

        /* A generic identity matrix */
        const float identity4x4[4*4] = {
          1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f
        };

        /* Set up all states for hardware vertex pipeline */
        p = pb_begin();

        p = pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
                     MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED)
                     | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));


        //FIXME: p = xgu_set_skinning(p, XGU_SKINNING_OFF);
        //FIXME: p = xgu_set_normalization(p, false);
        p = xgu_set_lighting_enable(p, false);
#if 0
        for(int i = 0; i < XGU_TEXTURE_COUNT; i++) {
            //FIXME: p = xgu_set_texgen(p, XGU_TEXGEN_OFF);
            p = xgu_set_texture_matrix_enable(p, i, false);
        }
#endif
        //FIXME: p = xgu_set_fog_enable(p, false);
        p = xgu_set_projection_matrix(p, identity4x4); //FIXME: Unused in XQEMU
//        p = xgu_set_composite_matrix(p, m_viewport); //FIXME: Always used in XQEMU?

//FIXME: Why do I have to transpose this?!
//
//       In FFP this is used like this: tPosition * compositeMat
//        => requires transposed copy of 4x4 matrix in NV097_SET_COMPOSITE_MATRIX
//        -- pb_push_transposed_matrix:
//        => v0 = m0,  m4,  m8,  m12
//        => v1 = m1,  m5,  m9,  m13
//        => v2 = m2,  m6,  m10, m14
//        => v3 = m3,  m7,  m11, m15
//        -- XQEMU NV097_SET_COMPOSITE_MATRIX:
//        => c0 = v0 = out[0][0], 0, 0, out[3][0]
//        => c1 = v1
//        => c2 = v2
//        => c3 = v3
//
//       In VP, this is used like this: mul(float4(I.position.xyz, 1.0f), m_viewport)
//        => requires unmodified copy of 4x4 matrix in constant space
//        => c0 = m0,  m1,  m2,  m3
//        => c1 = m4,  m5,  m6,  m7
//        => c2 = m8,  m9,  m10, m11
//        => c3 = m12, m13, m14, m15
//
//       So why is one transposed and the other one isn't? They run the same MUL?

          
        p = pb_push_transposed_matrix(p, NV097_SET_COMPOSITE_MATRIX, m_viewport);


        for(int i = 0; i < XGU_WEIGHT_COUNT; i++) {
            p = xgu_set_model_view_matrix(p, i, identity4x4); //FIXME: Not sure when used?
            p = xgu_set_inverse_model_view_matrix(p, i, identity4x4); //FIXME: Not sure when used?
        }
        pb_end(p);

        /* Clear all attributes */
        for(int i = 0; i < XGU_ATTRIBUTE_COUNT; i++) {
            xgux_set_attrib_pointer(i, XGU_FLOAT, 0, 0, NULL);
        }

        /* Set vertex attributes */
        xgux_set_attrib_pointer(XGU_VERTEX_ARRAY, XGU_FLOAT, 3, sizeof(ColoredVertex), &alloc_vertices->pos);
        xgux_set_attrib_pointer(XGU_COLOR_ARRAY, XGU_FLOAT, 3, sizeof(ColoredVertex), &alloc_vertices->color);

        /* Begin drawing triangles */
        xgux_draw_arrays(XGU_TRIANGLES, 0, ARRAY_SIZE(verts));

        /* Draw some text on the screen */
        pb_print("Vertex-FFP Demo\n");
        pb_print("Frames: %d\n", frames_total);
        if (fps > 0) {
            pb_print("FPS: %d", fps);
        }
        pb_draw_text_screen();

        xgux_wait_for_idle();
        xgux_swap();

        frames++;
        frames_total++;

        /* Latch FPS counter every second */
        now = GetTickCount();
        if ((now-last) > 1000) {
            fps = frames;
            frames = 0;
            last = now;
        }
    }

    /* Unreachable cleanup code */
    MmFreeContiguousMemory(alloc_vertices);
    pb_show_debug_screen();
    pb_kill();

    return 0;
}
