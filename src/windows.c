#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"

/*********************************** GLOBALS ***********************************/
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

int            g_scaling = 1;
DrawMeCanvas   g_CANVAS = {0};
DrawMeUI       g_UI = {0};
Texture2D      g_CANVAS_TEXTURE = {0};
DrawMode       g_brush = MeCircle;
Gradient       g_gradient_selector = {0};
int            g_brush_size = 3;
const char     *g_modeString = "";
Color          g_erase_color = WHITE;
enum select_type g_selected;
Vector2        g_mouse_pos;
Vector2        g_prev_mouse_pos;
/*********************************** GLOBALS ***********************************/

// Radio Buttons
Rectangle g_modeCircle = {.x = 25, .y = 25, .width = 25, .height = 25 };
Rectangle g_modeSquare = {.x = 55, .y = 25, .width = 25, .height = 25 };

// Hex/Num boxes
HexBox g_box_brush_color;
NumBox g_box_brush_size;

#include <stdarg.h>
#define FATAL(...) do { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(-1); } while(0)

void canvas_init(int x, int y, int width, int height, int pixel_size){
   g_CANVAS.rect.x = x;
   g_CANVAS.rect.y = y;
   g_CANVAS.rect.height = height;
   g_CANVAS.rect.width = width;
   
   pixel_size *= g_scaling;

   g_CANVAS.rows = height / pixel_size;
   g_CANVAS.cols = width / pixel_size;

   printf("Canvas Size: %d\n", g_CANVAS.rows * g_CANVAS.cols);
   printf("Canvas Bytes: %zu\n", g_CANVAS.rows * g_CANVAS.cols * sizeof(Color));
   g_CANVAS.image = calloc(1, g_CANVAS.rows * g_CANVAS.cols * sizeof(Color));
   if(g_CANVAS.image == NULL){
      FATAL("Unable to allocate memory for the canvas\n");
   }

   g_CANVAS.pixel_size = pixel_size;

   Image img = {
      .data    = g_CANVAS.image,
      .width   = g_CANVAS.cols,
      .height  = g_CANVAS.rows,
      .mipmaps = 1,
      .format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
   };
   
   g_CANVAS_TEXTURE = LoadTextureFromImage(img);
}

void canvas_close(){
   UnloadTexture(g_CANVAS_TEXTURE);
   free(g_CANVAS.image);
}

void canvas_update(){
   UpdateTexture(g_CANVAS_TEXTURE, (const void *)g_CANVAS.image);
}

void canvas_draw(){
   DrawTextureEx(g_CANVAS_TEXTURE, CLITERAL(Vector2){g_CANVAS.rect.x, g_CANVAS.rect.y}, 0, g_scaling, WHITE);
   DrawRectangleLinesEx(g_CANVAS.rect, 2, BLACK);
}

int pos_to_canvas_index(Vector2 pos, int *x, int *y){
   if(CheckCollisionPointRec(pos, g_CANVAS.rect)){
      *x = (pos.x - g_CANVAS.rect.x) / g_CANVAS.pixel_size;
      *y = (pos.y - g_CANVAS.rect.y) / g_CANVAS.pixel_size;

      return 1;
   }

   return 0;
}

Color *get_canvas_pixel(Vector2 pos){
   int x;
   int y;
   if(pos_to_canvas_index(pos, &x, &y)){
      return (Color *)CANVAS_PIXEL(y, x);
   }

   return NULL;
}

int interpolate(Vec2 start, Vec2 end, int step, Vec2 points[]){
   int dx = ABS(start.x - end.x),
       sx = start.x < end.x ? 1 : -1;

   int dy = ABS(start.y - end.y),
       sy = start.y < end.y ? 1 : -1;

   int err = (dx > dy ? dx : -dy) / 2,
       e2;

   int stepy = step;

   int count = 0;

   while(1){
      if (start.x == end.x && start.y == end.y){
         points[count++] = start;
         break;
      }

      if(stepy >= step){
         stepy = 0;
         points[count++] = start;
      }

      e2 = err;

      if(e2 > -dx){
         err -= dy;
         start.x += sx;
         stepy += 1;
      }

      if(e2 < dy){
         err += dx;
         start.y += sy;
         stepy += 1;
      }
   }
   return count;
}

void canvas_set_interpolated_line(Vec2 start, Vec2 end, DrawMode mode, int step, Color color){
   Vec2 points[100];
   int count = interpolate(start, end, step, points);
   
   draw_logf("Interpolating from (%d, %d) to (%d, %d) with step %d", start.x, start.y, end.x, end.y, step);
   draw_logf("  Number Points: %d", count);
   for(int i = 0; i < count; ++i){
      draw_logf("  (%d, %d)", points[i].x, points[i].y);
      canvas_set_cluster(points[i].x, points[i].y, step, mode, color);
   }
}

