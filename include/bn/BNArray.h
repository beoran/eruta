#import <bn/BNObject.h>

@interface BNArray : BNObject 
  + new_size: (int) size;
  - init_size:(int) amount;
  - init;
  - done ;
  - (BOOL) range_ok: (int) index ;
  - get: (int) index ;
  - put_raw: (int) index : (id) value; 
  - put: (int) index : (id) value;
@end




