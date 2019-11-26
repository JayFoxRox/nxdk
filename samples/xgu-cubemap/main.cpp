// Meant to showcase realtime rendering of cubemaps [not working yet]

#include <hal/video.h>
#include <hal/xbox.h>
#include <math.h>
#include <pbkit/pbkit.h>
#include <xgu/xgu.h>
#include <xgu/xgux.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <xboxkrnl/xboxkrnl.h>
#include <hal/debug.h>
#include <windows.h>

static float     m_viewport[4][4];

#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

static void matrix_viewport(float out[4][4], float x, float y, float width, float height, float z_min, float z_max);
static void init_shader(void);

const unsigned int cubemap_width = 128;
const unsigned int cubemap_height = 128;
const unsigned int cubemap_depth = 128;
const XguFormat cubemap_surface_format = XGU_X8R8G8B8;
const XguFormat cubemap_texture_format =  XGU_X8R8G8B8;
void* cubemap_pixels = NULL;

static void draw_environment() {
    // We draw some points around the center
    unsigned int rows = 3;
    unsigned int colums = 3;
    float beta_step = M_PI / (float)rows;
    float alpha_step = M_PI / (float)columns;
    float beta = 0.0f;
    float distance;

#if 0
    xgux_pixel_shader(
      R"SHADER(
        !!RC1.0
        out.rgb = color.rgb;
        out.a = zero;
      )SHADER"
    );
#else
    #include "_environment.ps.inl"
#endif

    xgux_point_size(10.0f);
    xgux_begin(XGU_POINTS);
    for(int y = 0; y < rows; y++) {
      float alpha = 0.0f;
      for(int x = 0; x < columns; x++) {
        vec3 position = {
          sinf(alpha) * cosf(beta),
          cosf(alpha) * cosf(beta),
          sinf(beta);
        };
        float distance = 3.0f;
        xgux_color3f(position.x, position.y, position.z);
        xgux_vertex3f(position.x * distance,
                      position.y * distance,
                      position.z * distance);
        alpha += alpha_step;
      }
      beta += beta_step;
    }
    xgux_end();
}

static void draw_object() {

    #define VERTEX(xyz) \
      { \
        float px = (x & 0b100) ? 1.0f : 0.0f; \
        float py = (y & 0b010) ? 1.0f : 0.0f; \
        float pz = (z & 0b001) ? 1.0f : 0.0f; \
        xgux_texcoord3f(0,px*2.0f-1.0f,py*2.0f-1.0f,pz*2.0f-1.0f); \
        xgux_vertex3f(px,py,pz); \
      }

    #define QUAD(a,b,c,d) \
      VERTEX(a) \
      VERTEX(b) \
      VERTEX(c) \
      \
      VERTEX(b) \
      VERTEX(c) \
      VERTEX(d)

#if 0
    xgux_pixel_shader(
      R"SHADER(
        !!TS1.0
        texture_cubemap();
        !!RC1.0
        out.rgb = texture0.rgb;
        out.a = zero;
      )SHADER"
    );
#else
    #include "_object.ps.inl"
#endif

    xgux_begin(XGUX_TRIANGLES);
    QUAD(0b000, 0b010, 0b100, 0b110) // -z
    QUAD(0b100, 0b101, 0b110, 0b111) // +x
    QUAD(0b010, 0b011, 0b110, 0b111) // +y
    QUAD(0b000, 0b001, 0b010, 0b011) // -x
    QUAD(0b000, 0b001, 0b100, 0b101) // -y
    QUAD(0b001, 0b011, 0b101, 0b111) // +z
    xgux_end();
}

