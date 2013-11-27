/*
	warp.h
	Mark Hereld
*/

#ifndef _WARP
#define _WARP


/***************************************************
*                                Simple Primatives
***************************************************/
typedef struct _IntPoint {
  int x, y;
} IntPoint;

typedef struct _IntBounds {
  int ll, lr, ul, ur;
} IntBounds;

typedef struct _Point {
  float x, y;
} Point;

typedef struct _Bounds {
  float left, right, top, bottom;
} Bounds;
void Bounds_Info(Bounds *b);

/***************************************************
*                                    Mesh
***************************************************/
typedef struct _Mesh {
  float *mesh;
  int cellsWide;
  int cellsHigh;
} Mesh;

void Mesh_Init(Mesh *m, IntPoint dim, Bounds b);
void Mesh_Set(Mesh *m, IntPoint i, Point x);
void Mesh_Get(Mesh *m, IntPoint i, Point *x);
void Mesh_Move(Mesh *m, IntPoint i, Point x);
void Mesh_Info(Mesh *m);

/***************************************************
*                                    Warp
***************************************************/
typedef struct _Warp {
  Mesh *vertexMesh;
  Mesh *textureMesh;
  int cellsWide;
  int cellsHigh;
  float left, right, bottom, top;
} Warp;

void Warp_Init(Warp* wm, IntPoint d, Bounds t, Bounds v);
void Warp_Info(Warp* wm);


/***************************************************
*                                    WarpContext
***************************************************/
typedef struct _WarpContext {
  Warp* warps;
  int maxWarps;
  int numWarps;
  int pixWide;
  int pixHigh;
} WarpContext;

void WarpContext_Init(WarpContext* wc, int numWarps, IntPoint pixDim, IntPoint meshDim, IntPoint warpPix);
void WarpContext_Load(WarpContext* wc, char* fn);
void WarpContext_Save(WarpContext* wc, char* fn);
//void WarpContext_SetVertex(WarpContext* wc, IntPoint warpIndex, IntPoint vertexIndex, Point value);
//void WarpContext_MoveVertex(WarpContext* wc, IntPoint warpIndex, IntPoint vertexIndex, Point delta);
void WarpContext_GetWarp(WarpContext* wc, int warpIndex, Warp* w);
//Warp* WarpContext_GetWarp(WarpContext* wc, int warpIndex);







//extern WarpContext *MyWarps;




void warp_init();
void warp_dynamics();
void warp_redraw();

#endif





