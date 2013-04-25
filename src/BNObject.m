#import <bn/BNObject.h>

#define BNOBJECT_EXTRABYTES 0 

@implementation BNObject

  // Allocates an instance of the object.
  + alloc {
    return class_createInstance(self, BNOBJECT_EXTRABYTES);
  }
  
  // Returns a dumb copy of self 
  - copy {
    return object_copy(self, BNOBJECT_EXTRABYTES);
  }
  
  // Constructor. Will be called from - init. Makes it easier to 
  // implement a custom cleanup function
  - setup {
    return self;
  }
    
  // Destructor. Will be called from - free. Makes it easier to 
  // implement a custom cleanup function
  - done {
    return self;
  }
  
  /* Returns the name of the class of the object as a C string*/
  - (const char *) name_as_cstring {
    return object_getClassName(self);
  }

  // Dispose of the given object 
  + free : (id) object {
    object_dispose(object);
    return nil;
  }
  
  // Initializes this object. You must call [super init] yourself on all objects. 
  // that override init.  - setup will be called automatically 
  - init {
    if (self) {
      references_ = 1;
      [self setup];
    }
    return self;
  }
  
  
  // Disposes of an object. Calls done if self is not nil. Returns nil.
  - free {
    if(self) {
      [self done];
    }
    object_dispose(self);
    return nil;
  }
  
  // Increases the reference count of this object and returns self.
  - retain {
    references_++;
    return self;
  }
  
  // Increases the reference count of this object. If the object's refcound becomes 
  // lower than 1, calls - free and returns nil. Othwerwise returns self. 
  - release {
    references_--;
    if (references_ < 1) {
      return [self free];
    }
    return self;
  }
  
  // Returns the reference count of this object
  - (int) references {
    return references_;
  }
  
@end




