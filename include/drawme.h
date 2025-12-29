#ifndef _DRAWME_
#define _DRAWME_

#include "raylib.h"
#include <stdint.h>

typedef struct _canvas {
   int x, y, width, height;
   int rows;
   int cols;
   int pixel_size;

   uint32_t *image;
} DrawMeCanvas;

#define HEX_SIZE 16

typedef struct _hexbox {
   Rectangle rect;
   char hex[HEX_SIZE];
   Color value;
   int size;
} HexBox;

typedef struct _numbox {
   Rectangle rect;
   char number[32];
   int value;
   int max_size;
   int size;
} NumBox;

#define HEXBOX_PAD 5

typedef enum {
   MeCircle,
   MeSquare
} DrawMode;


/* Extern Globals */
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern DrawMeCanvas  g_CANVAS;
extern Rectangle     g_modeCircle;
extern Rectangle     g_modeSquare;
extern DrawMode      g_brush;
extern HexBox        g_box_brush_color;
extern NumBox        g_box_brush_size;
extern Color         g_erase_color;
extern int           g_brush_size;
extern int           g_selected;

#define NONE_SELECT 0
#define BRUSH_COLOR_SELECT 1
#define SCALING_SELECT 2

/* Global Macros */
#define CANVAS_PIXEL(i, j) (g_CANVAS.image + ((i) * g_CANVAS.cols) + (j))
#define CANVAS_RECT() (CLITERAL(Rectangle){g_CANVAS.x, g_CANVAS.y, g_CANVAS.width, g_CANVAS.height})

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
HexBox make_hexbox(Color color, int x, int y);

void update_numbox(NumBox *hex);
void draw_numbox(NumBox *hex);
NumBox make_numbox(int deflt, int max_chars, int x, int y);

/* Utility Functions */
uint32_t color_to_int(Color c);
Color int_to_color(uint32_t i);

/* Private Functions events.c */
int is_mouse_held();
int button_held_event();
int right_button_pressed_event();
int left_button_pressed_event();
int right_button_released_event();
int left_button_released_event();

#endif
