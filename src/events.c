#include "raylib.h"
#include <drawme.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#define MOUSE_NONE 0xFFFFFFFF
static MouseButton mouse_held = MOUSE_NONE;

void ui_setup_callbacks(){
   ui_add_element(MOUSE_BUTTON_LEFT, (Rectangle*)&g_box_brush_color, brush_color_callback);
   ui_add_element(MOUSE_BUTTON_LEFT, (Rectangle*)&g_box_brush_size, brush_size_callback);

   ui_add_element(MOUSE_BUTTON_LEFT,  (Rectangle*)&g_CANVAS, canvas_callback);
   ui_add_element(MOUSE_BUTTON_RIGHT, (Rectangle*)&g_CANVAS, canvas_r_callback);

   ui_add_element(MOUSE_BUTTON_LEFT, &g_modeSquare, mode_square_callback);
   ui_add_element(MOUSE_BUTTON_LEFT, &g_modeCircle, mode_circle_callback);
}

int is_mouse_held(){
   return mouse_held != MOUSE_NONE;
}

int left_button_pressed_event(){
   g_selected = NONE_SELECT;
   for(int i = 0; i < g_UI.num_lmb_buttons; ++i){
      if(CheckCollisionPointRec(g_mouse_pos, *g_UI.lmb_clickable[i].rect)){
         return g_UI.lmb_clickable[i].callback(g_UI.lmb_clickable[i].rect);
      }
   }
   return 0;
}

int right_button_pressed_event(){
   g_selected = NONE_SELECT;
   for(int i = 0; i < g_UI.num_rmb_buttons; ++i){
      if(CheckCollisionPointRec(g_mouse_pos, *g_UI.rmb_clickable[i].rect)){
         return g_UI.rmb_clickable[i].callback(g_UI.rmb_clickable[i].rect);
      }
   }  
   return 1;
}

// Callback functions
int canvas_callback(void *){
   int x;
   int y;

   pos_to_canvas_index(g_mouse_pos, &x, &y);
   canvas_set_cluster(x, y, g_box_brush_size.value - 1, g_brush, g_box_brush_color.value);
   canvas_update();

   mouse_held = MOUSE_BUTTON_LEFT;

   return 1;
}

int canvas_r_callback(void *){
   int x;
   int y;

   pos_to_canvas_index(g_mouse_pos, &x, &y);
   canvas_set_cluster(x, y, g_box_brush_size.value - 1, g_brush, g_erase_color);
   canvas_update();

   mouse_held = MOUSE_BUTTON_RIGHT;
   return 1;
}

int mode_square_callback(void *){
   g_brush = MeSquare;
   return g_brush;
}

int mode_circle_callback(void *){
   g_brush = MeCircle;
   return g_brush;
}

int hexbox_callback(HexBox *hex){
   if(CheckCollisionPointRec(g_mouse_pos, hex->r.rect)){
      hex->selected = 1;
   }else if(CheckCollisionPointRec(g_mouse_pos, hex->g.rect)){
      hex->selected = 2;

   }else if(CheckCollisionPointRec(g_mouse_pos, hex->b.rect)){
      hex->selected = 3;

   }else if(CheckCollisionPointRec(g_mouse_pos, hex->a.rect)){
      hex->selected = 4;
   }else{
      hex->selected = 0;
   }
   return 0;
}

int brush_color_callback(void *hex){
   mouse_held = MOUSE_NONE;
   g_selected = BRUSH_COLOR_SELECT;
   hexbox_callback((HexBox *)hex);
   return 1;
}

int brush_size_callback(void *){
   mouse_held = MOUSE_NONE;
   g_selected = SCALING_SELECT;
   return 1;
}

// Callback functions

int button_held_event(){
   if(is_mouse_held()){
      if(CheckCollisionPointRec(g_mouse_pos, g_CANVAS.rect)){
         int x;
         int y;

         pos_to_canvas_index(g_mouse_pos, &x, &y);

         canvas_set_cluster(x, y, g_box_brush_size.value - 1, g_brush, mouse_held == MOUSE_BUTTON_RIGHT ? g_erase_color : g_box_brush_color.value);

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

void ui_add_element(MouseButton button, Rectangle *bounds, button_callback callback){
   if(button == MOUSE_BUTTON_LEFT){
      assert(g_UI.num_lmb_buttons < MAX_UI_ELEMENTS && "Too many lmb events");
      
      g_UI.lmb_clickable[g_UI.num_lmb_buttons] = CLITERAL(Clickable){
         .rect = bounds,
         .callback = callback
      };
      g_UI.num_lmb_buttons++;
   }else if(button == MOUSE_BUTTON_RIGHT){
      assert(g_UI.num_rmb_buttons < MAX_UI_ELEMENTS && "Too many lmb events");
      
      g_UI.rmb_clickable[g_UI.num_rmb_buttons] = CLITERAL(Clickable){
         .rect = bounds,
         .callback = callback
      };
      g_UI.num_rmb_buttons++;
   }
}
