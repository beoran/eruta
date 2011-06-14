package main

import (
  "sdl"
/*  
  "sdl/ttf"
  "sdl/mixer"
  "math" 
  "fmt"
*/  
)

// #include <SDL/SDL.h>
// #include <SDL/SDL_image.h>

import "C"
import "unsafe"

type cast unsafe.Pointer

func (*sdl.Surface surface) DisplayFormatAlpha() *sdl.Surface {
  aid = C.SDL_DisplayFormatAlpha(surface)
  return (*Surface)(cast(aid))
}  

type Surface * sdl.Surface
 
type Screen struct {
  * sdl.Surface
} 


func AllInit() {
  sdl.Init(sdl.INIT_EVERYTHING)  
} 

func ScreenMake(wide int, high int, fullscreen bool) * sdl.Surface {
  bpp   := 32  
  var flags uint32 = sdl.HWSURFACE | sdl.ANYFORMAT
  /* if doublebuf { flags |= sdl.DOUBLEBUF } */
  if fullscreen { flags |= sdl.FULLSCREEN }
  vs    := sdl.SetVideoMode(wide, high, bpp, flags)
  return vs
}  

func Done() {
  sdl.Quit()
}


func ImageLoad(filename string) *sdl.Surface {
  return sdl.Load(filename)  
}

func ImageLoadSolid(filename string) *sdl.Surface {
  image = ImageLoad(filename)
  image.ToDisplayAlpha()
}

func ImageLoadAlpha(filename string) *sdl.Surface {  
  image = ImageLoadSolid(filename)
  image.ToDisplayAlpha()
 
        surface = SDL::Surface.load(filename)
        return surface
      end 
      
      # Loads an solid image, prepares it for display.
      def load_solid(filename)
        image = load_filename(filename)
        return image.to_display()
      end 
      
      # Loads an image with a colorkey, prepares it for display.
      # By default, the colorkey is bright pink (#FF00FF), like in the Allegro game library
      def load_colorkey(filename, colorkey = Sisa::SurfaceMixin::DEFAULT_COLORKEY)
        image = load_filename(filename)
        image.set_colorkey(colorkey)
        return image.to_display_alpha()
      end 
      
      # Loads an image with alpha, prepares it for display.
      def load_alpha(filename)
        surface = load_filename(filename)
        return surface.to_display_alpha()
      end


func main() {
  AllInit()
  defer Done()
  screen := ScreenMake(640, 480, false)
  screen.Flip()
  println("Ok!")
}
