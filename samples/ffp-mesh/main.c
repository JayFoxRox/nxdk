/*
 * This sample provides a very basic demonstration of 3D rendering on the Xbox,
 * using pbkit. Based on the pbkit demo sources.
 */
#include <hal/video.h>
#include <hal/xbox.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <pbkit/pbkit.h>
#include <xgu/xgu.h>
#include <xgu/xgux.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <hal/debug.h>
#include "math3d.h"

static uint32_t *alloc_vertices;
static uint32_t  num_vertices;
static uint32_t  num_indices;

MATRIX m_model, m_view, m_proj, m_mvp;

VECTOR v_obj_rot     = {  0,   0,   0,  1 };
VECTOR v_obj_scale   = {  1,   1,   1,  1 };
VECTOR v_obj_pos     = {  0,   0,   0,  1 };
VECTOR v_cam_loc     = {  0,   0, 165,  1 };
VECTOR v_cam_rot     = {  0,   0,   0,  1 };
VECTOR v_light_color = {  1,   1,   1,  1 };
VECTOR v_light_pos   = {  0, 140,   0,  1 };

float light_ambient  = 0.125f;

#pragma pack(1)
typedef struct Vertex {
    float pos[3];
    float normal[3];
    float texcoord[2];
} Vertex;

#pragma pack()

#include "verts.h"
#include "texture.h"

#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

struct {
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    void     *addr;
} texture;

#define MAXRAM 0x03FFAFFF

static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max);
static void init_shader(void);
static void init_textures(void);
static void set_attrib_pointer(unsigned int index, unsigned int format, unsigned int size, unsigned int stride, const void* data);
static void draw_arrays(unsigned int mode, int start, int count);
static void draw_indices(void);

/* Main program function */
int main(void)
{
    uint32_t *p;
    int       i, status;
    int       width, height;
    int       start, last, now;
    int       fps, frames, frames_total;
    float     m_viewport[4][4];

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    if ((status = pb_init())) {
        debugPrint("pb_init Error %d\n", status);
        Sleep(2000);
        return 1;
    }

    pb_show_front_screen();

    /* Basic setup */
    width = pb_back_buffer_width();
    height = pb_back_buffer_height();

    /* Load constant rendering things (shaders, geometry) */
    init_shader();
    init_textures();

    alloc_vertices = MmAllocateContiguousMemoryEx(sizeof(vertices), 0, MAXRAM, 0, 0x404);
    memcpy(alloc_vertices, vertices, sizeof(vertices));
    num_vertices = sizeof(vertices)/sizeof(vertices[0]);
    num_indices = sizeof(indices)/sizeof(indices[0]);

    /* Setup to determine frames rendered every second */
    start = now = last = GetTickCount();
    frames_total = frames = fps = 0;

    /* Create view matrix (our camera is static) */
    matrix_unit(m_view);
    create_world_view(m_view, v_cam_loc, v_cam_rot);

    while(1) {
        pb_wait_for_vbl();
        pb_reset();
        pb_target_back_buffer();

        /* Clear depth & stencil buffers */
        pb_erase_depth_stencil_buffer(0, 0, width, height);
        pb_fill(0, 0, width, height, 0xff202020);
        pb_erase_text_screen();

        /* Tilt and rotate the object a bit */
        v_obj_rot[0] = (float)((now-start))/1000.0f * M_PI * -0.25f;

        /* Create local->world matrix given our updated object */
        matrix_unit(m_model);
        matrix_rotate(m_model, m_model, v_obj_rot);
        matrix_scale(m_model, m_model, v_obj_scale);
        matrix_translate(m_model, m_model, v_obj_pos);

        while(pb_busy()) {
            /* Wait for completion... */
        }

        /*
         * Setup texture stages
         */

        /* Enable texture stage 0 */
        /* FIXME: Use constants instead of the hardcoded values below */
        p = pb_begin();
        p = pb_push2(p,NV20_TCL_PRIMITIVE_3D_TX_OFFSET(0),(DWORD)texture.addr & 0x03ffffff,0x0001122a); //set stage 0 texture address & format
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_PITCH(0),texture.pitch<<16); //set stage 0 texture pitch (pitch<<16)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_NPOT_SIZE(0),(texture.width<<16)|texture.height); //set stage 0 texture width & height ((witdh<<16)|height)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(0),0x00030303);//set stage 0 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(0),0x4003ffc0); //set stage 0 texture enable flags
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(0),0x04074000); //set stage 0 texture filters (AA!)
        pb_end(p);

        /* Disable other texture stages */
        p = pb_begin();
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(1),0x0003ffc0);//set stage 1 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(2),0x0003ffc0);//set stage 2 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_ENABLE(3),0x0003ffc0);//set stage 3 texture enable flags (bit30 disabled)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(1),0x00030303);//set stage 1 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(2),0x00030303);//set stage 2 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_WRAP(3),0x00030303);//set stage 3 texture modes (0x0W0V0U wrapping: 1=wrap 2=mirror 3=clamp 4=border 5=clamp to edge)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(1),0x02022000);//set stage 1 texture filters (no AA, stage not even used)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(2),0x02022000);//set stage 2 texture filters (no AA, stage not even used)
        p = pb_push1(p,NV20_TCL_PRIMITIVE_3D_TX_FILTER(3),0x02022000);//set stage 3 texture filters (no AA, stage not even used)
        pb_end(p);

