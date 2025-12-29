#include <drawme.h>
#include "raylib.h"
#include <assert.h>

int main(void)
{
   static_assert(sizeof(uint32_t) == sizeof(Color), "This program needs the Color type to be 32 bits");

   drawme_init();

   // Main game loop
   drawme_mainloop();

   drawme_close();
   return 0;
}

