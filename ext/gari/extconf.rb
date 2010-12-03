require 'mkmf'

dir_config('gari')

CFLAGS='-O3'

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


create_makefile("gari")

