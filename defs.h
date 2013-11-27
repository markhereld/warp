/*
	defs.h
	Drew Olbrich, 1992
*/

#ifndef _DEFS
#define _DEFS

#ifdef WIN32
#pragma warning (disable:4244)  /* disable nasty conversion warnings. */
#endif

#define WIN_TITLE   "warp"
#define ICON_TITLE  "warp"

// #define DEFAULT_IMAGE_FN  "/Users/markhereld/Desktop/distort/data/distort.rgb"
#define DEFAULT_IMAGE_FN  "/Users/markhereld/Development/multiDisplay/texture/data/distort.rgb"

#define DEFAULT_EFFECT  rubber

#define WIN_SIZE_X  512
#define WIN_SIZE_Y  512

#define GRID_SIZE_X  32
#define GRID_SIZE_Y  32

#define CLIP_NEAR  -10.0
#define CLIP_FAR   1000.0


/*
    Debug stuff
*/
#define DBG(S) fprintf(stderr,(S))

#endif /* _DEFS */
