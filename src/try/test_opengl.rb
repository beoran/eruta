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

# Placeholder for the window object
window = ""

@objectsToDraw = Array.new

class PictureCollection
  attr_accessor :current_picture
  
  def initialize
    @pictures = Array.new
    @current_picture = -1
  end

  def select_next_picture
    @current_picture += 1 if (@current_picture + 1) != @pictures.size
  end

  def select_previous_picture
    @current_picture -= 1 if (@current_picture - 1) >= 0
  end

  def num_prepictures
    if @current_picture > -1
      @current_picture
    else
      0
    end
  end

  def num_postpictures
    @pictures.size - @current_picture - 1
  end

  def get_picture(pos)
    @pictures[pos] if pos > -1
  end

  def add_picture(pic)
    @pictures.push pic
    @current_picture = 0 if(@current_picture == -1)
  end

  def each
    @pictures.each do |pic|
      yield pic
    end
  end

  def length
    @pictures.length
  end
end

class Picture 
  def initialize(filename, number)
    aid     = SDL::Surface.load(filename)
    surface = SDL::Surface.new(SDL::SWSURFACE, 32, 32, aid.format)    
    SDL::Surface.blit(aid, 0, 0, 0, 0, surface, 0, 0)
    aid     = nil    
    # surface = SDL::Surface.load(filename)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    texture = glGenTextures(1)
    glBindTexture(GL_TEXTURE_2D, texture[0])
        if surface.format.Amask == 0
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface.w, surface.h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface.pixels.unpack('C*'))
        else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface.w, surface.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface.pixels.unpack('C*'))
        end
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)
    surface.destroy  # thanks to RubySDL, we can clean up immediately.
    surface    = nil #hopefully this gets garbage collected
    @texture    = texture[0] # Store the Texture ID
    draw_list
    @rotation   = 0     
  end
  
  # Draw self to cached list
  def draw_list()
    cw = 32;
    ch = 32;  
    l = t = 0;
    r = b = 1;
    hcw = (cw / 2.0);
    hch = (ch / 2.0);

    lists = glGenLists(1)
    @list = lists
    glNewList(lists, GL_COMPILE);
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
      glTranslatef(x, y, 0);
      # glRotatef(@rotation, 0, 0, 1.0);
      glCallList(@list);
    glPopMatrix();  
  end  



  def draw_at_old(x, y, z = 0)
  glBindTexture( GL_TEXTURE_2D, @texture );
	glPushMatrix();

	cw = 32;
	ch = 32;	
	l = t = 0;
	r = b = 1;
	hcw = (cw / 2.0);
	hch = (ch / 2.0);
	glTranslatef(x, y, 0);
	# glRotatef(rotation, 0, 0, 1.0f);

	glBegin( GL_QUADS );

	# Top-left 
	glTexCoord2f(l, t);
	glVertex3f(-hcw, -hch, 0);
	# Top-right 
	glTexCoord2f(r, t);
	glVertex3f(hcw, -hch, 0 );
	# Bottom-right 
	glTexCoord2f(r, b);
	glVertex3f(hcw, hch, 0);
	# Bottom-left 
	glTexCoord2f(l, b);
	glVertex3f(-hcw, hch, 0);
	
	glEnd();

	glPopMatrix();	
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
    glEnable(GL_TEXTURE_2D);
    # glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, w, h);
    # glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);        
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    # Do we need these above, and this one below? 
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL,0.01);
    
    glEnable(GL_TEXTURE_2D);
    # glEnable(GL_SCISSOR_TEST)
    
    glDisable(GL_DEPTH_TEST);
    # glTranslatef(0.375, 0.375, 0) # prevent overlaps between pixels that may cause antialiasing.
    # glLoadIdentity();
end

#draw_gl_scene = Proc.new do
def draw_gl_scene
    @objectsToDraw.get_picture(0).draw_at(0, 0)
    @objectsToDraw.get_picture(@objectsToDraw.current_picture).draw_at(0, 0)
    pic =  @objectsToDraw.get_picture(@objectsToDraw.current_picture)
    (0..30).step(10) do |z|
      (0...640).step(32) do |x|
        (0...480).step(32) do |y|
          pic.draw_at(x, y, z)
        end
      end
    end
    # Swap buffers for display 
    SDL.GLSwapBuffers
end


SDL.init(SDL::INIT_VIDEO)
SDL.setGLAttr(SDL::GL_RED_SIZE, 5 )
SDL.setGLAttr(SDL::GL_BLUE_SIZE, 5 )
SDL.setGLAttr(SDL::GL_GREEN_SIZE, 5 )
SDL.setGLAttr(SDL::GL_DEPTH_SIZE, 8 )
SDL.setGLAttr(SDL::GL_DOUBLEBUFFER,1)
# SDL::FULLSCREEN| SDL::FULLSCREEN|
SDL.setVideoMode(640, 480, 0, SDL::OPENGL|SDL::HWSURFACE)

init_gl_window(640, 480)

puts GL_MAX_TEXTURE_SIZE

@manytextures = glGenTextures(1000)

@objectsToDraw = PictureCollection.new
pos = 0
dirname = "/home/bjorn/eruta/share/image/tile"
d = Dir.new(dirname)
d.each do |path|
  if File.extname(path) == ".png"
    @objectsToDraw.add_picture Picture.new(dirname + '/' + path, pos)
    pos += 1
  end
end
puts "Images loaded!"
start   =   Time.now()
frames  =   0

# Main Loop
loop do
  event = SDL::Event2.poll
  if !event.nil?
    break if event.is_a?(SDL::Event2::Quit)
    if event.is_a?(SDL::Event2::KeyDown) # This means it's some kind of key press
      @objectsToDraw.select_next_picture if event.sym == SDL::Key::RIGHT
      @objectsToDraw.select_previous_picture if event.sym == SDL::Key::LEFT
      break if event.sym == SDL::Key::ESCAPE
    end
  end
  draw_gl_scene
  frames += 1 
end
stop  = Time.now()
delta = stop - start
fps   = frames.to_f / delta.to_f
puts "#{frames} frames in #{delta} s: #{fps} fps"
