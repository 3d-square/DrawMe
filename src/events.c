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
   ui_add_element(MOUSE_BUTTON_LEFT, (Rectangle*)&g_gradient_selector, gradient_selector_callback);

   ui_add_element(MOUSE_BUTTON_LEFT,  (Rectangle*)&g_CANVAS, canvas_callback);
   ui_add_element(MOUSE_BUTTON_RIGHT, (Rectangle*)&g_CANVAS, canvas_r_callback);

   ui_add_element(MOUSE_BUTTON_LEFT, &g_modeSquare, mode_square_callback);
   ui_add_element(MOUSE_BUTTON_LEFT, &g_modeCircle, mode_circle_callback);
}

int is_mouse_held(){
   return mouse_held != MOUSE_NONE;
}

int left_button_pressed_event(){
   int return_value = 0;

   if(g_selected != GRADIENT_SELECT){
      set_select(NONE_SELECT);
   }else if(!CheckCollisionPointRec(g_mouse_pos, g_gradient_selector.rect)){ // g_selected == GRADIENT_SELECT
      set_select(NONE_SELECT);
      return 0;
   }

   for(int i = 0; i < g_UI.num_lmb_buttons; ++i){
      if(CheckCollisionPointRec(g_mouse_pos, *g_UI.lmb_clickable[i].rect)){
         if(g_UI.lmb_clickable[i].callback(g_UI.lmb_clickable[i].rect)){
            return_value = 1;
            break;
         }
      }
   }
   return return_value;
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
   return 1;
}

int mode_circle_callback(void *){
   g_brush = MeCircle;
   return 1;
}

int brush_color_callback(void *hex){
   if(g_selected == GRADIENT_SELECT){
      return 0;
   }
   HexBox *hexbox = ((HexBox *)hex);
   mouse_held = MOUSE_NONE;
   set_select(BRUSH_COLOR_SELECT);
   hexbox_callback(hexbox);

   if(hexbox->selected == 5){
      set_select(GRADIENT_SELECT);
      hexbox->selected = 0;
   }
   return 1;
}

int brush_size_callback(void *){
   mouse_held = MOUSE_NONE;
   set_select(SIZE_SELECT);
   return 1;
}

// Callback functions
int button_held_event(){
   if(CheckCollisionPointRec(g_mouse_pos, g_CANVAS.rect)){
      int x;
      int y;

      pos_to_canvas_index(g_mouse_pos, &x, &y);

      Color c = mouse_held == MOUSE_BUTTON_RIGHT ? g_erase_color : g_box_brush_color.value;

      if(!CheckCollisionPointRec(g_mouse_pos, g_CANVAS.rect) || (g_mouse_pos.x == g_prev_mouse_pos.x && g_mouse_pos.y == g_prev_mouse_pos.y)){
         canvas_set_cluster(x, y, g_box_brush_size.value - 1, g_brush, c);
      }else{
         Vec2 start;
         pos_to_canvas_index(g_prev_mouse_pos, &start.x, &start.y);

         canvas_set_interpolated_line(start, CLITERAL(Vec2){x, y}, g_brush, g_box_brush_size.value - 1, c);
      }

      canvas_update();
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
