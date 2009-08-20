require 'rubygems'
require 'ffi'

# if false 
if true 

module FFI
  class AutoPointer < FFI::Pointer    
    # Call with pointer to wrap, class or module that contains the release 
    # function, and the name of the function (:release by default)
    def self.new(pointer, library, function = :release )
       install_finalizer(self, pointer, library, function.to_sym)
       return pointer
    end
    
    
    private
    def self.make_finalizer(ptr, library, function)
        proc { | id | library.send(function, ptr) }
    end
  
    def self.install_finalizer(obj, ptr, library, function)
      finalizer  = make_finalizer(ptr, library, function) 
      ObjectSpace.define_finalizer(obj, finalizer) 
      return obj
    end

  end

  class ManagedStruct <  FFI::Struct
    def initialize(pointer=nil)
      raise NoMethodError, "release() not implemented for class #{self}" unless self.class.respond_to? :release
      raise ArgumentError, "Must supply a pointer to memory for the Struct" unless pointer
      super(FFI::AutoPointer.new(pointer, self.class, :release))
    end 
  end

end

end

module SDL 
  extend FFI::Library
  ffi_lib('SDL')   # load SDL library through ffi  

  # Define some constants for the init function
  INIT_EVERYTHING   = 0x0000FFFF
  # And some for video functions
  SWSURFACE         = 0x00000000  
  HWSURFACE         = 0x00000001  
  ASYNCBLIT         = 0x00000004  
  ANYFORMAT         = 0x10000000  
  HWPALETTE         = 0x20000000  
  DOUBLEBUF         = 0x40000000  
  FULLSCREEN        = 0x80000000  
  OPENGL            = 0x00000002  
  OPENGLBLIT        = 0x0000000A  
  RESIZABLE         = 0x00000010  
  NOFRAME           = 0x00000020
  class SDL_Surface < FFI::ManagedStruct   
    layout :flags   => :ulong   ,
           :format  => :pointer ,
           :w       => :int     ,
           :h       => :int     ,
           :pitch   => :short   ,
           :pixels  => :pointer
    
    # the rest of the C struct is mostly private
    
    def self.release(ptr)
        puts "Release SDL_FreeSurface called!"
        SDL.SDL_FreeSurface(ptr)
    end
    
  end

  
  attach_function :SDL_Init, [:ulong], :int
  attach_function :SDL_Quit, [], :int
  attach_function :SDL_SetVideoMode, [:int, :int, :int, :ulong ], :pointer
  attach_function :SDL_CreateRGBSurface, [:ulong, :int, :int, :int, :ulong, :ulong, :ulong, :ulong], :pointer
  attach_function :SDL_FreeSurface, [:pointer], :void

end


def peek_at_proc
  puts `ps u -p #{Process.pid}`
end


if $0 == __FILE__
  SDL.SDL_Init(SDL::INIT_EVERYTHING)
  puts "Try to leak SDL_Surface"
  peek_at_proc
      1000.times do
        begin
        surface = SDL::SDL_Surface.new(
                  SDL.SDL_CreateRGBSurface(0, 32, 32, 32, 0x000000FF,  0x0000FF00,  0x00FF0000,  0xFF000000)
                  )
        surface = nil
        GC.start # force collection
        end
      end
  peek_at_proc
  SDL.SDL_Quit()
  peek_at_proc
end

