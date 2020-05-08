#ifndef __GL_H__
#define __GL_H__

// Some code stolen from mesa:
// Copyright (C) 1999-2005  Brian Paul   All Rights Reserved.

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#define inline
#include <xgu/xgu.h>
#undef inline

#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))

typedef unsigned char GLubyte;
typedef short GLshort;
typedef int GLenum;
typedef int GLint;
typedef float GLfloat;
typedef int GLsizei;
typedef double GLdouble;
typedef unsigned int GLbitfield;

#define GL_PROJECTION 0
#define GL_MODELVIEW 1
#define GL_TRIANGLES NV097_SET_BEGIN_END_OP_TRIANGLES
#define GL_TRIANGLE_FAN NV097_SET_BEGIN_END_OP_TRIANGLE_FAN
#define GL_QUADS NV097_SET_BEGIN_END_OP_QUADS

#define GL_COLOR_BUFFER_BIT 0x1
#define GL_DEPTH_BUFFER_BIT 0x2


static float projectionMatrix[4*4];
static float modelViewMatrix[4*4];

static float* targetMatrix = NULL;

/* A generic identity matrix */
const float m_identity[4*4] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

static void matrix_identity(float* out) {
  memset(out, 0x00, 4 * 4 * sizeof(float));
  for(int i = 0; i < 4; i++) {
    out[i*4+i] = 1.0f;
  }
}

static void _math_matrix_scale(GLfloat *m, GLfloat x, GLfloat y, GLfloat z ) {
   m[0] *= x;   m[4] *= y;   m[8]  *= z;
   m[1] *= x;   m[5] *= y;   m[9]  *= z;
   m[2] *= x;   m[6] *= y;   m[10] *= z;
   m[3] *= x;   m[7] *= y;   m[11] *= z;
}


bool invert(float invOut[16], const float m[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
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

void ortho(float* r, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble nearval, GLdouble farval) {
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

  memcpy(r, m, sizeof(m));
}

static void printMatrix(float* f) {
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      printf("%.3f\t", f[i*4+j]);
    }
    printf("\n");
  }
}

static void transposeMatrix(float* out, float* in) {
  float t[4*4];
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < 4; j++) {
      t[i*4+j] = in[i+4*j];
    }
  } 
  memcpy(out, t, sizeof(t));
}

static void update_matrices() {
  uint32_t *p;

  //matrix_identity(projectionMatrix);
  matrix_identity(modelViewMatrix);

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
      p = xgu_set_texture_matrix(p, i, m_identity);
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


  float tmp[4*4];

  // Undo the viewport transform
  //FIXME: Use actual viewport settings
  float undoViewport[4*4];
  ortho(tmp, 0.0f, 640.0f, 0.0f, 480.0f, 0.0f, 0xFFFFFF);
  invert(undoViewport, tmp);

printf("p:\n");printMatrix(projectionMatrix);printf("\n");
printf("mv:\n");printMatrix(modelViewMatrix);printf("\n");

  // Generate a composite matrix
  float compositeMatrix[4*4];
  matmul4(compositeMatrix, modelViewMatrix, projectionMatrix);


  memcpy(tmp, compositeMatrix, sizeof(tmp));
//  matmul4(compositeMatrix, tmp, undoViewport);

printf("c:\n");printMatrix(compositeMatrix);printf("\n");
transposeMatrix(compositeMatrix, compositeMatrix);
printf("ct:\n");printMatrix(compositeMatrix);printf("\n");

  p = pb_begin();
  p = xgu_set_composite_matrix(p, compositeMatrix); //FIXME: Always used in XQEMU?
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
  p = xgu_begin(p, mode);
  pb_end(p);
}

void glEnd(void) {
  uint32_t *p = pb_begin();
  p = xgu_end(p);
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
  ortho(m, left, right, bottom, top, nearval, farval);

  float t[4 * 4];
  matmul4( t, targetMatrix, m);
  memcpy(targetMatrix, t, sizeof(t));

//  memcpy(targetMatrix, m, sizeof(m));
}


//FIXME:   glScalef(1, -1, 1);           /* Invert Y axis so increasing Y goes down. */
void glScalef(GLfloat x, GLfloat y, GLfloat z) {
  float m[4*4];
  matrix_identity(m);
  _math_matrix_scale(m, x, y, z);

  float t[4 * 4];
  matmul4( t, targetMatrix, m);
  memcpy(targetMatrix, t, sizeof(t));
}

static void _math_matrix_translate(GLfloat *m, GLfloat x, GLfloat y, GLfloat z ) {
   m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
   m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
   m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
   m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
}

