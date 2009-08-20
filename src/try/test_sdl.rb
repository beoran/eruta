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
	  aid_surface = SDL::Surface.load(filename)
      if aid_surface.format.Amask == 0 
        @surface    = aid_surface.display_format
      else
        @surface    = aid_surface.display_format_alpha
      end
	end

    def draw_at(screen, xx, yy, zz = 0)
      SDL::Surface.blit(@surface, 0, 0, 0, 0, screen, xx, yy)
    end  

end

def init_gl_window(width = 640, height = 480)
    # Enable Textures
	glEnable(GL_TEXTURE_2D)
    tw = 640.0 / 40.0
    th = 480.0 / 40.0
    # glRotatef(180,0,0,1)

    # glViewport(0,0,width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,width,0,height,-100,100);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glAlphaFunc(GL_GREATER, 0.1);
    glEnable(GL_ALPHA_TEST);
    # glEnable(GL_FOG);
    # glFog(GL_FOG_DENSITY, 0.5  );

    # gluOrtho2D(0.0, width, 0.0, height)
        

=begin
	glViewport(0,0, width, height)
    
    
	# Background color to black
	glClearColor(0.0, 0.0, 0.0, 0)
    
	# Enables clearing of depth buffer
	glClearDepth(1.0)
	# Set type of depth test
    
	glDepthFunc(GL_LEQUAL)
	# Enable depth testing
	glEnable(GL_DEPTH_TEST)
	# Enable smooth color shading
	# glShadeModel(GL_SMOOTH)
	
	# glMatrixMode(GL_PROJECTION)
	glLoadIdentity
	# Calculate aspect ratio of the window
	# gluPerspective(60.0, width / height, 0.1, 100.0)
	
    gluLookAt(10.0, 8.0, 40.0, 10.0, 8.0, 0.0, 0.0, 1.0, 0.0);

	# glMatrixMode(GL_MODELVIEW)
    # glTranslate(0,0,10)
=end
end

#draw_gl_scene = Proc.new do
def draw_sdl_scene(screen)
=begin

	# Clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	
	# Reset the view
	glMatrixMode(GL_MODELVIEW)
	glLoadIdentity

	# Draw main picture
	glTranslatef(0,0,-4)
	@objectsToDraw.get_picture(@objectsToDraw.current_picture).draw

	glLoadIdentity

	# Draw pre pictures
	if(@objectsToDraw.num_prepictures > 0)
		pos = @objectsToDraw.current_picture - 1
		glTranslatef(-3.0,0,-6.0) # Starting position
		glRotatef(45.0,0,1,0)
		while pos >= 0
			@objectsToDraw.get_picture(pos).draw
			pos -= 1
			glTranslatef(-0.3,0,-1);
		end	
	end

	glLoadIdentity

	# Draw post pictures
	if(@objectsToDraw.num_postpictures > 0)
		pos = @objectsToDraw.current_picture + 1
		glTranslatef(3.0,0,-6.0) # Starting position
		glRotatef(-45.0,0,1,0)
		while pos < @objectsToDraw.length
			@objectsToDraw.get_picture(pos).draw
			pos += 1
			glTranslatef(0.3,0,-1);
		end	
	end
=end
    @objectsToDraw.get_picture(0).draw_at(@screen, 0, 0)
    @objectsToDraw.get_picture(@objectsToDraw.current_picture).draw_at(@screen, 0, 0)
    pic =  @objectsToDraw.get_picture(@objectsToDraw.current_picture)
    (0..30).step(10) do |z|
      (0..640).step(40) do |x|
        (0..480).step(40) do |y|
          pic.draw_at(screen, x, y, z)
        end
      end
    end
    # Swap buffers for display 
	# SDL.GLSwapBuffers
    screen.flip()
end


SDL.init(SDL::INIT_VIDEO)
# SDL.setGLAttr(SDL::GL_DOUBLEBUFFER,1)
# SDL::FULLSCREEN|
@screen = SDL.setVideoMode(640, 480, 0, SDL::FULLSCREEN|SDL::HWSURFACE | SDL::DOUBLEBUF|SDL::ANYFORMAT)

# init_gl_window(640, 480)

# puts GL_MAX_TEXTURE_SIZE

# @manytextures = glGenTextures(10000)


start   =   Time.now()
frames  =   0


sur1    = SDL::Surface.load('../../share/image/tile/tile_grass32.png')
sur2    = SDL::Surface.load('../../share/image/tile/tile_ugly_cloud_al32.png')
tex1    = sur1.display_format
tex2    = sur2.display_format_alpha

    done    = false
    levels  = [tex1, tex2, tex2, tex2]
    start   = Time.now()
    frames  = 0
  
    until done do
      event = SDL::Event2.poll
        if !event.nil?
          done = true if event.is_a?(SDL::Event2::Quit)
          if event.is_a?(SDL::Event2::KeyDown) # This means it's some kind of key press
            # if event.sym == SDL::Key::RIGHT
            # if event.sym == SDL::Key::LEFT
            done = true if event.sym == SDL::Key::ESCAPE
          end
        end
      (0..3).step(1) do |z|
        (0..640).step(32) do |x|
          (0..480).step(32) do |y|
            aid = levels[z]
            # .transform(levels[z].format.map_rgba(0,0,0,SDL::ALPHA_TRANSPARENT), frames,1,1,0)
            SDL::Surface.blit(aid, 0,0,0,0, @screen, x, y)
          end
        end
      end
      @screen.flip
      frames += 1
    end


stop  = Time.now()
delta = stop - start
fps   = frames.to_f / delta.to_f
puts "#{frames} frames in #{delta} s: #{fps} fps"

SDL.quit()
