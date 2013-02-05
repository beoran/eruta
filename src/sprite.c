#include "image.h"
#include "sprite.h"
#include "rebox.h"




/* A SpriteFrame is a single frame of animation of a sprite. 
 * It consists of a bounds rectangle, a image pointer, an index in the 
 * animation, and a duration in ms of the frame. The flags field 
 * registerrs if the image pointer is owned, etc.
 */
struct SpriteFrame_ {
  Rebox         bounds; 
  Image       * image;
  int           index;
  double        duration;
  int           flags;
};

static SpriteFrame 
SpriteFrameZero = { 0 };

/* Get Tarray interface, generate for SpriteFrame type */
#define TEMPLATE_ZERO   SpriteFrameZero
#define TEMPLATE_T      SpriteFrame
#define TEMPLATE_NAME   SpriteFrameArray
#define TEMPLATE_PREFIX spriteframearray_
#include "tarray.h"

/* Get Tarray implementation, generate for SpriteFrame type */
#define TEMPLATE_ZERO   SpriteFrameZero
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      SpriteFrame
#define TEMPLATE_NAME   SpriteFrameArray
#define TEMPLATE_PREFIX spriteframearray_
#include "tarray.h"



struct SpriteAction_ {
  int index;
  int flags;
  SpriteFrameArray frames;
};


static SpriteAction 
SpriteActionZero = { 0 };

/* Get Tarray interface, generate for SpriteFrame type */
#define TEMPLATE_ZERO   SpriteFrameZero
#define TEMPLATE_T      SpriteFrame
#define TEMPLATE_NAME   SpriteFrameArray
#define TEMPLATE_PREFIX spriteframearray_
#include "tarray.h"

/* Get Tarray implementation, generate for SpriteFrame type */
#define TEMPLATE_ZERO   SpriteFrameZero
#define TEMPLATE_IMPLEMENT
#define TEMPLATE_T      SpriteFrame
#define TEMPLATE_NAME   SpriteFrameArray
#define TEMPLATE_PREFIX spriteframearray_
#include "tarray.h"


struct Sprite_ {
  Image * sheet; // the sprite's sheet. It's an image that holds all frames.
  
  
  
};








