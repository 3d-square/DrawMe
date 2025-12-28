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

#define HEX_SIZE 16

typedef struct _hexbox {
   int x, y;
   char hex[HEX_SIZE];
   Color value;
   int size;
} HexBox;

typedef enum {
   MeCircle,
   MeSquare
} DrawMode;


/* Extern Globals */
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern DrawMeCanvas CANVAS;
extern Rectangle modeCircle;
extern Rectangle modeSquare;
extern DrawMode brush;
extern HexBox box_brush_color;
extern Color erase_color;
extern int brush_size;

/* Global Macros */
#define CANVAS_PIXEL(i, j) (CANVAS.image + ((i) * CANVAS.cols) + (j))
#define CANVAS_RECT() (CLITERAL(Rectangle){CANVAS.x, CANVAS.y, CANVAS.width, CANVAS.height})

/* Global Functions */
void drawme_init();
void drawme_mainloop();
void drawme_close();

/* Private Functions windows.c */
void canvas_update();
void canvas_draw();
int pos_to_canvas_index(Vector2 pos, int *x, int *y);
Color *get_canvas_pixel(Vector2 pos);
void canvas_set_cluster(int x, int y, int radius, DrawMode mode, Color color);
void update_hexbox(HexBox *hex);
void draw_hexbox(HexBox *hex);

/* Utility Functions */
unsigned color_to_int(Color c);
Color int_to_color(unsigned i);

/* Private Functions events.c */
int is_mouse_held();
int button_held_event();
int right_button_pressed_event();
int left_button_pressed_event();
int right_button_released_event();
int left_button_released_event();

#endif
