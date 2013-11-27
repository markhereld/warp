/*
    Modify
    Mark Hereld, 2013
*/

#include <stdio.h>
#include <math.h>
#include <malloc/malloc.h>
#include <GLUT/glut.h>

#include "defs.h"
#include "warp.h"

extern int win_size_x, win_size_y;



#define TRACE(S) printf("%s\n",S)


/**************************************************************************************
 *   Meshes for texture mapping
 **************************************************************************************/
//#define MESH_WIDTH  20    // grid points = (MESH_WIDTH + 1) * (MESH_HEIGHT + 1)
//#define MESH_HEIGHT 20


//++++++++++++++++++++++++++++++++++++++++++++ 
//++++++++++++++++++++++++++++++++++++++++++++ set up XY coord mesh to span rectanglar bounds
//++++++++++++++++++++++++++++++++++++++++++++ 
void Mesh_Init(Mesh *m, IntPoint dim, Bounds b) {
  int i,j;
  float dx, dy, slowOffset;
  IntPoint coord;
  Point pos, verify;
//  TRACE("initMesh");

  m->cellsWide = dim.x;     // vertices in row is cellsWide + 1
  m->cellsHigh = dim.y;     // vertices in col is cellsHigh + 1
  m->mesh = (float *)malloc((dim.x+1)*(dim.y+1)*2*sizeof(float));

  dx = (b.right - b.left)/dim.x;   // delta x between grid points
  dy = (b.top - b.bottom)/dim.y;   // delta y between grid points

  for (i = 0; i<= dim.x; i++ ) {
    for (j = 0; j<= dim.y; j++ ) {
      coord.x = i; coord.y = j;
      pos.x = i*dx + b.left;            // x coordinate at i,j
      pos.y = j*dy + b.bottom;          // y coordinate at i,j
      Mesh_Set(m,coord,pos);
    }
  }
}

//-------------------------------------------- convert 2D index to 1D index into mesh
int Mesh_Index(Mesh *m, IntPoint i) {
  return 2*((m->cellsHigh+1)*i.x + i.y);
}

//-------------------------------------------- set the XY value at vertex at IJ
void Mesh_Set(Mesh *m, IntPoint i, Point p) {
  int offsetXY = Mesh_Index(m,i);
  m->mesh[offsetXY] = p.x;
  m->mesh[offsetXY+1] = p.y;
}

//-------------------------------------------- get the XY value at vertex at IJ
void Mesh_Get(Mesh *m, IntPoint i, Point *p) {
  int offsetXY = Mesh_Index(m,i);
  p->x = m->mesh[offsetXY];
  p->y = m->mesh[offsetXY+1];
}

//-------------------------------------------- nudge the XY value at vertex at IJ
void Mesh_Move(Mesh *m, IntPoint i, Point p) {
  int offsetXY = Mesh_Index(m,i);
  m->mesh[offsetXY]   += p.x;
  m->mesh[offsetXY+1] += p.y;
}

//-------------------------------------------- summarize basics of mesh struct
void _Mesh_Info(Mesh *m) {
  printf("%0x %d %d",m->mesh,m->cellsWide,m->cellsHigh);
}

void Mesh_Info(Mesh *m) {
  _Mesh_Info(m);
  printf("\n");
}


//++++++++++++++++++++++++++++++++++++++++++++ 
//++++++++++++++++++++++++++++++++++++++++++++ setup the warp object
//++++++++++++++++++++++++++++++++++++++++++++ 
void Warp_Init(Warp* w, IntPoint dim, Bounds tb, Bounds vb) {
  w->cellsWide = dim.x;
  w->cellsHigh = dim.y;

  w->textureMesh = (float *)malloc((dim.x+1)*(dim.y+1)*2*sizeof(float));
  Mesh_Init(w->textureMesh, dim, tb);
//  printf("tBounds:    %f %f   %f %f\n", tb.left,tb.right,tb.bottom,tb.top);

  w->vertexMesh = (float *)malloc((dim.x+1)*(dim.y+1)*2*sizeof(float));
  Mesh_Init(w->vertexMesh, dim, vb);
//  printf("vBounds:    %f %f   %f %f\n", vb.left,vb.right,vb.bottom,vb.top);
}

#define WARP_Z 1.0
void _Warp_Vertex(Warp *w, int i, int j) {      // emit mapping for single vertex in mesh to display list
  IntPoint vrtx;
  Point val;

  vrtx.x = i;
  vrtx.y = j;

  Mesh_Get(w->textureMesh,vrtx,&val);
  glTexCoord2f(val.x,val.y);
  Mesh_Get(w->vertexMesh,vrtx,&val);
  glVertex3f(val.x,val.y,WARP_Z);
}

