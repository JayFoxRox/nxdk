/*
 * Based GLUT sample with the following license:
 *
 *     Copyright (c) Mark J. Kilgard, 1996.
 *
 *     This program is freely distributable without licensing fees 
 *     and is provided without guarantee or warrantee expressed or 
 *     implied. This program is -not- in the public domain.
 *
 *
 * This sample implements a tiny subset of the OpenGL API
 */

#include <GL/glut.h>

//#define INT

#ifndef INT
float lo = 200.0f;
float hi = 400.0f;
#define COL(x,y) glColor3f((x == lo) ? 0.0f : 1.0f, (y == lo) ? 0.0f: 1.0f, 0.0f)
#define VERT(x,y) COL(x,y); glVertex3f(x, y, 0.0f)
#else
int lo = 0x2FFF;
int hi = 0x7FFF;
#define COL(x,y) glColor4ub((x == lo) ? 0x00 : 0xFF, (y == lo) ? 0x00 : 0xFF, 0x00, 0xFF)
#define VERT(x,y) COL(x,y); glVertex2i(x, y)
#endif

void
reshape(int w, int h)
{
  /* Because Gil specified "screen coordinates" (presumably with an
     upper-left origin), this short bit of code sets up the coordinate
     system to correspond to actual window coodrinates.  This code
     wouldn't be required if you chose a (more typical in 3D) abstract
     coordinate system. */

  glViewport(0, 0, w, h);       /* Establish viewing area to cover entire window. */
  glMatrixMode(GL_PROJECTION);  /* Start modifying the projection matrix. */
  glLoadIdentity();             /* Reset project matrix. */
//  glOrtho(0, w, 0, h, -1, 1);   /* Map abstract coords directly to window coords. */
  glOrtho(0, 640, 0, 480, -1, 1);   /* Map abstract coords directly to window coords. */
#if 0
  glScalef(1, -1, 1);           /* Invert Y axis so increasing Y goes down. */
  glTranslatef(0, -h, 0);       /* Shift origin up to upper-left corner. */
#endif
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_QUADS);

    glColor3f(0.0, 0.0, 0.0);  /* blue */
    VERT(lo, lo);

    glColor3f(1.0, 0.0, 0.0);  /* blue */
    VERT(hi, lo);

    glColor3f(1.0, 1.0, 0.0);  /* green */
    VERT(hi, hi);

    glColor3f(0.0, 1.0, 0.0);  /* red */
    VERT(lo, hi);

  glEnd();

#if 0
  glBegin(GL_TRIANGLES);

    glColor3f(0.0, 1.0, 0.0);  /* red */
    //glVertex2i(20, 200);
    glVertex3f(lo, hi, 0.0f);

    glColor3f(1.0, 1.0, 0.0);  /* green */
    //glVertex2i(200, 200);
    glVertex3f(hi, hi, 0.0f);

    glColor3f(0.0, 0.0, 0.0);  /* blue */
    //glVertex2i(0, 0);
    glVertex3f(lo, lo, 0.0f);

  glEnd();
#endif

  glFlush();  /* Single buffered, so needs a flush. */
}

int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutCreateWindow("single triangle");
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
