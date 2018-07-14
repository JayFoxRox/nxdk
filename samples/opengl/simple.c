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
  glOrtho(0, w, 0, h, -1, 1);   /* Map abstract coords directly to window coords. */
#if 0
  glScalef(1, -1, 1);           /* Invert Y axis so increasing Y goes down. */
  glTranslatef(0, -h, 0);       /* Shift origin up to upper-left corner. */
#endif
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glBegin(GL_TRIANGLES);

    glColor3f(0.0, 0.0, 1.0);  /* blue */
    //glVertex2i(0, 0);
    glVertex3f(0.0f, 0.0f, 0.0f);

    glColor3f(0.0, 1.0, 0.0);  /* green */
    //glVertex2i(200, 200);
    glVertex3f(1.0f, 1.0f, 0.0f);

    glColor3f(1.0, 0.0, 0.0);  /* red */
    //glVertex2i(20, 200);
    glVertex3f(0.1f, 1.0f, 0.0f);
  glEnd();
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
