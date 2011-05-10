

class Rogaru::Tilemap::Layer
  attr_reader   :tilewide
  attr_reader   :tilehigh
  attr_reader   :layer
  attr_accessor :tileset
    
  def initialize(tileset, wide, high) 
    @tileset    = tileset 
    @layer      = []
    @layer_save = []
    @high       = high
    @wide       = wide
    for index in (0...@high)
      @layer[index] = []
      @layer_save[index] = []
      for jdex in (0...@wide)
        @layer[index].push(nil)
        @layer_save[index].push(-1)
      end  
    end
    @tilehigh   = @tileset.tilehigh
    @tilewide   = @tileset.tilewide
    @realhigh   = @tilehigh * @high
    @realwide   = @tilewide * @wide
    @x          = 0
    @y          = 0
    @vx         = 0
    @vy         = 0
  end
  
  def set_tiles(tilearray) 
    @tiles = tilearray
  end

  def add_tile(tile) 
    raise "Obsolete"
  end

  # Sets the tile in this layer to the tile with this tile index
  def set(x, y, tile_id)
    if (x < 0) or (y<0) or ( x >= @wide) or (y >= @high)
      raise "Index #{x} #{y} #{tile_id} not valid." 
    end  
    real_tile_id    = -1
    real_tile       = nil
    unless tile_id.nil? or tile_id < 0 
      real_tile       = @tileset.get_tile(tile_id)
      real_tile_id    = tile_id
      unless real_tile
        warn "No real tile found for #{x} #{y} #{tile_id}" 
        real_tile_id    = -1
        real_tile       = nil
      end
    end
    @layer[y][x]      = real_tile 
    @layer_save[y][x] = real_tile_id
  end
  
  # Gets the tile index of the tile at the given x,y coordinates,
  # or -1 if out of bounds 
  def get(x, y) 
    gx = x; gy = y
    return -1 if gy > @high or gx > @wide or gy < 0 or gx < 0  
    return @layer_save[gy][gx]
  end
  
  # Returns the actual tile at the given x and y coordinates, 
  # or nil if the coordinates are out of bounds
  def get_tile(x, y) 
    gx = x; gy = y
    return nil if gy > @high or gx > @wide or gy < 0 or gx < 0  
    return @layer[gy][gx]
  end
  
  # Iterate over all tiles in this layer, column by column. 
  # Yields Tile objects, and the world x and y coordinate of this tile 
  # in the layer.
  def each_tile()
    for y in (0...@high)
      for x in (0...@wide)          
          wx, wy = tile_to_world(x, y)
          yield @layer[y][x], wx, wy
      end
    end
  end

  def draw(screen, x, y)
    txstart     = ( x / @tilewide )
    tystart     = ( y / @tilehigh )
    xtilestop   = (screen.w / @tilewide) + 1
    ytilestop   = (screen.h / @tilehigh) + 1
    txstop      = xtilestop + txstart
    tystop      = ytilestop + tystart
    drawx       = 0
    drawy       = 0
    row         = nil
    aidimage    = nil
    nilcount    = 0
    return if (txstart >= @wide or tystart >= @high) 
    txstart = 0    if (txstart < 0) 
    tystart = 0    if (tystart < 0)
    txstop  = @wide if (txstop > @wide) 
    tystop  = @high if (tystop > @high) 
    drawy   = -y + ( (tystart-1) * @tilehigh )
    tydex   = tystart
    # for tydex in (tystart...tystop)
    while tydex < tystop do 
      drawy   += @tilehigh;
      drawx   = -x + ( (txstart-1) * @tilewide )
      row     = @layer[tydex]
      # for txdex in (txstart...txstop) do
      txdex = txstart
      while txdex < txstop
        drawx   += @tilewide        
        aidtile  = row[txdex]
        unless aidtile.nil? then
          aidimage    = aidtile.get
          screen.blit(drawx, drawy, aidimage)
        end
        txdex += 1
      end
      tydex  += 1
    end
  end

  def draw_effects(screen, x, y)
    txstart     = ( x / @tilewide )
    tystart     = ( y / @tilehigh )
    xtilestop   = (screen.w / @tilewide) + 1
    ytilestop   = (screen.h / @tilehigh) + 1
    txstop      = xtilestop + txstart
    tystop      = ytilestop + tystart
    drawx       = 0
    drawy       = 0
    row         = nil
    aidimage    = nil
    nilcount    = 0
    return if (txstart >= @wide or tystart >= @high) 
    txstart = 0    if (txstart < 0) 
    tystart = 0    if (tystart < 0)
    txstop  = @wide if (txstop > @wide) 
    tystop  = @high if (tystop > @high) 
    drawy   = -y + ( (tystart-1) * @tilehigh )
    c1      = screen.format.map_rgb(10,10,10)
    c2      = screen.format.map_rgb(255,0,0)
    c3      = screen.format.map_rgb(0,255,255)
    c4      = screen.format.map_rgb(255,255,0)
    c5      = screen.format.map_rgb(255,255,255)
    c6      = screen.format.map_rgb(255,255,255)
    
    for tydex in (tystart...tystop) do 
      drawy   += @tilehigh;
      drawx   = -x + ( (txstart-1) * @tilewide )
      row     = @layer[tydex]
      for txdex in (txstart...txstop) do
        drawx   += @tilewide        
        aidtile  = row[txdex]
        unless aidtile.nil? then
          screen.draw_rect(drawx   , drawy    , 2, 2, c1, true) unless aidtile.walk
          screen.draw_rect(drawx   , drawy + 2, 2, 2, c2, true) if aidtile.swim
          screen.draw_rect(drawx   , drawy + 4, 2, 2, c3, true) if aidtile.pain
          screen.draw_rect(drawx+8 , drawy    , 2, 2, c4, true) unless aidtile.jump
          screen.draw_rect(drawx+12, drawy    , 2, 2, c5, true) if aidtile.ledge
          screen.draw_rect(drawx+16, drawy    , 2, 2, c6) if aidtile.up
          screen.draw_rect(drawx+16, drawy+16 , 2, 2, c6) if aidtile.down
        end
      end
    end
  end

  def w
    return @realwide
  end

  def h
    return @realhigh
  end
  
  def tw
    return @wide
  end

  def th
    return @high
  end

  def to_xml() 
    layer = REXML::Element.new 'layer'
    sizehash = {'wide' => @wide.to_s, 
          'high' => @high.to_s,
          'tilewide' => @tilewide.to_s, 
          'tilehigh' => @tilehigh.to_s }
          
    layer.add_attributes(sizehash)
    rows = layer.add_element 'rows'
    for y in (0...@high) do
      row = rows.add_element('row')
      row.add_attribute('y', y.to_s) 
      row.text = @layer_save[y].join(' ');      
    end
    return layer
  end
  
  def self.new_from_xml(xml_layer, tileset)
    mw    = xml_layer.attributes['wide'].to_i
    mh    = xml_layer.attributes['high'].to_i
    tw    = xml_layer.attributes['tilewide'].to_i
    th     = xml_layer.attributes['tilehigh'].to_i
    layer = Tilemap::Layer.new(tileset, mw, mh)
    xml_layer.elements.each('rows/row') do | xml_row |
      y   = xml_row.attributes['y'].to_i
      row = xml_row.text.tr("\n\t\r,;", "     ").strip.split(" ").map { |v| v.to_i  }
      # parse the row, translate all seps to spaces, strip the edges, and split on space
      # and finally transfor into an array of integers.
      stop = row.size
      for x in (0...stop) do
        layer.set(x, y, row[x])
      end
    end
    return layer
  end
    
  # Loads a layer from a tmx file (www.mapeditor.org)
  def self.new_from_tmx(tmx, tileset)
    mw    = tmx.attributes['width'].to_i
    mh    = tmx.attributes['height'].to_i
    layer = Tilemap::Layer.new(tileset, mw, mh)
    y     = 0
    x     = 0
    tmx.elements.each('data/tile') do | tmx_tile |
      unless x < mw
        x   = 0
        y  += 1
      end
      index = tmx_tile.attributes['gid'].to_i 
      index = -1 if index < 1 
      layer.set(x, y, index)
      x    += 1
    end
    return layer
  end

  def screen_to_tile_x(top_x, screen_x)
    return ((top_x + screen_x) / @tilewide).floor
  end 
  
  def screen_to_tile_y(top_y, screen_y)
    return ((top_y + screen_y) / @tilehigh).floor
  end 

  def tile_to_screen_x(top_x, tile_x)
    return   (tile_x * @tilewide) - top_x
  end 
  
  def tile_to_screen_y(top_y, tile_y)
    return   (tile_y * @tilehigh) - top_y
  end 

  def tile_to_world(tile_x, tile_y)
    return tile_x * @tilewide, tile_y * @tilehigh
  end

end