int r;
#define OPT r = rand()%2; pb_print("%d, %d\n", __LINE__, r); if(r)

    /* Create projection matrix */
    matrix_unit(m_proj);
    create_view_screen(m_proj, (float)width/(float)height, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10000.0f);

    /* Create viewport matrix, combine with projection */
    matrix_viewport(m_viewport, 0, 0, width, height, 0.0f, 1.0f);
    matrix_multiply(m_proj, m_proj, (float*)m_viewport);

static int t = 0;
t++;
t %= 100;
if (t > 50) {

pb_print("FFP\n");

    float m_identity[4*4];
    matrix_unit(m_identity);

    /* Set up some default GPU state (should be done in xgux_init maybe? currently partially done in pb_init) */
    {
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
    }


    /* Set up all states for hardware vertex pipeline */
    p = pb_begin();
    p = xgu_set_transform_execution_mode(p, XGU_FIXED, XGU_RANGE_MODE_PRIVATE);
    //FIXME: p = xgu_set_fog_enable(p, false);

    float m_p[4*4];
    matrix_copy(m_p, m_proj); // Already contains viewport matrix!

    float m_mv[4*4];
    matrix_multiply(m_mv, m_model, m_view);
//OPT matrix_multiply(m_mv, m_view, m_model);

    float m_mvp[4*4];
    matrix_multiply(m_mvp, m_mv, m_p);
//OPT matrix_multiply(m_mvp, m_p, m_mv);


    float mt_p[4*4];
    matrix_transpose(mt_p, m_p);
//OPT matrix_copy(mt_p, m_p);

    float mt_mvp[4*4];
    matrix_transpose(mt_mvp, m_mvp);
//OPT matrix_copy(mt_mvp, m_mvp);


    p = xgu_set_projection_matrix(p, m_identity); //FIXME: Unused in XQEMU
    p = xgu_set_composite_matrix(p, mt_mvp); //FIXME: Always used in XQEMU?
    p = xgu_set_viewport_offset(p, 0.0f, 0.0f, 0.0f, 0.0f);
    p = xgu_set_viewport_scale(p, 1.0f, 1.0f, 1.0f, 1.0f); //FIXME: Ignored?!
    //p = xgu_set_viewport_scale(p, 1.0f / width, 1.0f / height, 1.0f / (float)0xFFFF, 1.0f); //FIXME: Ignored?!


    //FIXME: Fix depth values [why is this?]

    //FIXME: Set up light


    pb_end(p);
} else {
        /* Send shader constants
         *
         * WARNING: Changing shader source code may impact constant locations!
         * Check the intermediate file (*.inl) for the expected locations after
         * changing the code.
         */
        p = pb_begin();

p = xgu_set_transform_execution_mode(p, XGU_PROGRAM, XGU_RANGE_MODE_USER);

        /* Set shader constants cursor at C0 */
        p = pb_push1(p, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_ID, 96);

        /* Send the model matrix */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
        memcpy(p, m_model, 16*4); p+=16;

        /* Send the view matrix */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
        memcpy(p, m_view, 16*4); p+=16;

        /* Send the projection matrix */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 16);
        memcpy(p, m_proj, 16*4); p+=16;

        /* Send camera position */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
        memcpy(p, v_cam_loc, 4*4); p+=4;

        /* Send light position */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
        memcpy(p, v_light_pos, 4*4); p+=4;

        /* Send light color */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
        memcpy(p, v_light_color, 4*4); p+=4;

        /* Send ambient light intensity */
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
        memcpy(p, &light_ambient, 4); p+=4;

        /* Send shader constants 0 2 64 1 */
        float constants_0[4] = {0, 2, 64, 1};
        pb_push(p++, NV20_TCL_PRIMITIVE_3D_VP_UPLOAD_CONST_X, 4);
        memcpy(p, constants_0, 4*4); p+=4;

        pb_end(p);
}
        
        /* Clear all attributes */
        for(i = 0; i < XGU_ATTRIBUTE_COUNT; i++) {
            xgux_set_attrib_pointer(i, XGU_FLOAT, 0, 0, NULL);
        }

        /*
         * Setup vertex attributes
         */

        xgux_set_attrib_pointer(XGU_VERTEX_ARRAY, XGU_FLOAT, 3,
                           sizeof(Vertex), &alloc_vertices[0]);
        xgux_set_attrib_pointer(XGU_NORMAL_ARRAY, XGU_FLOAT, 3,
                           sizeof(Vertex), &alloc_vertices[3]);
        assert(XGU_TEXCOORD0_ARRAY == 9);
        xgux_set_attrib_pointer(XGU_TEXCOORD0_ARRAY, XGU_FLOAT, 2, //FIXME: Bug in XQEMU nv2a_regs.h
                           sizeof(Vertex), &alloc_vertices[6]);

        // 8 is what Cg uses :'(
        xgux_set_attrib_pointer(8, XGU_FLOAT, 2, //FIXME: Bug in XQEMU nv2a_regs.h
                           sizeof(Vertex), &alloc_vertices[6]);


        /* Begin drawing triangles */
        xgux_draw_elements16(XGU_TRIANGLES, indices, num_indices * 2);

        /* Draw some text on the screen */
        pb_print("Mesh Demo\n");
        pb_print("Frames: %d\n", frames_total);
        if (fps > 0) {
            pb_print("FPS: %d", fps);
        }
        pb_draw_text_screen();

        while(pb_busy()) {
            /* Wait for completion... */
        }

        /* Swap buffers (if we can) */
        while (pb_finished()) {
            /* Not ready to swap yet */
        }

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

