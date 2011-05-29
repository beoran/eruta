require "raku"

class Rogaru::Tilemap::Map
  
  attr_accessor   :camera
  attr_accessor   :tileset
  
  # The map will have a background. Not rendered yet.
  attr_accessor   :background
  
  def initialize(tileset = nil, layers = [], backname = nil)
    @layers       = layers    
    @tileset      = tileset
    @layer_count  = layers.size
    load_background(backname)
    # load_background(nil)   # Don't load the background for now... 
    # These tiles will be shared between the map and all it's layers. 
    # Tiles with negative index will be skipped.  
    @moving_tiles = [] 
    @x            = 0
    @y            = 0
    @vx           = 0
    @vy           = 0
    @maxwide      = 0
    @maxhigh      = 0
    @maxtilehigh  = 0
    @maxtilewide  = 0
    @entities     = []
    @camera       = nil
  end
  
  # Loads the background image
  def load_background(backname)
    @background   = nil         # Background.
    @backname     = backname    # Background Name.
    if @backname
      printf('Loading map background...')
      imgname = File.join('..', 'share', 'image', 'background', @backname)
      @background = Sisa::Surface.load_solid(imgname) rescue nil
      puts "Failed to load map background #{imgname}." unless @background
      # Don't care too much if it fails, just don't render then
      puts('OK!')
    end
  end
  
  # Draws the background if any
  def draw_background(screen)
    @background.blit(screen, 0, 0) if @background
  end
  
  
  attr_accessor :entities
  
  def update(screen_wide, screen_high)
    if @camera
      @camera.update 
      @x = @camera.x
      @y = @camera.y
    else 
      @x += @vx
      @y += @vy
    end
  end
  
  def advance
    @moving_tiles.each { | tile | tile.advance }
  end

  def add_tileset_tile(tile) 
    raise "Obsolete"
  end
  
  def get_tileset_tile(index) 
    return @tileset[index]
  end

  def add_layer(layer)
    layer.tileset = @tileset
    @layers.push layer
    @layer_count += 1 
    if @maxwide < layer.w then
      @maxwide     = layer.w 
      @maxtilewide   = layer.tw
    end
    if @maxhigh < layer.h then
      @maxhigh     = layer.h 
      @maxtilehigh   = layer.th
    end
  end
  
  # Adds a new layer with the given size to this map 
  def new_layer(wide, high)
    layer = Tilemap::Layer.new(@tileset, wide, high)
    self.add_layer(layer)
  end
  
  def draw_parallax_auto(screen)
    self.draw_background(screen)
    self.draw_parallax(screen, @x, @y)
  end
  
  def draw_auto(screen)
    self.draw_background(screen)
    self.draw(screen, @x, @y)
  end
  
  def draw_with_sprites(screen, sprites)
    self.draw_background(screen)
    index = 0
    @layers.each do |layer|  
      layer.draw(screen, @x, @y)
      sprites.draw_layer(screen, index)
      index += 1
    end
  end
  
  attr_accessor :vx
  attr_accessor :vy
  attr_reader :x
  attr_reader :y

  def tw()
    return @maxtilewide
  end

  def th()
    return @maxtilehigh
  end
  
  def draw_parallax(screen, x, y)
    top = @layers.last
    top_w = top.w
    top_h = top.h
    @layers.each do
      |layer|  
      x_scale = (layer.w - screen.w).to_f  / top_w
      y_scale = (layer.h - screen.h).to_f  / top_h
      x_draw  = (x * x_scale).to_i
      y_draw  = (y * y_scale).to_i
      layer.draw(screen, x_draw, y_draw)
    end
  end
  
  def draw(screen, x, y)
    index = 0
    @layers.each do
      |layer|  
      layer.draw(screen, x, y)
      if index == 0 then
        @entities.each { |entity| entity.draw(screen, x, y ) }
        index += 1
      end
    end
  end


  def draw_with_effects(screen, x, y)
    @layers.each do
      |layer|  
      layer.draw(screen, x, y)
      layer.draw_effects(screen, x, y)
    end
  end
  
  def draw_auto_with_effects(screen)
    self.draw_background(screen)
    draw_with_effects(screen, @x, @y)
  end
  
  # Saves this map to a rexml document
  def to_xml()
    map     = REXML::Element.new 'map'
    if @backname
      bg      = map.add_element 'background'
      bg.text = @backname  
    end    
    tileset = map.add_element(@tileset.to_xml)
    layers  = map.add_element 'layers'
    i       = 0
    # for i in (0...@layers.size)
    while i < @layers.size      
      layer = @layers[i].to_xml()
      layer.add_attribute('z', i.to_s)
      layers.add_element(layer)
      i += 1
    end
    return map
  end
  
  def to_raku
    res = StringIO.new('')
    res.puts('map {')
    res.puts("  backgound '#{@backname}'") if @backname
    res << @tileset.to_raku
    res.puts('layers {')
    for i in (0...@layers.size)
      res << @layers[i].to_raku(i) 
    end
    res.puts('}')
    res.puts('}') 
    return res.string
  end  
  
  
  # Scopes out the animated tilmes and prepares them to be animated by adding them to moving_tiles
  def prepare_animated_tiles(tileset)
    for tile in tileset
      @moving_tiles << tile if tile.animated? 
    end
  end
  
  # Load this tile map from an REXML element/document
  def self.new_from_xml(xml)
    layers  = []    
    tileset = Tilemap::Tileset.new_from_xml(xml.elements['tileset'])
    if xml.elements['background']
        backname = xml.elements['background'].text.strip
    else 
        backname = nil    
    end
    xml.elements.each('layers/layer') do | xml_layer |
      layer = Tilemap::Layer.new_from_xml(xml_layer, tileset)
      layers << layer
    end
    map = Tilemap::Map.new(tileset, layers, backname)
    # Load tileset and prepare for animation
    map.prepare_animated_tiles(tileset)    
    return map
  end
  
  # Loads this tile map from a tmx file (www.mapeditor.org)
  def self.new_from_tmx(tmx)
    layers  = []    
    tileset = Tilemap::Tileset.new_from_tmx(tmx)
    backname = nil
    p tmx
    tmx.elements.each('properties/property') do |tmx_prop|
      p "property"
      p tmx_prop
      if tmx_prop.attributes['name'] == 'background'        
        backname = tmx_prop.attributes['value'].strip
        p backname
      end
    end
    tmx.elements.each('layer') do | tmx_layer |
      layer = Tilemap::Layer.new_from_tmx(tmx_layer, tileset)
      layers << layer
    end  
    map = Tilemap::Map.new(tileset, layers, backname)
    return map
  end
  
  # Loads the map from a Raku file
  def self.new_from_raku(text)
    puts "Loading map..." 
    raku, err  = Raku.parse_to_node(text)
    if err
      warn err.fail_message
      return nil 
    end
    puts "Loaded!" 
    layers    = []
    tileset   = Tilemap::Tileset.new_from_raku(raku)
    bg        = raku.find_first(:map, :background)
    backname  = bg ? bg.data[0] : nil
    rlayers   = raku.find_all(:map, :layers, :layer)
    if rlayers 
      for rlayer in rlayers
        layer = Tilemap::Layer.new_from_raku(rlayer, tileset)
        layers << layer
      end
    else 
      warn "Map has no layers!"
    end   
    map = Tilemap::Map.new(tileset, layers, backname)
    map.prepare_animated_tiles(tileset)
    return map
  end
  

  # Loads the map from the given file name
  def self.load_from(filename)
    file = File.new(filename)
    return nil unless file    
    if filename =~ /\.tmx\Z/
      doc = REXML::Document.new(file)       
      result = self.new_from_tmx(doc.root)
    elsif filename =~ /\.ra\Z/
      text   = file.read
      result = self.new_from_raku(text)
    else
      doc = REXML::Document.new(file)
      result = self.new_from_xml(doc.root)
    end
    file.close
    return result
  end

  
  # Saves this tile map to the given file name.
  def save_to(filename)
    raku = self.to_raku
    return false unless raku 
    # Do conversion first, so the map is not clobbered in case it bombs.
    return File.open(filename, 'w') do |file|
      file.write(raku)
    end
  end
  
  def screen_to_tile_x(layer_index, screen_x)
    @layers[layer_index].screen_to_tile_x(@x, screen_x)
  end 
  
  def screen_to_tile_y(layer_index, screen_y)
    @layers[layer_index].screen_to_tile_y(@y, screen_y)
  end 

  def tile_to_screen_x(layer_index, tile_x)
    @layers[layer_index].tile_to_screen_x(@x, tile_x)
  end 
  
  def tile_to_screen_y(layer_index, tile_y)
    @layers[layer_index].tile_to_screen_y(@y, tile_y)
  end 
  
  # Returns amunt of tiles that the map uses. 
  # Warning DO NOT USE FOR ITERATION!
  def tile_count
    return @tileset.size
  end
  
  def layer_count
    return @layers.size
  end

  def set(x, y , z, tile_id) 
    layer = @layers[z]
    return nil if layer.nil?
    layer.set(x, y, tile_id)
  end
  
  # Gets the layer with the given index, or nil if no such layer
  def layer(z)
    return @layers[z]
  end
  
  # Gets the tile index at this location
  def get(x, y , z) 
    layer = @layers[z]
    return nil if layer.nil?
    layer.get(x, y)
  end
  
  # Gets the tile at this location
  def get_tile(x, y, z) 
    layer = @layers[z]
    return nil if layer.nil?
    layer.get_tile(x, y)
  end
  
  
  # Iterates over all layers. Yields the index of the layer and the Layer itself.
  def each_layer
    index = 0
    for index in 0...@layers.size 
    # It seems that ranges are about just as fast as a while loop...
    # while index < @layers.size
      yield index, @layers[index]
    #  index += 1  
    end
  end
  
  # Determines the map's size, based upon the biggest dimensions of each 
  # layer in the map.
  def map_size
    wres = 0
    hres = 0
    each_layer do |li, layer|
      wres = layer.w if layer.w > wres
      hres = layer.h if layer.h > hres
    end
    return wres, hres
  end
  
  # Initializes this map's camera
  def initialize_camera(screen)
    
  end
  
end
