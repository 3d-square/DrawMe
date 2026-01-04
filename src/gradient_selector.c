#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"

Gradient make_gradient(Color c, int x, int y){
   Gradient gradient = {.rect = {.x = x, .y = y, .height = 256, .width = 360}};
   gradient.rb = CLITERAL(Rectangle){x + 270, y, 25, 256};
   gradient.gb = CLITERAL(Rectangle){x + 300, y, 25, 256};
   gradient.bb = CLITERAL(Rectangle){x + 330, y, 25, 256};

   gradient.top = c;
   gradient.image = GenImageGradientLinear(256, 256, 45, c, gradient.top);
   
   gradient.texture = LoadTextureFromImage(gradient.image);

   return gradient;
}

void free_gradient(Gradient *gradient){
   UnloadImage(gradient->image);
   UnloadTexture(gradient->texture);
}

void update_gradient(Gradient *gradient, Color top){
   UnloadImage(gradient->image);
   gradient->image = GenImageGradientLinear(256, 256, 45, int_to_color(0x000000FF), top);
   UpdateTexture(gradient->texture, gradient->image.data);
   gradient->top = top;
}

void draw_gradient_selector(Gradient *gradient){
   DrawTextureEx(gradient->texture, CLITERAL(Vector2){gradient->rect.x, gradient->rect.y}, 0, 1, WHITE);

   DrawRectangle(gradient->rb.x, gradient->rect.y + gradient->rb.height - gradient->top.r, gradient->rb.width, gradient->top.r, BLUE);
   DrawRectangleLinesEx(gradient->rb, 1, BLACK);

   DrawRectangle(gradient->gb.x, gradient->rect.y + gradient->gb.height - gradient->top.g, gradient->gb.width, gradient->top.g, BLUE);
   DrawRectangleLinesEx(gradient->gb, 1, BLACK);

   DrawRectangle(gradient->bb.x, gradient->rect.y + gradient->bb.height - gradient->top.b, gradient->bb.width, gradient->top.b, BLUE);
   DrawRectangleLinesEx(gradient->bb, 1, BLACK);
   
   DrawRectangleLines(gradient->rect.x, gradient->rect.y, 360, 256, BLACK);
}

int gradient_selector_callback(void *gradient){
   if(g_selected != GRADIENT_SELECT){
      return 0;
   }

   Color selected;

   if(gradient_callback((Gradient *)gradient, &selected)){
      g_box_brush_color.value = selected;
      set_hexbox_by_color(&g_box_brush_color);
   }
   
   return 1;
}

int gradient_callback(Gradient *grad, Color *out){
   Rectangle img = {grad->rect.x, grad->rect.y, 256, 256};
   if(CheckCollisionPointRec(g_mouse_pos, grad->rb)){
      grad->top.r = 256 - (int)(g_mouse_pos.y - grad->rect.y);
   }else if(CheckCollisionPointRec(g_mouse_pos, grad->gb)){
      grad->top.g = 256 - (int)(g_mouse_pos.y - grad->rect.y);
   }else if(CheckCollisionPointRec(g_mouse_pos, grad->bb)){
      grad->top.b = 256 - (int)(g_mouse_pos.y - grad->rect.y);
   }else if(out && CheckCollisionPointRec(g_mouse_pos, img)){
      *out = GetImageColor(grad->image, g_mouse_pos.x - grad->rect.x, g_mouse_pos.y - grad->rect.y);
      printf("Pixel Color: 0x%08X\n", color_to_int(*out));
      return 1;
   }

   update_gradient(grad, grad->top);

   printf("Gradient was selected\n");
   return 0;
}
