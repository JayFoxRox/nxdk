#ifndef __GL_H__
#define __GL_H__

// Some code stolen from mesa:
// Copyright (C) 1999-2005  Brian Paul   All Rights Reserved.

#if 0
#include <assert.h>
#else
#define assert(x)
#endif

#include <stdbool.h>
typedef int bool;

#include <stdint.h>

#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

typedef int GLenum;
typedef int GLint;
typedef float GLfloat;
typedef int GLsizei;
typedef double GLdouble;
typedef unsigned int GLbitfield;

#define GL_PROJECTION 0
#define GL_MODELVIEW 1
#define GL_TRIANGLES NV097_SET_BEGIN_END_OP_TRIANGLES
#define GL_QUADS NV097_SET_BEGIN_END_OP_QUADS

#define GL_COLOR_BUFFER_BIT 0x1
#define GL_DEPTH_BUFFER_BIT 0x2


static float projectionMatrix[4*4];
static float modelViewMatrix[4*4];

static float* targetMatrix = NULL;

static void matrix_identity(float* out) {
  memset(out, 0x00, 4 * 4 * sizeof(float));
  for(int i = 0; i < 4; i++) {
    out[i*4+i] = 1.0f;
  }
}

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col) product[(col<<2)+row]
static void matmul4( GLfloat *product, const GLfloat *a, const GLfloat *b )
{
   GLint i;
   for (i = 0; i < 4; i++) {
      const GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
      P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
      P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
      P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
      P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   }
}
#undef A
#undef B
#undef P


static void update_matrices() {
  uint32_t *p;

  p = pb_begin();
  pb_push(p++, NV097_SET_PROJECTION_MATRIX, 4 * 4); //bit 30 means all params go to same register 0x1810
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      *(p++) = *(uint32_t*)&projectionMatrix[i*4+j];
    }
  }
  pb_end(p);

  //FIXME: Use the real modelview
  p = pb_begin();
  pb_push(p++, NV097_SET_MODEL_VIEW_MATRIX, 4 * 4); //bit 30 means all params go to same register 0x1810
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      *(p++) = *(uint32_t*)&modelViewMatrix[i*4+j];
    }
  }
  pb_end(p);

  // Generate a composite matrix
  float compositeMatrix[4*4];
  matmul4(compositeMatrix, modelViewMatrix, projectionMatrix);
  p = pb_begin();
  pb_push(p++, NV097_SET_COMPOSITE_MATRIX, 4 * 4); //bit 30 means all params go to same register 0x1810
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      *(p++) = *(uint32_t*)&compositeMatrix[i*4+j];
    }
  }
  pb_end(p);
}


#if 0

  /* Clear all attributes */
  pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT,16);
  for(i = 0; i < 16; i++) {
      *(p++) = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
  }
  pb_end(p);

  /* Set one attribute */
  pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + index*4,
      MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, format) | \
      MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, size) | \
      MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, stride));
  p += 2;
  pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + index*4, (uint32_t)data & 0x03ffffff);
  p += 2;

#endif

void glBegin(GLenum mode) {
  update_matrices();
  uint32_t *p = pb_begin();
  pb_push1(p, NV097_SET_BEGIN_END, mode); p += 2;
  pb_end(p);
}

void glEnd(void) {
  uint32_t *p = pb_begin();
  pb_push1(p,NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END); p += 2;
  pb_end(p);
}

#if 0
/* Send draw commands for the triangles */
//FIXME: void glDrawArrays(GLenum mode, GLint first, GLsizei count);
static void draw_arrays(unsigned int mode, int start, int count)
{
  update_matrices();

  uint32_t *p = pb_begin();
  pb_push1(p, NV097_SET_BEGIN_END, mode); p += 2;

  pb_push(p++,0x40000000|NV097_DRAW_ARRAYS,1); //bit 30 means all params go to same register 0x1810
  *(p++) = MASK(NV097_DRAW_ARRAYS_COUNT, (count-1)) | MASK(NV097_DRAW_ARRAYS_START_INDEX, start);

  pb_push1(p,NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END); p += 2;
  pb_end(p);
}
#endif

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
  //FIXME: Do this
  //pb_set_viewport(x, y, width, height, 0.0f, 1.0f);
}

void glMatrixMode(GLenum mode) {

  //FIXME: Should be done in some constructor function,
  //       Just not sure if nxdk does those yet :(
  static bool init = false;
  if (init == false) {
    
    matrix_identity(projectionMatrix);
    matrix_identity(modelViewMatrix);

    init = true;
  }

  if (mode == GL_PROJECTION) {
    targetMatrix = projectionMatrix;
  } else if (mode == GL_MODELVIEW) {
    targetMatrix = modelViewMatrix;
  }
}