/* Construct a viewport transformation matrix */
static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max)
{
    memset(out, 0, 4*4*sizeof(float));
    out[0][0] = width/2.0f;
    out[3][0] = x + width/2.0f;

    out[1][1] = height/-2.0f;
    out[3][1] = y + height/2.0f;

    out[2][2] = (z_max - z_min)*0xFFFF;
    out[3][2] = z_min*0xFFFF;

    out[3][3] = 1.0f;
}

/* Load the shader we will render with */
static void init_shader(void)
{
    uint32_t *p;
    int       i;

    /* Setup vertex shader */
    uint32_t vs_program[] = {
        #include "vs.inl"
    };

    p = pb_begin();

    /* Set run address of shader */
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);

    /* Set execution mode */
    p = pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
                 MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
                 | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));

    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);
    pb_end(p);

    /* Set cursor and begin copying program */
    p = pb_begin();
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);
    pb_end(p);

    /* Copy program instructions (16-bytes each) */
    for (i=0; i<sizeof(vs_program)/16; i++) {
        p = pb_begin();
        pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
        memcpy(p, &vs_program[i*4], 4*4);
        p+=4;
        pb_end(p);
    }

    /* Setup fragment shader */
    p = pb_begin();
    #include "ps.inl"
    pb_end(p);
}

/* Load the textures we will render with */
static void init_textures(void)
{
    texture.width = texture_width;
    texture.height = texture_height;
    texture.pitch = texture.width*4;
    texture.addr = MmAllocateContiguousMemoryEx(texture.pitch*texture.height, 0, MAXRAM, 0, 0x404);
    memcpy(texture.addr, texture_rgba, sizeof(texture_rgba));
}
