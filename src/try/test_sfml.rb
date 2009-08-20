# Include the RubySFML extension
require "RubySFML"
include SFML
require "time"

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
	  # @img     = Image.new(filename)
      @@int    ||= 50
      @@int    += 5
      @img     = Image.new(40, 40, Color.new(0, @@int, 0));
      @sprite  = Sprite.new(@img) 
      @sprite.left = 40
      @sprite.top  = 40
	end

    def draw_at(screen, xx, yy, zz = 0)
      @sprite.left = xx
      @sprite.top  = yy

      @sprite.x = xx
      @sprite.y = yy
      screen.draw(@sprite)
    end  

end

#draw_gl_scene = Proc.new do
def draw_sfml_scene(screen)
    

    @objectsToDraw.get_picture(0).draw_at(@screen, 0, 0)
    @objectsToDraw.get_picture(@objectsToDraw.current_picture).draw_at(@screen, 0, 0)
    pic =  @objectsToDraw.get_picture(@objectsToDraw.current_picture)
    for z in (0..30).step(10) do 
      for x in (0..640).step(40) do 
        for y in (0..480).step(40) do
          pic.draw_at(screen, x, y, z)
        end
      end
    end
    # Swap buffers for display 
	# SDL.GLSwapBuffers
    # screen.flip()
    screen.display
end

mode    = VideoMode.new(640, 480, 32)
@screen = RenderWindow.new(mode, "RubySFML Test", Window::Fullscreen)
view    = View.new(FloatRect.new(0, 0, 640, 480))
@screen.showMouseCursor(true)
@screen.useVerticalSync(false)

@objectsToDraw = PictureCollection.new
pos = 0
dirname = "/home/bjmey/eruta/share/image/tile"
d = Dir.new(dirname)
d.each do |path|
	if File.extname(path) == ".png"
		@objectsToDraw.add_picture Picture.new(dirname + '/' + path, pos)
		pos += 1
	end
end

start   =   Time.now()
frames  =   0
done = false
# Main Loop
while not done do 
    while e = @screen.getEvent() do
    	done = true if e.type == Event::Closed or (e.type == Event::KeyReleased and e.code == Key::Escape)
		@objectsToDraw.select_next_picture    if e.type == Event::KeyReleased and e.code  == Key::Right
		@objectsToDraw.select_previous_picture if e.type == Event::KeyReleased and e.code == Key::Left       
	end
	draw_sfml_scene(@screen)
    frames += 1
    puts frames 
end
stop  = Time.now()
delta = stop - start
fps   = frames.to_f / delta.to_f
puts "#{frames} frames in #{delta} s: #{fps} fps"
