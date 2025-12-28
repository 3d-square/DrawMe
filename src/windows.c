#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

/*********************************** GLOBALS ***********************************/
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

int scaling = 10;
DrawMeCanvas CANVAS = {0};
Texture2D CANVAS_TEXTURE = {0};
DrawMode brush = MeCircle;
int brush_size = 3;
const char *modeString = "";
Color erase_color = WHITE;
/*********************************** GLOBALS ***********************************/

// Radio Buttons
Rectangle modeCircle = {.x = 25, .y = 25, .width = 25, .height = 25 };
Rectangle modeSquare = {.x = 55, .y = 25, .width = 25, .height = 25 };

// Hexboxes
HexBox box_brush_color;

#include <stdarg.h>
#define FATAL(...) do { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(-1); } while(0)

void canvas_init(int x, int y, int width, int height, int pixel_size){
   CANVAS.x = x;
   CANVAS.y = y;
   CANVAS.height = height;
   CANVAS.width = width;
   
   pixel_size *= scaling;

   CANVAS.rows = height / pixel_size;
   CANVAS.cols = width / pixel_size;

   printf("Canvas Size: %d\n", CANVAS.rows * CANVAS.cols);
   printf("Canvas Bytes: %zu\n", CANVAS.rows * CANVAS.cols * sizeof(Color));
   CANVAS.image = calloc(1, CANVAS.rows * CANVAS.cols * sizeof(Color));
   if(CANVAS.image == NULL){
      FATAL("Unable to allocate memory for the canvas\n");
   }

   CANVAS.pixel_size = pixel_size;
}

void canvas_close(){
   free(CANVAS.image);
}

void canvas_update(){
   if(scaling == 1){
      UnloadTexture(CANVAS_TEXTURE);
      Image img = {
         .data    = CANVAS.image,
         .width   = CANVAS.width,
         .height  = CANVAS.height,
         .mipmaps = 1,
         .format  = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
      };

      CANVAS_TEXTURE = LoadTextureFromImage(img);
   }
}

void canvas_draw(){
   if(scaling != 1){
      int pixel_x = 0;
      int pixel_y = CANVAS.y;
      DrawRectangleLinesEx(CANVAS_RECT(), 2, BLACK);
      for(int i = 0; i < CANVAS.rows; ++i){
         pixel_x = CANVAS.x;
         for(int j = 0; j < CANVAS.cols; ++j){
            Color *pixel = CANVAS_PIXEL(i, j);
            if(pixel->a != 0){
               DrawRectangle(pixel_x, pixel_y, CANVAS.pixel_size, CANVAS.pixel_size, *pixel);
            }
            pixel_x += CANVAS.pixel_size;
         }
         pixel_y += CANVAS.pixel_size;
      }
   }else{
      DrawTextureEx(CANVAS_TEXTURE, CLITERAL(Vector2){CANVAS.x, CANVAS.y}, 0, scaling, WHITE);
   }
}

int pos_to_canvas_index(Vector2 pos, int *x, int *y){
   if(CheckCollisionPointRec(pos, CANVAS_RECT())){
      *x = (pos.x - CANVAS.x) / CANVAS.pixel_size;
      *y = (pos.y - CANVAS.y) / CANVAS.pixel_size;

      return 1;
   }

   return 0;
}

Color *get_canvas_pixel(Vector2 pos){
   int x;
   int y;
   if(pos_to_canvas_index(pos, &x, &y)){
      return CANVAS_PIXEL(y, x);
   }

   return NULL;
}

void canvas_set_cluster(int x, int y, int radius, DrawMode mode, Color color){
   int x_low = (int)fmax(0, x - radius),
       x_hi  = (int)fmin(CANVAS.cols - 1, x + radius);
   int y_low = (int)fmax(0, y - radius),
       y_hi  = (int)fmin(CANVAS.rows - 1, y + radius);

   if(mode == MeCircle){
      int r2 = radius * radius;
      printf("r2: %d xl: %d xh: %d yl: %d yh: %d\n", r2, x_low, x_hi, y_low, y_hi);

      for(int i = y_low; i <= y_hi; ++i){
         int dy = i - y;
         for(int j = x_low; j <= x_hi; ++j){
            int dx = j - x;
            if(dx * dx + dy * dy <= r2){
               *CANVAS_PIXEL(i, j) = color;
            }
         }
      }
   }else if(mode == MeSquare){
      for(int i = y_low; i <= y_hi; ++i){
         for(int j = x_low; j <= x_hi; ++j){
            *CANVAS_PIXEL(i, j) = color;
         }
      }
   }
   
}


void drawme_init(){
   InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DrawMe - The slutty tool");
   SetTargetFPS(30);

   canvas_init(100, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, 1);
   
   box_brush_color.x = 10;
   box_brush_color.y = 100;
   sprintf(box_brush_color.hex, "0x%08X", color_to_int(GREEN));
   box_brush_color.value = GREEN;
   box_brush_color.size = 10;
   
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

      update_hexbox(&box_brush_color);

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
         
         if(brush == MeCircle){
            DrawRectangleRec(modeCircle, BLUE);
            modeString = "Circle";
         }else if(brush == MeSquare){
            DrawRectangleRec(modeSquare, BLUE);
            modeString = "Square";
         }
         DrawRectangleLinesEx(modeCircle, 1, BLACK);
         DrawRectangleLinesEx(modeSquare, 1, BLACK);
         DrawText(modeString, 25, 55, 20, BLACK);
         draw_hexbox(&box_brush_color);

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
      
      if((character >= '0' && character <= '9') || (character >= 'A' && character <= 'F')){
         hex->hex[hex->size++] = (char)character;
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
   // r %d
   // g %d
   // b %d
   // a %d
   // color
   char buffer[32];

   DrawText(hex->hex, hex->x, hex->y, 15, BLACK);
   // R
   sprintf(buffer, "R: %d", hex->value.r);
   int color_box_offset = 50;
   DrawText(buffer, hex->x, hex->y + 21, 15, BLACK);
   // G
   sprintf(buffer, "G: %d", hex->value.g);
   DrawText(buffer, hex->x, hex->y + 41, 15, BLACK);
   // B
   sprintf(buffer, "B: %d", hex->value.b);
   DrawText(buffer, hex->x, hex->y + 61, 15, BLACK);
   // A
   sprintf(buffer, "A: %d", hex->value.a);
   DrawText(buffer, hex->x, hex->y + 81, 15, BLACK);

   DrawRectangle(hex->x + color_box_offset, hex->y + 21, 15, 15, hex->value);
   DrawRectangleLines(hex->x + color_box_offset, hex->y + 21, 15, 15, BLACK);
}

unsigned color_to_int(Color c){
   printf("%x %x %x %x\n", c.r, c.g, c.b, c.a);
   unsigned hc = (c.r << 24)
               + (c.g << 16) 
               + (c.b << 8) 
               + c.a;

   printf("hc = %x\n", hc);

   return hc;
}

Color int_to_color(unsigned i){
   Color c = {
      .r = i & 0xFF000000,
      .g = i & 0x00FF0000,
      .b = i & 0x0000FF00,
      .a = i & 0x000000FF
   };
   printf("%x\n", i);
   printf("%x %x %x %x\n", c.r, c.g, c.b, c.a);

   return c;
}
