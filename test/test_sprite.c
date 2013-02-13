/**
* This is a test for sprite in $package$
*/
#include "si_test.h"
#include "sprite.h"


TEST_FUNC(sprite) {
  Sprite * sprite;
  sprite = sprite_new();
  TEST_NOTNULL(sprite);
  sprite_free(sprite);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(sprite);
  TEST_REPORT();
}