//FIXME:   glTranslatef(0, -h, 0);       /* Shift origin up to upper-left corner. */
void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
  float m[4*4];
  matrix_identity(m);
  _math_matrix_translate(m, x, y, z);

  float t[4 * 4];
  matmul4( t, targetMatrix, m);
  memcpy(targetMatrix, t, sizeof(t));
}

#if 0
//FIXME:   glRotatef(60, 1.0, 0.0, 0.0); (used to rotate cube)
void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
}
#endif

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

  uint32_t *p = pb_begin();
  
  /* Clear depth & stencil buffers */
  if (mask & GL_DEPTH_BUFFER_BIT) {
    p = xgu_set_zstencil_clear_value(p, 0xFFFFFFFF);
    p = xgu_set_clear_rect_horizontal(p, 0, width);
    p = xgu_set_clear_rect_vertical(p, 0, height);
    p = xgu_clear_surface(p, XGU_CLEAR_STENCIL | XGU_CLEAR_Z);
  }

  if (mask & GL_COLOR_BUFFER_BIT) {
    p = xgu_set_color_clear_value(p, 0x00FF00FF);
    p = xgu_set_clear_rect_horizontal(p, 0, width);
    p = xgu_set_clear_rect_vertical(p, 0, height);
    p = xgu_clear_surface(p, XGU_CLEAR_COLOR);
  }

  pb_end(p);
}

static void vertex_attribute_4f(int index, float x, float y, float z, float w) {
  int reg = NV097_SET_VERTEX_DATA4F_M + (index * 4) * 4;

  uint32_t *p = pb_begin();
  pb_push(p++, reg, 4);
  *(p++) = *(uint32_t*)&x;
  *(p++) = *(uint32_t*)&y;
  *(p++) = *(uint32_t*)&z;
  *(p++) = *(uint32_t*)&w;
  pb_end(p);
}

static void vertex_attribute_3f(int index, float x, float y, float z) {
  vertex_attribute_4f(index, x, y, z, 1.0f);
}

static void vertex_attribute_2s(int index, int16_t x, int16_t y) {
  int reg = NV097_SET_VERTEX_DATA2S + index * 4;

  uint32_t *p = pb_begin();
  pb_push(p++, reg, 1);
  *(p++) = x | (y << 16);
  pb_end(p);
}

static void vertex_attribute_4ub(int index, uint8_t x, uint8_t y, uint8_t z, uint8_t w) {
  int reg = NV097_SET_VERTEX_DATA4UB + index * 4;

  uint32_t *p = pb_begin();
  pb_push(p++, reg, 1);
  *(p++) = x | (y << 8) | (z << 16) | (w << 24);
  pb_end(p);
}

void glColor3f(GLfloat red, GLfloat green, GLfloat blue) {
  uint32_t *p = pb_begin();
  p = xgu_set_vertex_data4f(p, XGU_COLOR_ARRAY, red, green, blue, 1.0f);
  pb_end(p);
}

void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
  vertex_attribute_3f(0, x, y, z);
}

void glVertex3fv(const GLfloat * v) {
  glVertex3f(v[0], v[1], v[2]);
}

//FIXME: This is wrong! Xbox 2s seems to normalize values (XQEMU does it at least), so this gets all messed up
void glVertex2s(GLshort x, GLshort y) {
  //vertex_attribute_2s(0, x, y);
  uint32_t* p = pb_begin();
  p = xgu_vertex3f(p, x, y, 0.0f);
  pb_end(p);
}

void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) {
  vertex_attribute_4ub(3, red, green, blue, alpha);
}

#if 0
void glColor3ub(GLubyte red, GLubyte green, GLubyte blue) {

}
#endif

void glNormal3f(GLfloat x, GLfloat y, GLfloat z) {
  vertex_attribute_3f(2, x, y, z);
}

void glNormal3fv(const GLfloat * v) {
  glNormal3f(v[0], v[1], v[2]);
}

#define glVertex2i(x,y) glVertex2s(x,y)


void glFlush(void) {
  //FIXME: Send command stream only, not sure why we wait here, check pbkit docs
  while(pb_busy()) {
      /* Wait for completion... */
  }
}

void glFinish(void) {

#if !defined(NXDK)
  /* Force pushbuffer to run in XQEMU */
  {
    uint32_t* p = pb_begin();
    pb_push(p++, NV097_WAIT_FOR_IDLE, 1);
    *p++ = 0;
    pb_end(p);
  }
#endif

  //FIXME: Why does pb_finished swap buffers? check pbkit docs
  /* Swap buffers (if we can) */
  while (pb_finished()) {
      /* Not ready to swap yet */
  }
}

#endif
