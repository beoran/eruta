/**
* This is a test for sprite in $package$
*/
#include "si_test.h"
#include "eruta.h"
#include "sprite.h"


TEST_FUNC(sprite) {
  SpriteList    * list;
  Sprite        * sprite;
  SpriteAction  * act;
  SpriteFrame   * frame;
  SpriteLayer   * layer;
  Point           offset = cpv(1.0, 2.0);
  sprite = sprite_new(1);
  list = spritelist_new();
  TEST_NOTNULL(list);
  TEST_NOTNULL(spritelist_getornew(list, 123));

  
  TEST_NOTNULL(sprite);
  TEST_NULL(sprite_action(sprite, 0));
  TEST_NULL(sprite_action(sprite, 1));
  TEST_NULL(sprite_frame(sprite, 1, 2));
  TEST_NULL(sprite_layer(sprite, 1, 2, 3));
  
  act    = sprite_newaction(sprite, 0, SPRITE_NORTH, SPRITE_ACTIVE);
  TEST_NOTNULL(act);
  TEST_PTREQ(act, sprite_action(sprite, 0));
  act    = sprite_newaction(sprite, 0, SPRITE_NORTH,SPRITE_ACTIVE);
  TEST_NOTNULL(act);
  TEST_PTREQ(act, sprite_action(sprite, 0));
  
  frame  = sprite_newframe(sprite, 0, 1,  SPRITE_ACTIVE, 0.25);
  TEST_NOTNULL(frame);
  TEST_PTREQ(frame, sprite_frame(sprite, 0, 1));
  
  layer  = sprite_newlayer(sprite, 0, 1, 2, NULL, offset);
  TEST_NOTNULL(layer);
  TEST_PTREQ(layer, sprite_layer(sprite, 0, 1, 2));
 
  /* Check if out of bounds new works and doesn't leak. */
  act    = sprite_newaction(sprite, 100, SPRITE_NORTH, SPRITE_ACTIVE);
  TEST_NULL(act);
  
  layer  = sprite_newlayer(sprite, 0, 1, 22, NULL, offset);
  TEST_NULL(layer);
  TEST_PTREQ(layer, sprite_layer(sprite, 0, 1, 22));
 
  frame  = sprite_newframe(sprite, 0, 111,  SPRITE_ACTIVE, 0.25);
  TEST_NULL(frame);
  
  layer  = sprite_newlayer(sprite, 0, 1, 222, NULL, offset);
  TEST_NULL(layer);
  
  TEST_NOTNULL(sprite_maxactions_(sprite, 77));
  TEST_INTEQ(77, sprite_maxactions(sprite));  
  act    = sprite_newaction(sprite, 60, SPRITE_NORTH, SPRITE_ACTIVE);
  TEST_NOTNULL(sprite_action(sprite, 60));
  
  TEST_NOTNULL(sprite_maxactions_(sprite, 1));
  TEST_NULL(sprite_action(sprite, 60));
  TEST_INTEQ(1, sprite_maxactions(sprite));  
  TEST_NOTNULL(sprite_action(sprite, 0));
  act = sprite_action(sprite, 0);
  if (act) {
    TEST_NOTNULL(spriteaction_maxframes_(act, 3));
  }
  spritelist_free(list);
  sprite_free(sprite);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(sprite);
  TEST_REPORT();
}



