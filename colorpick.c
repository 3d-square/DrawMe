#include <raylib.h>
#include <stdio.h>

// Color gradient[256][256] = {0xFF};
//

int scaling = 2;

int main(){
   InitWindow(800, 800, "DrawMe - The slutty tool");
   SetTargetFPS(10);

   Image grad = GenImageGradientLinear(256, 256, 45, (Color){.r = 0, .g = 0, .b = 0, .a = 0xFF}, (Color){.r = 0, .g = 0, .b = 0xFF, .a = 0xFF});
   Texture2D tex = LoadTextureFromImage(grad);
   Rectangle r_grade = {530, 0, 25, 256 * scaling};
   Rectangle g_grade = {560, 0, 25, 256 * scaling};
   Rectangle b_grade = {590, 0, 25, 256 * scaling};
   unsigned r = 0;
   unsigned g = 0;
   unsigned b = 0xFF;
   char buffer[32] = {0};
   Color chosen = WHITE;
   while (!WindowShouldClose())
   {
      // 
      //
      if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
         Vector2 mouse_pos = GetMousePosition();
         printf("pos: %f %f\n", mouse_pos.x, mouse_pos.y);

         if(CheckCollisionPointRec(mouse_pos, r_grade)){
            // Get height from bottom
            r = 255 - (int)(mouse_pos.y / scaling);
         }else if(CheckCollisionPointRec(mouse_pos, g_grade)){
            // Get height from bottom
            g = 255 - (int)(mouse_pos.y / scaling);
         }else if(CheckCollisionPointRec(mouse_pos, b_grade)){
            // Get height from bottom
            b = 255 - (int)(mouse_pos.y / scaling);
         }else if(CheckCollisionPointRec(mouse_pos, (Rectangle){0, 0, 256 * scaling, 256 * scaling})){
            chosen = GetImageColor(grad, mouse_pos.x / scaling, mouse_pos.y / scaling);
            sprintf(buffer, "0x%02X%02X%02X%0X", (unsigned)chosen.r, (unsigned)chosen.g, (unsigned)chosen.b, 0xFF);
         }
      }


      UnloadImage(grad);
      grad = GenImageGradientLinear(256, 256, 0, (Color){.r = 0, .g = 0, .b = 0, .a = 0xFF}, (Color){.r = r, .g = g, .b = b, .a = 0xFF});
      UpdateTexture(tex, grad.data);
      BeginDrawing();
         ClearBackground(WHITE);

         // Gradient
         DrawTextureEx(tex, (Vector2){0, 0}, 0, 2, WHITE);
         DrawRectangleLines(0, 0, 256 * scaling, 256 * scaling, BLACK);

         // Hex Value
         DrawText(buffer, 15, 256 * scaling + 20, 20, BLACK);
         DrawRectangle(15, 256 * scaling + 50, 25, 25, chosen);

         // RGB Offset
         DrawRectangle(r_grade.x, r_grade.height - r * scaling, r_grade.width, r * scaling, BLUE); // Fill Up Bar
         DrawRectangleLinesEx(r_grade, 1, BLACK);

         DrawRectangle(g_grade.x, g_grade.height - g * scaling, g_grade.width, g * scaling, BLUE); // Fill Up Bar
         DrawRectangleLinesEx(g_grade, 1, BLACK);

         DrawRectangle(b_grade.x, b_grade.height - b * scaling, b_grade.width, b * scaling, BLUE); // Fill Up Bar
         DrawRectangleLinesEx(b_grade, 1, BLACK);

      EndDrawing();

   }


   UnloadImage(grad);
   UnloadTexture(tex);
   CloseWindow();
}