void glLoadIdentity(void) {
  matrix_identity(targetMatrix);
}


void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearval, GLdouble farval) {
  //FIXME: Multiply this onto the existing stack - don't overwrite


   GLfloat m[16];

#define M(row,col)  m[col*4+row]
   M(0,0) = 2.0F / (right-left);
   M(0,1) = 0.0F;
   M(0,2) = 0.0F;
   M(0,3) = -(right+left) / (right-left);

   M(1,0) = 0.0F;
   M(1,1) = 2.0F / (top-bottom);
   M(1,2) = 0.0F;
   M(1,3) = -(top+bottom) / (top-bottom);

   M(2,0) = 0.0F;
   M(2,1) = 0.0F;
   M(2,2) = -2.0F / (farval-nearval);
   M(2,3) = -(farval+nearval) / (farval-nearval);

   M(3,0) = 0.0F;
   M(3,1) = 0.0F;
   M(3,2) = 0.0F;
   M(3,3) = 1.0F;
#undef M

  float t[4 * 4];
  matmul4( t, m, targetMatrix);
  memcpy(targetMatrix, t, sizeof(t));
}

//FIXME:   glScalef(1, -1, 1);           /* Invert Y axis so increasing Y goes down. */
void glScalef(GLfloat x, GLfloat y, GLfloat z) {
}

//FIXME:   glTranslatef(0, -h, 0);       /* Shift origin up to upper-left corner. */
void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
}

//FIXME:   glRotatef(60, 1.0, 0.0, 0.0); (used to rotate cube)
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
}

#define GL_LIGHT0 0
#define GL_DIFFUSE 0
#define GL_POSITION 1

void glLightfv(GLenum light, GLenum pname, GLfloat* param) {
  //FIXME: Add light support
}

#define GL_DEPTH_TEST 0
#define GL_LIGHTING 1

void glEnable(GLenum cap) {
  switch(cap) {
  default:
    assert(false);
  }
}

void glClear(GLbitfield mask) {

  int width = pb_back_buffer_width();
  int height = pb_back_buffer_height();

  /* Clear depth & stencil buffers */
  if (mask & GL_DEPTH_BUFFER_BIT) {
    pb_erase_depth_stencil_buffer(0, 0, width, height);
  }

  if (mask & GL_COLOR_BUFFER_BIT) {
    pb_fill(0, 0, width, height, 0x00FF00FF);
  }
}

static void vertex_attribute_4f(int index, float x, float y, float z, float w) {
  int reg = NV097_SET_VERTEX_DATA4F_M + (index * 4) * 4;

  uint32_t *p = pb_begin();
  pb_push(p++, reg, 4); //bit 30 means all params go to same register 0x1810
  *(p++) = *(uint32_t*)&x;
  *(p++) = *(uint32_t*)&y;
  *(p++) = *(uint32_t*)&z;
  *(p++) = *(uint32_t*)&w;
  pb_end(p);
}

static void vertex_attribute_3f(int index, float x, float y, float z) {
  vertex_attribute_4f(index, x, y, z, 1.0f);
}

static void vertex_attribute_2i(int index, int16_t x, int16_t y) {
  int reg = NV097_SET_VERTEX_DATA2S + index * 4;

  uint32_t *p = pb_begin();
  pb_push(p++, reg, 1);
  *(p++) = x | (y << 16);
  pb_end(p);
}

void glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
  vertex_attribute_3f(3, red, green, blue);
}


void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
  vertex_attribute_3f(0, x, y, z);
}

void glVertex3fv(const GLfloat * v) {
  glVertex3f(v[0], v[1], v[2]);
}

void glVertex2i(GLint x, GLint y) {
  vertex_attribute_2i(0, x, y);
}

void glNormal3f(GLfloat x, GLfloat y, GLfloat z) {
  vertex_attribute_3f(2, x, y, z);
}

void glNormal3fv(const GLfloat * v) {
  glNormal3f(v[0], v[1], v[2]);
}


void glFlush(void) {
  //FIXME: Send command stream only, not sure why we wait here, check pbkit docs
  while(pb_busy()) {
      /* Wait for completion... */
  }
}

void glFinish(void) {
  //FIXME: Why does pb_finished swap buffers? check pbkit docs
  /* Swap buffers (if we can) */
  while (pb_finished()) {
      /* Not ready to swap yet */
  }
}

#endif
