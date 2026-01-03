#include <raylib.h>
#include <stdio.h>

#define printvec(v) printf("Vec2 x:%f y:%f\n", (v).x, (v).y)

#define ABS(a) (a) > 0 ? (a) : -(a)

typedef struct Vec2{
   int x;
   int y;
} Vec2;

int interpolate(Vec2 start, Vec2 end, int step, Vec2 points[]){
   int dx = ABS(start.x - end.x),
       sx = start.x < end.x ? 1 : -1;

   int dy = ABS(start.y - end.y),
       sy = start.y < end.y ? 1 : -1;

   int err = (dx > dy ? dx : -dy) / 2,
       e2;


   int stepy = step;

   int count = 0;

   while(1){
      if (start.x == end.x && start.y == end.y){
         points[count++] = start;
         break;
      }

      if(stepy >= step){
         stepy = 0;
         points[count++] = start;
      }

      e2 = err;

      if(e2 > -dx){
         err -= dy;
         start.x += sx;
         stepy += 1;
      }

      if(e2 < dy){
         err += dx;
         start.y += sy;
         stepy += 1;
      }
   }
   return count;
}

int main(){
   Vec2 start;
   Vec2 end;

   int step;

   printf("Start Point: ");
   scanf("%d %d", &start.x, &start.y);
   printf("End Point: ");
   scanf("%d %d", &end.x, &end.y);
   printf("Step: ");
   scanf("%d", &step);
   
   Vec2 points[50];
   int count = interpolate(start, end, step, points);

   for(int i = 0; i < count; ++i){
      printf("Point %d %d\n", points[i].x, points[i].y);
   }

   return 0;
}
