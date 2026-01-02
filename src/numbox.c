#include <drawme.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#include "raylib.h"

NumBox make_numbox(int deflt, int max_chars, int x, int y){
   NumBox box = {
      .rect = {
         .x = x, .y = y, 
         .height = 20
      },
      .deflt = deflt,
      .value = deflt,
      .size = 1,
      .max_size = max_chars
   };

   assert(box.max_size < 32);

   set_numbox_str(&box);
   char buff[32];
   sprintf(buff, "%0*d", max_chars, 0);
   box.rect.width = MeasureText(buff, 15) + 2 * HEXBOX_PAD;

   return box;
}

void limit_numbox(NumBox *num, int upper, int lower){
   if(num->value > upper){
      num->value = upper;
      set_numbox_str(num);
   }else if(num->value < lower){
      num->value = lower;
      set_numbox_str(num);
   }
}

int update_numbox(NumBox *num){
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

      set_numbox_value(num);
   }

   return updated;
}

void set_numbox_value(NumBox *num){
   if(num->size == 0){
      num->value = num->deflt;
   }else{
      num->value = atoi(num->number);
   }
}

void set_numbox_str(NumBox *num){
   sprintf(num->number, "%d", num->value);
   num->size = (int)strlen(num->number);
}

void draw_numbox(NumBox *num){
   // The number
   DrawText(num->number, num->rect.x + HEXBOX_PAD, num->rect.y + HEXBOX_PAD, 15, BLACK);
   
   // The border
   DrawRectangleLinesEx(num->rect, 1, BLACK);
}

