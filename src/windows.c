#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"

/*********************************** GLOBALS ***********************************/
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

DrawMeCanvas CANVAS;
/*********************************** GLOBALS ***********************************/

#include <stdarg.h>
#define FATAL(...) do { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(-1); } while(0)

#define CANVAS_PIXEL(x, y) (CANVAS.image + ((y) * CANVAS.cols) + (x))
#define CANVAS_RECT() (CLITERAL(Rectangle){CANVAS.x, CANVAS.y, CANVAS.width, CANVAS.height})

void canvas_init(int x, int y, int width, int height, int pixel_size){
   CANVAS.x = x;
   CANVAS.y = y;
   CANVAS.height = height;
   CANVAS.width = width;

   CANVAS.rows = height / pixel_size;
   CANVAS.cols = width / pixel_size;

   CANVAS.image = calloc(1, CANVAS.rows * CANVAS.cols * sizeof(Color));
   if(CANVAS.image == NULL){
      FATAL("Unable to allocate memory for the canvas\n");
   }

   CANVAS.pixel_size = pixel_size;
}

void canvas_close(){
   free(CANVAS.image);
}

void canvas_draw(){
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
}

Color *get_canvas_pixel(Vector2 pos){
   if(CheckCollisionPointRec(pos, CANVAS_RECT())){
      int x = (pos.x - CANVAS.x) / CANVAS.pixel_size;
      int y = (pos.y - CANVAS.y) / CANVAS.pixel_size;

      return CANVAS_PIXEL(x, y);
   }

   return NULL;
}

void drawme_init(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DrawMe - The slutty tool");
    SetTargetFPS(60);

    canvas_init(50, 50, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100, 25);
}

void drawme_mainloop(){
   while (!WindowShouldClose())
   {

      if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
         Color *pixel = get_canvas_pixel(GetMousePosition());

         if(pixel){
            pixel->r += 25;
            pixel->g += 25;
            pixel->b += 25;
            pixel->a += 25;
         }
      }else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
         Color *pixel = get_canvas_pixel(GetMousePosition());

         if(pixel){
            pixel->r += 0;
            pixel->g += 0;
            pixel->b += 0;
            pixel->a += 0;
         }
      }

      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();

          ClearBackground(RAYWHITE);

          canvas_draw();

      EndDrawing();
      //----------------------------------------------------------------------------------
   }
}

void drawme_close(){
    canvas_close();
    CloseWindow();        // Close window and OpenGL context
}
