require 'mkmf'

dir_config('gari')

CFLAGS='-O3'

unless find_header("png.h", "/usr/include/SDL", "/usr/local/include/SDL")
  raise "PNG includes not found!" 
end

unless find_header("jpeglib.h", "/usr/include/SDL", "/usr/local/include/SDL")
  raise "JPEG includes not found!" 
end

unless find_header("SDL.h", "/usr/include/SDL", "/usr/local/include/SDL")
  raise "SDL includes not found!" 
end

unless find_library("SDL", "SDL_Init", "/usr/lib", "/usr/local/lib")
  raise "SDL library not found!" 
end

unless find_library("SDL_mixer", "Mix_Resume", "/usr/lib", "/usr/local/lib")
  raise "SDL_mixer library not found!" 
end

unless find_library("SDL_image", "IMG_Load", "/usr/lib", "/usr/local/lib")
  raise "SDL_image library not found!" 
end

unless find_library("SDL_ttf", "TTF_Init", "/usr/lib", "/usr/local/lib")
  raise "SDL_ttf library not found!" 
end

unless find_library("png", "png_create_write_struct", "/usr/lib", "/usr/local/lib")
  raise "PNG library libpng not found!" 
end

unless find_library("jpeg", "jpeg_stdio_src", "/usr/lib", "/usr/local/lib")
  raise "JPEG library libjpeg not found!" 
end


create_makefile("gari")

