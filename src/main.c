#include <drawme.h>
#include "raylib.h"

int main(void)
{
   drawme_init();

   // Main game loop
   drawme_mainloop();

   drawme_close();
   return 0;
}