static void render_cubemap() {

    typedef struct {
      vec3 dir;
      vec3 up;
      int width;
      int height;    
    } Side;

    Side sides[6] = {
      { { 0, 0,-1}, {0, 1, 0}, cubemap_width, cubemap_depth  }, // bottom
      { { 1, 0, 0}, {0, 0, 1}, cubemap_width, cubemap_height }, // front
      { { 0, 1, 0}, {0, 0, 1}, cubemap_depth, cubemap_height }, // right
      { {-1, 0, 0}, {0, 0, 1}, cubemap_width, cubemap_height }, // back
      { { 0,-1, 0}, {0, 0, 1}, cubemap_depth, cubemap_height }, // left
      { { 0, 0, 1}, {0,-1, 0}, cubemap_width, cubemap_depth  }  // top
    };

    void* pixels = cubemap_pixels;
    
    // Render each side of the cubemap
    const z_near = 0.1f;
    const z_far = 100.0f;
    for(int i = 0; i < 6; i++) {
      Side* side = &sides[i];
      xgux_set_color_surface(cubemap_format, pixels);
      xgux_set_depth_surface(cubemap_depth_buffer);
      xgux_set_surface_dimensions(side->width, side->height);
      xgux_set_viewport(0,0,side->width, side->height);   

      // Prepare the camera matrix for rendering
      // (this is equivalent to setting up `look_at(null, side->dir, side->up))`
      vec3 left = cross(side->dir, side->up);
      float view_matrix[4*4] = {
        left,             0.0f,
        side->up,         0.0f,
        -side->dir,       0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
      };

      // We also have to set up the projection matrix
      // (this could remain the same if the cubemap width, height and depth are the same)
      float fov = 90.0f; //FIXME: Will this have to change when we rescale the cubemap?
      float aspect_ratio = dimensions[i].width / dimensions[i].height;
      //FIXME: Use different z-near so that we actually clip anything inside cube?
      perspective(&projection_matrix, fov, aspect_ratio, z_near, z_far);


      pb_erase_depth_stencil_buffer(0, 0, dimensions[i].width, dimensions[i].height); //FIXME: Do in XGU
      pb_fill(0, 0, dimensions[i].width, dimensions[i].height, 0x00000000); //FIXME: Do in XGU

      draw_environment();

      while(pb_busy()) {
          /* Wait for completion... */
      }

      // Advance pixel cursor
      pixels = (uintptr_t)pixels + dimensions[i].width * dimensions[i].height * 4;
    }

}


/* Main program function */
void main(void)
{
    uint32_t *p;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    int status = pb_init();
    if (status) {
        debugPrint("pb_init Error %d\n", status);
        Sleep(2000);
        XReboot();
        return;
    }

    //FIXME: Workaround XQEMU bug
    Sleep(2000);

    pb_show_front_screen();

    matrix_viewport(m_viewport, 0, 0, width, height, 0, (float)0xFFFF);

    /* Create cubemap surface */
    cubemap_pixels = xgux_create_cubemap_surface(cubemap_width, cubemap_height, cubemap_surface);

#if 0
    // We register a GPU tile, which means the memory will be accessed differently, for faster access
    xgux_register_tile_for_cubemap_surface(cubemap_width, cubemap_height, cubemap_surface);
#endif

    while(1) {
        pb_wait_for_vbl();
        pb_reset();
        pb_target_back_buffer();

#if 0
        // Prepare our cubemap
        render_cubemap();

        // Now render the world
        xgux_set_color_framebuffer(XGU_COLOR_R8G8B8A8, color_buffer);
        xgux_set_depth_framebuffer(XGU_DEPTH_16, true, depth_buffer);
        xgux_set_surface_dimensions(640,480);
        xgux_set_viewport(0,0,640,480);
#endif

        pb_erase_depth_stencil_buffer(0, 0, width, height); //FIXME: Do in XGU
        pb_fill(0, 0, width, height, 0x00000000); //FIXME: Do in XGU
        pb_erase_text_screen();

        while(pb_busy()) {
            /* Wait for completion... */
        }

        // Set up camera
        look_at(view_matrix, 7.0f, 7.0f, 7.0f
                             0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f);
        perspective(projection_matrix, 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
        viewport_matrix();

        draw_environment();
#if 0
        draw_object();
#endif

        /* Draw some text on the screen */
        pb_print("Cubemap Demo\n");
        pb_draw_text_screen();

        while(pb_busy()) {
            /* Wait for completion... */
        }

        /* Swap buffers (if we can) */
        while (pb_finished()) {
            /* Not ready to swap yet */
        }
    }

    /* Unreachable cleanup code */
    pb_kill();
    
    return 0;
}