void canvas_set_cluster(int x, int y, int radius, DrawMode mode, Color color){
   int x_low = (int)fmax(0, x - radius),
       x_hi  = (int)fmin(g_CANVAS.cols - 1, x + radius);
   int y_low = (int)fmax(0, y - radius),
       y_hi  = (int)fmin(g_CANVAS.rows - 1, y + radius);

   if(mode == MeCircle){
      int r2 = radius * radius;

      for(int i = y_low; i <= y_hi; ++i){
         int dy = i - y;
         for(int j = x_low; j <= x_hi; ++j){
            int dx = j - x;
            if(dx * dx + dy * dy <= r2){
               *(Color *)CANVAS_PIXEL(i, j) = color;
            }
         }
      }
   }else if(mode == MeSquare){
      for(int i = y_low; i <= y_hi; ++i){
         for(int j = x_low; j <= x_hi; ++j){
            *(Color *)CANVAS_PIXEL(i, j) = color;
         }
      }
   }
   
}

void drawme_init(){
   InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DrawMe - The slutty tool");
   // SetTargetFPS(10);
   Font default_font = LoadFontEx("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf", 15, NULL, 0);
   SetFontDefault(default_font);
   
   g_box_brush_color = make_hexbox(GREEN, 10, 100);
   g_box_brush_size = make_numbox(3, 2, 10, 250);
   g_gradient_selector = make_gradient(10, 100);

   canvas_init(125, 50, SCREEN_WIDTH - 125, SCREEN_HEIGHT - 100, 1);

   ui_setup_callbacks();
}

void drawme_mainloop(){
   while (!WindowShouldClose())
   {
      g_mouse_pos = GetMousePosition();
      if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
         left_button_pressed_event();
      }else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
         right_button_pressed_event();
      }else if(is_mouse_held()){
         button_held_event();
      }

      // User changeable text boxes
      if(g_selected == BRUSH_COLOR_SELECT){
         update_hexbox(&g_box_brush_color);
      }else if(g_selected == SIZE_SELECT){
         update_numbox(&g_box_brush_size);
      }

      if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
         left_button_released_event();
      }else if(IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){
         right_button_released_event();
      }

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();

         ClearBackground(WHITE);

         // Buttons
         if(g_brush == MeCircle){
            DrawRectangleRec(g_modeCircle, BLUE);
            g_modeString = "Circle";
         }else if(g_brush == MeSquare){
            DrawRectangleRec(g_modeSquare, BLUE);
            g_modeString = "Square";
         }
         
         // UI Elements
         DrawRectangleLinesEx(g_modeCircle, 1, BLACK);
         DrawRectangleLinesEx(g_modeSquare, 1, BLACK);

         DrawText(g_modeString, 25, 55, 15, BLACK);

         draw_hexbox(&g_box_brush_color);
         draw_numbox(&g_box_brush_size);

         // Canvas
         canvas_draw();

         // Gradient Selector
         if(g_selected == GRADIENT_SELECT){
            draw_gradient_selector(&g_gradient_selector);
         }

         // Mouse
         if(CheckCollisionPointRec(g_mouse_pos, g_CANVAS.rect)){
            if(g_brush == MeCircle){
               DrawCircleLinesV(g_mouse_pos, (g_box_brush_size.value - 1) * g_scaling, BLACK);
            }else if(g_brush == MeSquare){
               int side = ((g_box_brush_size.value - 1) * g_scaling);
               DrawRectangleLines(g_mouse_pos.x - side, g_mouse_pos.y - side, side + side, side + side, BLACK);
            }
         }
         

      EndDrawing();
      //----------------------------------------------------------------------------------
      g_prev_mouse_pos = g_mouse_pos;
   }
}

void drawme_close(){
   canvas_close();
   free_gradient(&g_gradient_selector);
   CloseWindow();        // Close window and OpenGL context
}

uint32_t color_to_int(Color c){
   uint32_t hc = (c.r << 24)
               + (c.g << 16) 
               + (c.b << 8) 
               + c.a;

   printf("hc = %x\n", hc);

   return hc;
}

Color int_to_color(uint32_t i){
   Color c = {
      .r = (i >> 24) & 0xFF,
      .g = (i >> 16) & 0xFF,
      .b = (i >> 8) & 0xFF,
      .a =  i & 0xFF
   };

   return c;
}

void set_select(enum select_type select){
   const char *prev_state = select_str();
   g_selected = select;

   draw_logf("State: %s -> %s\n", prev_state, select_str());
}

const char *select_str(){
   switch(g_selected){
      case NONE_SELECT: return "NONE_SELECT";
      case BRUSH_COLOR_SELECT: return "BRUSH_COLOR_SELECT";
      case SIZE_SELECT: return "SIZE_SELECT";
      case GRADIENT_SELECT: return "GRADIENT_SELECT"; 
   };
   return "NULL";
}

/**************************** LOG *******************************/

#include <stdarg.h>
void _draw_logf(const char *str, const char *func, int line, ...){
#  ifndef NO_DEBUG
      va_list args;
      va_start(args, line);
      char buff[1024];
      
      sprintf(buff, "LOG[%s][%d]: %s\n", func, line, str);
      vprintf(buff, args);
      va_end(args);
#  else
   (void) str;
   (void) func;
   (void) line;
#  endif
}

/**************************** LOG *******************************/
