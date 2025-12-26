#ifndef _DRAWME_
#define _DRAWME_

#include "raylib.h"

typedef struct _canvas {
   int x, y, width, height;
   int rows;
   int cols;
   int pixel_size;

   Color *image;
} DrawMeCanvas;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

extern DrawMeCanvas CANVAS;

void drawme_init();
void drawme_mainloop();
void drawme_close();


#endif
