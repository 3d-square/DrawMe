#ifndef _DRAWME_
#define _DRAWME_

#include "raylib.h"
#include <stdint.h>

typedef struct _canvas {
   Rectangle rect;
   int rows;
   int cols;
   int pixel_size;

   uint32_t *image;
} DrawMeCanvas;

#define HEX_SIZE 16

typedef struct _numbox {
   Rectangle rect;
   char number[32];
   int value;
   int deflt;
   int max_size;
   int size;
} NumBox;

typedef struct {
   Rectangle rect;
   Image image;
   Texture2D texture;
   Color top;
   Rectangle rb, gb, bb;
} Gradient;

typedef struct _hexbox {
   Rectangle rect;
   char hex[HEX_SIZE];
   Color value;
   int size;
   NumBox r;
   NumBox g;
   NumBox b;
   NumBox a;
   char selected;
} HexBox;

typedef int (*button_callback)(void *);

typedef struct _clickable {
   Rectangle *rect;
   button_callback callback;
} Clickable;

#define MAX_UI_ELEMENTS 500

typedef struct _ui {
   Clickable lmb_clickable[MAX_UI_ELEMENTS];
   int num_lmb_buttons;
   Clickable rmb_clickable[MAX_UI_ELEMENTS];
   int num_rmb_buttons;
} DrawMeUI;

#define HEXBOX_PAD 5

typedef enum {
   MeCircle,
   MeSquare
} DrawMode;

#define ABS(a) (a) > 0 ? (a) : -(a)


/* Extern Globals */
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern DrawMeCanvas  g_CANVAS;
extern DrawMeUI      g_UI;
extern Rectangle     g_modeCircle;
extern Rectangle     g_modeSquare;
extern DrawMode      g_brush;
extern HexBox        g_box_brush_color;
extern NumBox        g_box_brush_size;
extern Gradient      g_gradient_selector;
extern Color         g_erase_color;
extern int           g_brush_size;
extern enum select_type g_selected;
extern Vector2       g_mouse_pos;
extern Vector2       g_prev_mouse_pos;

enum select_type {
   NONE_SELECT,
   BRUSH_COLOR_SELECT,
   SIZE_SELECT,
   GRADIENT_SELECT
};

typedef struct Vec2{
   int x;
   int y;
} Vec2;

/* Global Macros */
#define CANVAS_PIXEL(i, j) (g_CANVAS.image + ((i) * g_CANVAS.cols) + (j))

/* Global Functions */
void drawme_init();
void drawme_mainloop();
void drawme_close();

/* UI Functions */
void ui_add_element(MouseButton button, Rectangle *bounds, button_callback callback);

/* State Transitions */
void set_select(enum select_type);
const char *select_str();

/* Callbacks */
void ui_setup_callbacks();
int brush_color_callback(void *);
int  brush_size_callback(void *);
int      canvas_callback(void *);
int    canvas_r_callback(void *);
int mode_square_callback(void *);
int mode_circle_callback(void *);
int gradient_selector_callback(void *);
int gradient_callback(Gradient *grad, Color *out);
/* Callbacks */

/* Private Functions windows.c */
void canvas_update();
void canvas_draw();
int pos_to_canvas_index(Vector2 pos, int *x, int *y);
Color *get_canvas_pixel(Vector2 pos);
void canvas_set_cluster(int x, int y, int radius, DrawMode mode, Color color);
int interpolate(Vec2 start, Vec2 end, int step, Vec2 points[]);
void canvas_set_interpolated_line(Vec2 start, Vec2 end, DrawMode mode, int step, Color color);

// HexBox
int update_hexbox(HexBox *hex);
void set_hexbox_value(HexBox *num);
void set_hexbox_str(HexBox *num);
void set_hexbox_by_color(HexBox *hex);
void draw_hexbox(HexBox *hex);
int hexbox_callback(HexBox *hex);
HexBox make_hexbox(Color color, int x, int y);

// NumBox
int update_numbox(NumBox *num);
void set_numbox_value(NumBox *num);
void set_numbox_str(NumBox *num);
void draw_numbox(NumBox *num);
void limit_numbox(NumBox *num, int upper, int lower);
NumBox make_numbox(int deflt, int max_chars, int x, int y);

// Gradient
Gradient make_gradient(int x, int y);
void free_gradient(Gradient *selector);
void update_gradient(Gradient *gradient, Color top);
void draw_gradient_selector(Gradient *gradient);

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
