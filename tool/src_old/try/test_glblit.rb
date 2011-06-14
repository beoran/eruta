#!/usr/bin/env ruby -rubygems
require "sdl"
require "opengl"
require "mathn"
require "find"
require "time"

# Add GL and GLUT namespaces in to make porting easier
include Gl
include Glu
include Glut


class Texture
  def initialize(filename)
    aid     = SDL::Surface.load(filename)
    new_h   = new_w   = 1 
    new_w   = new_w * 2 while new_w < aid.w
    new_h   = new_h * 2 while new_h < aid.h
    # get next power of two size
    @surface = SDL::Surface.new(SDL::SWSURFACE, new_w, new_h, aid.format)    
    SDL::Surface.blit(aid, 0, 0, 0, 0, @surface, 0, 0)
    # Make new SW surface and blit the loaded surface on it.
    aid.destroy
    aid     = nil    
    # Don't need original surface now
    # surface = SDL::Surface.load(filename)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    # Texture parameters, and generate textures
    @textures = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, @textures[0])
    # Bind the texture, and load it from the pixel data
    if @surface.format.Amask == 0
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, @surface.w, @surface.h, 0, 
        GL_RGB, GL_UNSIGNED_BYTE, @surface.pixels.unpack('C*'))
    else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, @surface.w, @surface.h, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, @surface.pixels.unpack('C*'))
    end
    # Remember surface size
    @surface_w  = @surface.w
    @surface_h  = @surface.h
    puts "#{@surface_w} #{@surface_h}"
    # Keep the surface around, in case the user switches away from the 
    # window, and the textures need to be regenerated
    # May happen on windows, I read.
    # surface.destroy  # thanks to RubySDL, we can clean up immediately.
    # surface   = nil #hopefully this gets garbage collected    
    @texture    = @textures[0] # Store the Texture ID
    # Draw self to a drawig list
    draw_list
    @rotation   = 0     
  end
  
  # Draw self to cached list
  def draw_list()
    cw = @surface_w;
    ch = @surface_h;  
    l = t = 0;
    r = b = 1;
    hcw = (cw / 2.0);
    hch = (ch / 2.0);

    @list = glGenLists(1)
    glNewList(@list, GL_COMPILE);
      glBegin( GL_QUADS );
      # Top-left 
      glTexCoord2f(l, t);
      glVertex3f(0, 0, 0);
      # glVertex3f(-hcw, -hch, 0);
      # Top-right 
      glTexCoord2f(r, t);
      glVertex3f(cw, 0, 0);
      # glVertex3f(hcw, -hch, 0 );
      # Bottom-right 
      glTexCoord2f(r, b);
      glVertex3f(cw, ch, 0);
      # glVertex3f(hcw, hch, 0);
      # Bottom-left 
      glTexCoord2f(l, b);
      glVertex3f(0, ch, 0);
      # glVertex3f(-hcw, hch, 0);
      glEnd();
    glEndList();
  end

  def draw_at(x, y, z = 0)
    @rotation += 1.0
    glBindTexture(GL_TEXTURE_2D, @texture);
    glPushMatrix();
      glTranslatef(x, y, z);
      # glRotatef(@rotation, 0, 0, 1.0);
      glCallList(@list);
    glPopMatrix();  
  end  



  def draw_at_old(x, y, z = 0)
    glColor4f( 1.0, 1.0, 1.0, 1.0 ) ;
    glBindTexture( GL_TEXTURE_2D, @texture) ;
    glMatrixMode(GL_TEXTURE)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    xw = x + @surface_w
    yh = y + @surface_h
    
    glBegin(GL_QUADS) 
	glTexCoord2f( 1, 1) ; glVertex2i( xw, y  ) 
	glTexCoord2f( 1, 0) ; glVertex2i( xw, yh ) 
	glTexCoord2f( 0, 0) ; glVertex2i( x,  yh ) 
	glTexCoord2f( 0, 1) ; glVertex2i( x,  y  ) 
    glEnd() 
  end  

  def draw
    glBindTexture(GL_TEXTURE_2D, @texture)
    glMatrixMode(GL_TEXTURE)
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

    glLoadIdentity
    glRotatef(180,0,0,1) # reverse updside down texture
    #  glScalef(-1,1,1)
    glMatrixMode(GL_MODELVIEW)

    # Draw a rectangle
    glBegin(GL_QUADS)
    glTexCoord2f(0,0)
    glVertex3f(-1.0, -1.0, 0.0) # Bottom Left
    glTexCoord2f(1,0)
    glVertex3f( 1.0, -1.0, 0.0) # Bottom Right
    glTexCoord2f(1,1)
    glVertex3f( 1.0,  1.0, 0.0) # Top Right
    glTexCoord2f(0,1)
    glVertex3f(-1.0,  1.0, 0.0) # Top Left
    glEnd
  end
end

def init_gl_window(w = 640, h = 480)
    # Set up texture blending line in SDL
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);        
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    # glAlphaFunc(GL_GEQUAL,0.01);
    # Set viewport
    glViewport(0, 0, w, h);
    # Set to orthographic projection for 2D mode    
    glDisable( GL_DEPTH_TEST ) ;
    glMatrixMode( GL_PROJECTION ) ;
    glLoadIdentity() ;
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
end



SDL.init(SDL::INIT_VIDEO)
SDL.setGLAttr(SDL::GL_RED_SIZE  , 5 )
SDL.setGLAttr(SDL::GL_BLUE_SIZE , 5 )
SDL.setGLAttr(SDL::GL_GREEN_SIZE, 5 )
SDL.setGLAttr(SDL::GL_DEPTH_SIZE, 8 )
SDL.setGLAttr(SDL::GL_DOUBLEBUFFER, 0)
# SDL.setGLAttr(SDL::GL_SWAP_CONTROL, 0)
flags   = SDL::OPENGL # | SDL::HWSURFACE
flags  |= SDL::FULLSCREEN
SDL.setVideoMode(640, 480, 0, flags)
init_gl_window(640, 480)
puts GL_MAX_TEXTURE_SIZE

start   =  Time.now()
frames  =  0

texture =  Texture.new("test_tile.png") 

# Main Loop
loop do
  event = SDL::Event2.poll
  if !event.nil?
    break if event.is_a?(SDL::Event::Quit)    
    if event.is_a?(SDL::Event::KeyDown) # This means it's some kind of key press
      break if event.sym == SDL::Key::ESCAPE
    end
  end
  for x in (0...640).step(32) 
    for y in (0...640).step(32) 
      texture.draw_at_old(x, y)
    end
  end
  SDL.GLSwapBuffers
  frames += 1 
end
stop  = Time.now()
delta = stop - start
fps   = frames.to_f / delta.to_f
puts "#{frames} frames in #{delta} s: #{fps} fps"
