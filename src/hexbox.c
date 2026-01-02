#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"

HexBox make_hexbox(Color color, int x, int y){
   HexBox box = {
      .rect = {
         .x = x, .y = y, 
         .height = 120
      },
      .value = color,
      .size = 10
   };

   // Magic Numbers
   int width_offset = 17;
   box.rect.width  = 106;

   box.r = make_numbox(0, 3, x + HEXBOX_PAD + width_offset, y + 21);
   box.g = make_numbox(0, 3, x + HEXBOX_PAD + width_offset, y + 46);
   box.b = make_numbox(0, 3, x + HEXBOX_PAD + width_offset, y + 71);
   box.a = make_numbox(0, 3, x + HEXBOX_PAD + width_offset, y + 96);

   sprintf(box.hex, "0x%08X", color_to_int(color));
   set_hexbox_value(&box);

   return box;
}

void set_hexbox_value(HexBox *hex){
   uint32_t hc;
   sscanf(hex->hex, "0x%x", &hc);
   hex->value = int_to_color(hc);
   hex->r.value = hex->value.r;
   set_numbox_str(&hex->r);
   hex->g.value = hex->value.g;
   set_numbox_str(&hex->g);
   hex->b.value = hex->value.b;
   set_numbox_str(&hex->b);
   hex->a.value = hex->value.a;
   set_numbox_str(&hex->a);
}

void set_hexbox_by_color(HexBox *hex){
   sprintf(hex->hex, "0x%08X", color_to_int(hex->value));
   set_hexbox_value(hex);
   hex->r.value = hex->value.r;
   set_numbox_str(&hex->r);
   hex->g.value = hex->value.g;
   set_numbox_str(&hex->g);
   hex->b.value = hex->value.b;
   set_numbox_str(&hex->b);
   hex->a.value = hex->value.a;
   set_numbox_str(&hex->a);
}

int update_hexbox(HexBox *hex){
   int character = 0;
   int updated = 0;

   if(hex->selected == 0){
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

         hex->r.value = hex->value.r;
         set_numbox_str(&hex->r);
         hex->g.value = hex->value.g;
         set_numbox_str(&hex->g);
         hex->b.value = hex->value.b;
         set_numbox_str(&hex->b);
         hex->a.value = hex->value.a;
         set_numbox_str(&hex->a);
      }
      return updated;
   }else if(hex->selected == 1){
      if((updated = update_numbox(&hex->r))){
         limit_numbox(&hex->r, 0xFF, 0);
      }
   }else if(hex->selected == 2){
      if((updated = update_numbox(&hex->g))){
         limit_numbox(&hex->g, 0xFF, 0);     
      }
   }else if(hex->selected == 3){
      if((updated = update_numbox(&hex->b))){
         limit_numbox(&hex->b, 0xFF, 0);    
      }
   }else if(hex->selected == 4){
      if((updated = update_numbox(&hex->a))){
         limit_numbox(&hex->a, 0xFF, 0);   
      }
   }
   if(updated) set_hexbox_str(hex);
   return updated;
}

void set_hexbox_str(HexBox *hex){
   hex->value.r = hex->r.value & 0xFF;
   hex->value.g = hex->g.value & 0xFF;
   hex->value.b = hex->b.value & 0xFF;
   hex->value.a = hex->a.value & 0xFF;
   
   sprintf(hex->hex, "0x%08X", color_to_int(hex->value));
}

void draw_hexbox(HexBox *hex){
   // Format for hex box is
   // 0xXXXXXXXX
   // r %d   [c]
   // g %d
   // b %d
   // a %d
   // color
   int x_offset = hex->rect.x + HEXBOX_PAD;

   DrawText(hex->hex, x_offset, hex->rect.y + HEXBOX_PAD, 15, BLACK);
   // R
   draw_numbox(&hex->r);
   DrawText("R: ", x_offset, hex->r.rect.y + HEXBOX_PAD, 15, BLACK);
   // G
   draw_numbox(&hex->g);
   DrawText("G: ", x_offset, hex->g.rect.y + HEXBOX_PAD, 15, BLACK);
   // B
   draw_numbox(&hex->b);
   DrawText("B: ", x_offset, hex->b.rect.y + HEXBOX_PAD, 15, BLACK);
   // A
   draw_numbox(&hex->a);
   DrawText("A: ", x_offset, hex->a.rect.y + HEXBOX_PAD, 15, BLACK);

   // Color Box
   int color_box_offset = 55;
   DrawRectangle(x_offset + color_box_offset, hex->rect.y + 21, 15, 15, hex->value);
   DrawRectangleLines(x_offset + color_box_offset, hex->rect.y + 21, 15, 15, BLACK);

   // Border of the whole thing
   DrawRectangleLinesEx(hex->rect, 1, BLACK);
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
   }else if(CheckCollisionPointRec(g_mouse_pos, CLITERAL(Rectangle){hex->rect.x + HEXBOX_PAD + 55, hex->rect.y + 21, 15, 15})){
      hex->selected = 5; // The color box was selected this allows for custom implemetations when it is clicked on
   }else{
      hex->selected = 0;
   }
   return 1;
}
