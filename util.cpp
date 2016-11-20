#include "gamecore.h"
#include "assets.h"

bool Collision(
  const int x1, const char y1, const byte w1, const byte h1, 
  const int x2, const char y2, const byte w2, const byte h2
) {
  if(x1 - w1/2 <= x2 + w2/2 && x1 + w1/2 >= x2 - w2/2 &&
     y1 - h1/2 <= y2 + h2/2 && y1 + h1/2 >= y2 - h2/2) {
    return true;    
  }
  return false;
}
