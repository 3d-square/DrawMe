#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#include "raylib.h"

/*********************************** GLOBALS ***********************************/
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

int            g_scaling = 3;
DrawMeCanvas   g_CANVAS = {0};
Texture2D      g_CANVAS_TEXTURE = {0};
DrawMode       g_brush = MeCircle;
int            g_brush_size = 3;
const char     *g_modeString = "";
Color          g_erase_color = WHITE;
int            g_selected;
/*********************************** GLOBALS ***********************************/

// Radio Buttons
Rectangle g_modeCircle = {.x = 25, .y = 25, .width = 25, .height = 25 };
Rectangle g_modeSquare = {.x = 55, .y = 25, .width = 25, .height = 25 };

// Hexboxes
HexBox g_box_brush_color;

// Numboxes
NumBox g_box_brush_size;

#include <stdarg.h>
#define FATAL(...) do { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(-1); } while(0)

void canvas_init(int x, int y, int width, int height, int pixel_size){
   g_CANVAS.x = x;
   g_CANVAS.y = y;
   g_CANVAS.height = height;
   g_CANVAS.width = width;
   
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
   DrawTextureEx(g_CANVAS_TEXTURE, CLITERAL(Vector2){g_CANVAS.x, g_CANVAS.y}, 0, g_scaling, WHITE);
   DrawRectangleLinesEx(CANVAS_RECT(), 2, BLACK);
}

int pos_to_canvas_index(Vector2 pos, int *x, int *y){
   if(CheckCollisionPointRec(pos, CANVAS_RECT())){
      *x = (pos.x - g_CANVAS.x) / g_CANVAS.pixel_size;
      *y = (pos.y - g_CANVAS.y) / g_CANVAS.pixel_size;

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
   SetTargetFPS(60);

   
   g_box_brush_color = make_hexbox(GREEN, 10, 100);
   g_box_brush_size = make_numbox(3, 2, 10, 250);

   canvas_init(125, 50, SCREEN_WIDTH - 125, SCREEN_HEIGHT - 100, 1);
}

float elapsed;

void drawme_mainloop(){
   while (!WindowShouldClose())
   {

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
      }else if(g_selected == SCALING_SELECT){
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
         DrawRectangleLinesEx(g_modeCircle, 1, BLACK);
         DrawRectangleLinesEx(g_modeSquare, 1, BLACK);
         DrawText(g_modeString, 25, 55, 20, BLACK);
         draw_hexbox(&g_box_brush_color);
         draw_numbox(&g_box_brush_size);

         // Canvas
         canvas_draw();
         

      EndDrawing();
      //----------------------------------------------------------------------------------
   }
}

void drawme_close(){
    canvas_close();
    CloseWindow();        // Close window and OpenGL context
}

HexBox make_hexbox(Color color, int x, int y){
   HexBox box = {
      .rect = {
         .x = x, .y = y, 
         .height = 100
      },
      .value = color,
      .size = 10
   };

   sprintf(box.hex, "0x%08X", color_to_int(color));
   box.rect.width = MeasureText("0xFFFFFFFF", 15) + 2 * HEXBOX_PAD;

   return box;
}

void update_hexbox(HexBox *hex){
   int character = 0;
   int updated = 0;

   if(IsKeyPressed(KEY_BACKSPACE)){
      if(hex->size > 2){
         hex->size--;
         updated = 1;
      }
   }
   while((character = GetCharPressed()) != 0){
      if(hex->size >= 10) break;
      
      if((character >= '0' && character <= '9') || (character >= 'A' && character <= 'F') || (character >= 'a' && character <= 'f')){
         hex->hex[hex->size++] = toupper((char)character);
         updated = 1;
      }
   }
   if(updated){
      hex->hex[hex->size] = 0;

      unsigned hc;
      sscanf(hex->hex, "0x%x", &hc);
      // RRGGBBAA
      hex->value = int_to_color(hc);
   }
}

void draw_hexbox(HexBox *hex){
   // Format for hex box is
   // 0xXXXXXXXX
   // r %d   [c]
   // g %d
   // b %d
   // a %d
   // color
   char buffer[32];
   int x_offset = hex->rect.x + HEXBOX_PAD;

   DrawText(hex->hex, x_offset, hex->rect.y, 15, BLACK);
   // R
   sprintf(buffer, "R: %d", hex->value.r);
   int color_box_offset = 50;
   DrawText(buffer, x_offset, hex->rect.y + 21, 15, BLACK);
   // G
   sprintf(buffer, "G: %d", hex->value.g);
   DrawText(buffer, x_offset, hex->rect.y + 41, 15, BLACK);
   // B
   sprintf(buffer, "B: %d", hex->value.b);
   DrawText(buffer, x_offset, hex->rect.y + 61, 15, BLACK);
   // A
   sprintf(buffer, "A: %d", hex->value.a);
   DrawText(buffer, x_offset, hex->rect.y + 81, 15, BLACK);

   // Color Box
   DrawRectangle(x_offset + color_box_offset, hex->rect.y + 21, 15, 15, hex->value);
   DrawRectangleLines(x_offset + color_box_offset, hex->rect.y + 21, 15, 15, BLACK);

   // Border of the whole thing
   DrawRectangleLinesEx(hex->rect, 1, BLACK);
}

NumBox make_numbox(int deflt, int max_chars, int x, int y){
   NumBox box = {
      .rect = {
         .x = x, .y = y, 
         .height = 20
      },
      .value = deflt,
      .size = 1,
      .max_size = max_chars
   };

   assert(box.max_size < 32);

   sprintf(box.number, "%d", deflt);
   char buff[32];
   sprintf(buff, "%0*d", max_chars, 0);
   box.rect.width = MeasureText(buff, 15) + 2 * HEXBOX_PAD;

   return box;
}

void update_numbox(NumBox *num){
   int character = 0;
   int updated = 0;

   if(IsKeyPressed(KEY_BACKSPACE)){
      if(num->size > 0){
         num->size--;
         updated = 1;
      }
   }

   while((character = GetCharPressed()) != 0){
      if(num->size >= num->max_size) break;

      if(num->size == 0 && (character == '-' || character == '+')){
         num->number[num->size++] = (char)character;
         updated = 1;
      }else if(isdigit(character)){
         num->number[num->size++] = (char)character;
         updated = 1;
      }
   }
   if(updated){
      num->number[num->size] = 0;

      if(num->size == 0){
         num->value = 0;
      }else{
         num->value = atoi(num->number);
      }

   }
}

void draw_numbox(NumBox *num){
   // The number
   DrawText(num->number, num->rect.x + HEXBOX_PAD, num->rect.y + HEXBOX_PAD, 15, BLACK);
   
   // The border
   DrawRectangleLinesEx(num->rect, 1, BLACK);
}

uint32_t color_to_int(Color c){
   printf("%x %x %x %x\n", c.r, c.g, c.b, c.a);
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
   printf("%x %x %x %x\n", c.r, c.g, c.b, c.a);

   return c;
}
