
#include <objc/runtime.h>
#include <objc/Object.h>
#include <objc/Protocol.h>
#include <objc/NXConstStr.h>


@interface BNObject : Object 
  + alloc;
  
  + free : (id) object;
  
  - init;
  - free;
  - retain;
  - release; 
  - (int) references;  
  int references_;    
@end