void Warp_Map(Warp *w) {
  int i,j;
  glBegin( GL_TRIANGLES );

  for (i=0; i < w->cellsWide; i++) {
    for (j=0; j < w->cellsHigh; j++) {
      _Warp_Vertex(w, i  ,j  );       // map lower left triangle of quad
      _Warp_Vertex(w, i  ,j+1);
      _Warp_Vertex(w, i+1,j+1);
      _Warp_Vertex(w, i+1,j+1);       // map upper right triangle of quad
      _Warp_Vertex(w, i+1,j  );
      _Warp_Vertex(w, i  ,j  );
    }
  }

  glEnd();
}

void Warp_Info(Warp* w) {
  TRACE("Warp_Info");
  printf("Texture: ");
  _Mesh_Info(w->textureMesh);
  printf("     Vertex: ");
  _Mesh_Info(w->vertexMesh);
  printf("\n");
}

void Bounds_Info(Bounds* b) {
  printf("%f %f %f %f\n", b->left, b->right, b->bottom, b->top);
}

//++++++++++++++++++++++++++++++++++++++++++++ 
//++++++++++++++++++++++++++++++++++++++++++++ setup the warp object
//++++++++++++++++++++++++++++++++++++++++++++ 
void WarpContext_Init(WarpContext* wc, int numWarps, IntPoint pixDim, IntPoint meshDim, IntPoint warpPix) {
  IntPoint wNum;

  wc->maxWarps = numWarps;
  wc->numWarps = numWarps;
  wc->warps = (Warp *)malloc(numWarps*sizeof(Warp));
  wc->pixWide = pixDim.x;
  wc->pixHigh = pixDim.y;

  wNum.x = (pixDim.x+1)/warpPix.x;
  wNum.y = (pixDim.y+1)/warpPix.y;

  int i,iw,jw;
  iw = 0; jw = 0;
  for (i=0;i<numWarps;i++) {
    Bounds tBounds, vBounds;
    tBounds.left   = 0.0 + 1.0*iw/wNum.x; 
    tBounds.right  = 0.0 + 1.0*(iw+1)/wNum.x;;
    tBounds.bottom = 0.0 + 1.0*jw/wNum.y;
    tBounds.top    = 0.0 + 1.0*(jw+1)/wNum.y;
    vBounds.left   = 0.0 + pixDim.x*iw/wNum.x;
    vBounds.right  = 0.0 + pixDim.x*(iw+1)/wNum.x;
    vBounds.bottom = 0.0 + pixDim.y*jw/wNum.y;
    vBounds.top    = 0.0 + pixDim.y*(jw+1)/wNum.y;

    Warp_Init(&(wc->warps[i]), meshDim, tBounds, vBounds);
    //Warp_Info(&(wc->warps[i]));

    iw ++;
    if (iw >= wNum.x) {
      jw++;
      iw = 0;
    }
  }
}

void WarpContext_Map(WarpContext* wc) {
  int i;
  if (wc->numWarps < 1) return;
  for (i=0; i < wc->numWarps; i++)
    Warp_Map(&(wc->warps[i]));
}


//-------------------------------------------- initialize warp transformation
WarpContext myWarpContext;

#define PI 3.14159
void moreInit() {
  int nW;
  IntPoint pixS, meshD, warpS;
  nW = 64;
  pixS.x = 512;         pixS.y = 512;
  meshD.x = 5;          meshD.y = 4;
  warpS.x = 64;         warpS.y = 64;
  WarpContext_Init(&myWarpContext, nW, pixS, meshD, warpS);
}

/*-----------------------------------------------------------------------------------*/

#define TEXTURE_WIDTH 512
#define TEXTURE_HEIGHT 512
GLuint textureId;

int showNew = 1;


void warpFramebuffer() {
  int i,j;
  //TRACE("warpFramebuffer");
  /* 
   *  Copy flat rendered framebuffer into texture for warping
   */
  glBindTexture(GL_TEXTURE_2D, textureId);
  glReadBuffer(GL_BACK);
  glDrawBuffer(GL_BACK);
  glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,0,0,win_size_x,win_size_y,0);
  glEnable(GL_TEXTURE_2D);
  
 // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* 
   *  lay in the warped version from the texture read back from fb
   *  make sure right texture is bound here
   */
  glClear( GL_COLOR_BUFFER_BIT );

  WarpContext_Map(&myWarpContext);

  glBindTexture(GL_TEXTURE_2D, 0);
}



