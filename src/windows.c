#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "raylib.h"

/*********************************** GLOBALS ***********************************/
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

int scaling = 1;
DrawMeCanvas CANVAS = {0};
Texture2D CANVAS_TEXTURE = {0};
DrawMode brush = MeCircle;
const char *modeString = "";
/*********************************** GLOBALS ***********************************/

// Radio Buttons
Rectangle modeCircle = {.x = 25, .y = 25, .width = 25, .height = 25 };
Rectangle modeSquare = {.x = 55, .y = 25, .width = 25, .height = 25 };

#include <stdarg.h>
#define FATAL(...) do { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(-1); } while(0)

#define CANVAS_PIXEL(i, j) (CANVAS.image + ((i) * CANVAS.cols) + (j))
#define CANVAS_RECT() (CLITERAL(Rectangle){CANVAS.x, CANVAS.y, CANVAS.width, CANVAS.height})

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
      // printf("r2: %d, xh: %d, xl: %d, yh: %d, xl: %d\n", r2, x_hi, x_low, y_hi, y_low);

      for(int i = y_low; i <= y_hi; ++i){
         int dy = i - y;
         for(int j = x_low; j <= x_hi; ++j){
            int dx = j - x;
            // printf("dx2: %d, dy2: %d\n", dx * dx, dy * dy);
            if(dx * dx + dy * dy < r2){
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
    // SetTargetFPS(60);

    canvas_init(50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, 1);
}

int mouse_held = 0;
float elapsed;

void drawme_mainloop(){
   while (!WindowShouldClose())
   {
      Vector2 mousePos = GetMousePosition();

      int lmb = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
      int rmb = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);

      if(lmb || rmb || mouse_held){
         if(CheckCollisionPointRec(mousePos, CANVAS_RECT())){
            int x;
            int y;

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || mouse_held == 1){
               if(pos_to_canvas_index(mousePos, &x, &y)){
                  canvas_set_cluster(x, y, 4, brush, GREEN);
                  canvas_update();
               }
               mouse_held = 1;
            }else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || mouse_held == 2){
               pos_to_canvas_index(mousePos, &x, &y);
               canvas_set_cluster(x, y, 4, brush, WHITE);
               canvas_update();

               mouse_held = 2;
            }
         }else if(lmb){
            if(CheckCollisionPointRec(mousePos, modeCircle)){
               brush = MeCircle;
            }else if(lmb && CheckCollisionPointRec(mousePos, modeSquare)){
               brush = MeSquare;
            }
         }
      }

      if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) || IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)){
         mouse_held = 0;
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
