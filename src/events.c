#include "raylib.h"
#include <drawme.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>


#define MOUSE_NONE 0xFFFFFFFF
static MouseButton mouse_held = MOUSE_NONE;

int is_mouse_held(){
   return mouse_held != MOUSE_NONE;
}

int left_button_pressed_event(){
   Vector2 mouse_pos = GetMousePosition();
   g_selected = NONE_SELECT;
   if(CheckCollisionPointRec(mouse_pos, CANVAS_RECT())){
      int x;
      int y;

      pos_to_canvas_index(mouse_pos, &x, &y);
      printf("%x %x %x %x\n", g_box_brush_color.value.r, g_box_brush_color.value.g, g_box_brush_color.value.b, g_box_brush_color.value.a);
      canvas_set_cluster(x, y, g_box_brush_size.value, g_brush, g_box_brush_color.value);
      canvas_update();

      mouse_held = MOUSE_BUTTON_LEFT;
   }else{
      if(CheckCollisionPointRec(mouse_pos, g_modeCircle)){
         g_brush = MeCircle;
      }else if(CheckCollisionPointRec(mouse_pos, g_modeSquare)){
         g_brush = MeSquare;
      }else if(CheckCollisionPointRec(mouse_pos, g_box_brush_color.rect)){
         mouse_held = MOUSE_NONE;
         g_selected = BRUSH_COLOR_SELECT;
      }else if(CheckCollisionPointRec(mouse_pos, g_box_brush_size.rect)){
         mouse_held = MOUSE_NONE;
         g_selected = SCALING_SELECT;
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
      canvas_set_cluster(x, y, g_box_brush_size.value, g_brush, g_erase_color);
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

         canvas_set_cluster(x, y, g_box_brush_size.value, g_brush, mouse_held == MOUSE_BUTTON_RIGHT ? g_erase_color : g_box_brush_color.value);

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
