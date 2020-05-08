#include <math.h>
#include <pbkit/pbkit.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#if defined(NXDK)
#include <hal/video.h>
#include <hal/xbox.h>
#include <xboxkrnl/xboxkrnl.h>
#include <hal/debug.h>
#else
#define debugPrint(...) printf(__VA_ARGS__)
#endif

#include "gl.h"
#include "glu.h"

static void(*displayFunc)(void) = NULL;
static void(*reshapeFunc)(int w, int h) = NULL;

void glutInit(int* argc, char* argv[]) {

  //FIXME: Do most of this in glutCreateWindow instead?

  int status;
  if ((status = pb_init())) {
    debugPrint("pb_init Error %d\n", status);
#if defined(NXDK)
    Sleep(2000);
#endif
    return;
  }

  pb_show_front_screen();

#if 0
  /* Basic setup */
  width = pb_back_buffer_width();
  height = pb_back_buffer_height();
  glViewport(0, 0, width, height);
#endif

  // Setup fixed vertex processing
#if 0
    p = pb_begin();

    /* Set execution mode */
    pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
        MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
        | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));
    p += 2;

    pb_end(p);
#endif

  // Setup register combiner
  //FIXME: Emulate FFP or use https://www.khronos.org/registry/OpenGL/extensions/NV/NV_register_combiners.txt
  uint32_t* p = pb_begin();
  #include "fp.inl"
  pb_end(p);
}

void glutInitWindowSize(int width, int height) {
  //FIXME: Possibly use this to pick default viewport or even display mode?
}

#define GLUT_DOUBLE 0x1
#define GLUT_RGB 0x2
#define GLUT_DEPTH 0x4

void glutInitDisplayMode(int flags) {
  // Don't need this for now
}

void glutCreateWindow(const char* title) {
  // We don't need a window on Xbox  
}

void glutDisplayFunc(void(*func)(void)) {
  displayFunc = func;
}

void glutReshapeFunc(void(*func)(int w, int h)) {
  reshapeFunc = func;  
}

//FIXME: Avoid waiting for vsync twice?
void glutSwapBuffers(void) {
  pb_wait_for_vbl();
  pb_reset();
  pb_target_back_buffer();

  glFinish();
}

void glutMainLoop(void) {
  // FIXME: this probably shouldn't have to call reshape, but we don't have save GL defaults
  int width = pb_back_buffer_width();
  int height = pb_back_buffer_height();
  reshapeFunc(width, height);

  int       start, last, now;
  int       fps, frames, frames_total;

#if defined(NXDK)
  /* Setup to determine frames rendered every second */
  start = now = last = GetTickCount();
  frames_total = frames = fps = 0;
#endif

  while(true) {
    displayFunc();

#if 0
  //FIXME: Draw window title?
  pb_print("Frames: %d\n", frames_total);
  if (fps > 0) {
      pb_print("FPS: %d", fps);
  }
  pb_draw_text_screen();
  pb_erase_text_screen();
#endif

    //FIXME: This is bad, we should be calling the idle func during this time instead!
    glFinish();
#if defined(NXDK)
    frames++;
    frames_total++;

    /* Latch FPS counter every second */
    now = GetTickCount();
    if ((now-last) > 1000) {
        fps = frames;
        frames = 0;
        last = now;
    }
#else
    printf("Leaving mainloop\n");
    break;
#endif
  }

  pb_show_debug_screen();
  pb_kill();
}
