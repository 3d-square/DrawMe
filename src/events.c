#include "raylib.h"
#include <drawme.h>
#include <stdlib.h>
#include <math.h>



static MouseButton mouse_held = -1;
#define MOUSE_NONE 0xFFFFFFFF

int is_mouse_held(){
   return mouse_held != MOUSE_NONE;
}

int left_button_pressed_event(){
   Vector2 mouse_pos = GetMousePosition();

   if(CheckCollisionPointRec(mouse_pos, CANVAS_RECT())){
      int x;
      int y;

      pos_to_canvas_index(mouse_pos, &x, &y);
      canvas_set_cluster(x, y, brush_size, brush, box_brush_color.value);
      canvas_update();

      mouse_held = MOUSE_BUTTON_LEFT;
   }else{
      if(CheckCollisionPointRec(mouse_pos, modeCircle)){
         brush = MeCircle;
      }else if(CheckCollisionPointRec(mouse_pos, modeSquare)){
         brush = MeSquare;
      }
   }
   return 1;
}

int right_button_pressed_event(){
   Vector2 mouse_pos = GetMousePosition();

   if(CheckCollisionPointRec(mouse_pos, CANVAS_RECT())){
      int x;
      int y;

      pos_to_canvas_index(mouse_pos, &x, &y);
      canvas_set_cluster(x, y, brush_size, brush, erase_color);
      canvas_update();

      mouse_held = MOUSE_BUTTON_RIGHT;
   }
   return 1;
}

int button_held_event(){
   if(is_mouse_held()){
      Vector2 mouse_pos = GetMousePosition();
      if(CheckCollisionPointRec(mouse_pos, CANVAS_RECT())){
         int x;
         int y;

         pos_to_canvas_index(mouse_pos, &x, &y);

         canvas_set_cluster(x, y, brush_size, brush, mouse_held == MOUSE_BUTTON_RIGHT ? erase_color : box_brush_color.value);

         canvas_update();
      }
   }
   return 1;
}

int right_button_released_event(){
   mouse_held = MOUSE_NONE;
   return 1;
}
int left_button_released_event(){
   mouse_held = MOUSE_NONE;
   return 1;
}
