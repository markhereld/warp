/*
    Modified
    Mark Hereld, 2013
*/

#include <Carbon/Carbon.h>

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

#include "texture.h"
#include "defs.h"


int win_size_x = WIN_SIZE_X, win_size_y = WIN_SIZE_Y;


/*
	Load the image to distort, make a texture out of it, and enable 
	texturing.
 */

void image_init(char *fn)
{
  unsigned int *buf;
  int width, height, depth;

  buf = read_texture(fn, &width, &height, &depth);
  if (buf == NULL)
  {
    fprintf(stderr, "distort: Can't load image file \"%s\".\n", fn);
    exit(-1);
  }

  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, depth, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
  glEnable(GL_TEXTURE_2D);
  
  free(buf);
}


/*
	Keyboard handler to allow exit on <ESC>
 */

extern int showNew;

void keyboard(unsigned char key, int x, int y)
{
  if (key == 27)                // ESC to QUIT
    exit(0);
  if (key == 'n')
    showNew = (showNew == 0);
}

/*
	Show the world our stuff.
 */

void display(void)
{
  warp_redraw();
}

/*
	Spend idle time drawing dynamics.
 */

void idle(void)
{
  warp_dynamics();
  glutPostRedisplay();
}

/*
	Open and initialize an OpenGL window.
 */

void display_init()
{
  glutInitWindowSize(WIN_SIZE_X, WIN_SIZE_Y);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

  glutCreateWindow(WIN_TITLE);
  glutSetIconTitle(ICON_TITLE);
  
  glViewport(0, 0, WIN_SIZE_X, WIN_SIZE_Y);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-0.5, WIN_SIZE_X - 0.5, -0.5, WIN_SIZE_Y - 0.5, CLIP_NEAR, CLIP_FAR);

  warp_init();

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
}

/*
	If a command line argument is provided, it is interpreted
	as the name of an alternate source image.
 */

int main(int argc, char **argv)
{
  DBG("\n");
  DBG("entering main\n");
  glutInit(&argc, argv);

  display_init();

  if (argc == 1)
    image_init(DEFAULT_IMAGE_FN);
  else
    image_init(argv[1]);

  glutMainLoop();

  return 0;
}
