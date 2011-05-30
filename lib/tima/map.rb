module Tima
  class Map
    # Width of map in tiles
    attr_reader :wide_tile
    # Height of map in tiles
    attr_reader :high_tile
    
    def initialize(wide_tile, high_tile, layers, attributes = {})
      @wide_tile, @high_tile = wide_tile, high_tile
      @layers                = layers
      @backname              = attributes[:background]
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
  
    # Loads the map from a Raku file
    def self.new_from_raku(raku)
      layers    = []
      list      = Tima::Tilelist.new_from_raku(raku)
      bg        = raku.find_first(:map, :background)
      backname  = bg ? bg.data[0] : nil
      rlayers   = raku.find_all(:map, :layers, :layer)
      if rlayers 
        for rlayer in rlayers
          layer = Tima::Layer.new_from_raku(rlayer, list)
          layers << layer
        end
      else 
        warn "Map has no layers!"
      end   
      attrs = { :background => backname } 
      map = self.new(tileset, layers, attrs)
      map.prepare_animated_tiles(tileset)
      return map
    end
    
    def draw(screen, x, y)
      for layer in layers do
        layer.draw(screen, x, y)
      end
    end
    
    # Loads the map from the given file name.
    def self.load(filename)
      file        = File.open(filename, "rt")
      return nil, "Could not open file." unless file
      text        = file.read
      return nil, "Could not read file." unless text 
      raku, err   = Raku.parse_to_node(text)
      return nil, err unless raku
      result, err = self.new_from_raku(raku)
      return result, err
    end
    
  end
end