/**************************************************************************************
 *   miscellaneous test bits and initialization 
 **************************************************************************************/
typedef struct {
  float v1[2];
  float v2[2];
  float v3[2];
  float vel[2];
  float color[3];
} TRINGLE;

static TRINGLE doink;

void anim_init() {
  TRACE("anim_init");
  printf("(%d,%d)\n",win_size_x,win_size_y);
  doink.v1[0] = 300; doink.v1[1] = 100;
  doink.v2[0] = 390; doink.v2[1] = 200;
  doink.v3[0] = 200; doink.v3[1] = 300;
  doink.vel[0] = 0.305562342; doink.vel[1] = 0.211312;
}

void anim_redraw() {
  //TRACE("anim_redraw");
  glDisable(GL_TEXTURE_2D);


  /*                                             MOVING TRIANGLE */
  if (showNew)
    glColor4f(0.0f,1.0f,0.0f,1.0f);
  else
    glColor4f(1.0f,0.0f,0.0f,1.0f);
#define TRI_Z -0.1f
  glBegin(GL_TRIANGLES);
    glVertex3f(doink.v1[0],doink.v1[1],TRI_Z);
    glVertex3f(doink.v2[0],doink.v2[1],TRI_Z);
    glVertex3f(doink.v3[0],doink.v3[1],TRI_Z);
  glEnd();

  /*                                             STATIONARY SQUARE */
  glColor4f(0.5f,1.0f,1.0f,1.0f);               // Set The Color To Blue One Time Only
#define QUAD_Z -0.3f
  glBegin(GL_QUADS);                            // Start Drawing Quads
    glVertex3f( 200.0f,470.0f,QUAD_Z);          // Left And Up 1 Unit (Top Left)
    glVertex3f(470.0f,470.0f,QUAD_Z);           // Right And Up 1 Unit (Top Right)
    glVertex3f(470.0f, 200.0f,QUAD_Z);          // Right And Down One Unit (Bottom Right)
    glVertex3f( 200.0f, 200.0f,QUAD_Z);         // Left And Down One Unit (Bottom Left)
  glEnd();  

  /*                                             CHECKERBOARD */
#define CHECK_Z -0.5f
  int i,j;
  float dx,dy;
#define CHECK_NUM 30
  dx = 512./CHECK_NUM;
  dy = 512./CHECK_NUM;
  glBegin(GL_QUADS);
    for (i=0; i<CHECK_NUM; i++) {
      for (j=0; j<CHECK_NUM; j++) {
        if ((i+j)%2==0) 
          glColor4f(0.0,0.0,0.2,1.0);
        else
          glColor4f(0.6,0.6,0.9,1.0);
        glVertex3f(i*dx,j*dx,CHECK_Z);
        glVertex3f((i+1)*dx,j*dx,CHECK_Z);
        glVertex3f((i+1)*dx,(j+1)*dx,CHECK_Z);
        glVertex3f(i*dx,(j+1)*dx,CHECK_Z);
      }
    }
  glEnd();
}


void texture_init() {
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
  GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void warp_init()
{
  int i, j;
  int k;
  int m;

  TRACE("warp_init");

  texture_init();
  moreInit();
  anim_init();

  glEnable(GL_DEPTH_TEST);
}

/*
	Do the dynamics simulation for the next frame.
*/

void warp_dynamics()
{
  //TRACE("warp_dynamics");
  int i;
  float cen[2];
  for(i=0; i<2; i++) {
    doink.v1[i] += doink.vel[i];
    doink.v2[i] += doink.vel[i];
    doink.v3[i] += doink.vel[i];
    cen[i] = (doink.v1[i] + doink.v2[i] + doink.v3[i])/3;
  }
  if (cen[0] < 0) doink.vel[0] *= -1;
  if (cen[0] > win_size_x) doink.vel[0] *= -1;
  if (cen[1] < 0) doink.vel[1] *= -1;
  if (cen[1] > win_size_x) doink.vel[1] *= -1;
}

/*
	Draw the next frame of animation.
*/

void warp_redraw()
{
  int k;
  int i, j;

  //TRACE("warp_redraw");
  glClearColor(0.2f,0.2f,0.3f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  anim_redraw();

  warpFramebuffer();

  glutSwapBuffers();
}

