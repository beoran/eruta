
class Tilemap::Entity

  def initialize(x, y, image)
    @x = x
    @y = y
    @image = image  
    @world_x = x
    @world_y = y
    @vx = 0
    @vy = 0
  end
  
  def draw(screen, map_x, map_y)
    #@image.blit(screen, [@world_x - map_x, @world_y - map_y])
  end
    
  def update
    @world_x += @vx
    @world_y += @vy
  end
  
  attr_accessor :vx, :vy

end
