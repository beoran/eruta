require 'stringio'

module Tima
  class Map
    # Width of map in tiles
    attr_reader :wide_tile
    # Height of map in tiles
    attr_reader :high_tile
    # List of tiles (TileList) that can be used in the map
    attr_reader :list
    # Layers of the map
    attr_reader :layers
    
    def initialize(list, layers, attributes = {})
      @layers                = layers
      @background            = nil
      @list                  = list
      @attributes            = attributes
      @backname              = @attributes[:background]
      load_background(@backname)
    end
    
    def load_background(backname)
      p "background:", backname, @attributes
      return nil unless backname
      toread      = File.join(Fimyfi.background_dir, backname)
      @background = Gari::Image.loadsolid(toread)
      p toread
      p @background
      return @background
    end
    
    # Returns the amount of layers in this map.
    def layer_count
      return @layers.size
    end
    
    
    def to_raku
      res = StringIO.new('')
      res.puts('map {')
      res.puts("  background '#{@backname}'") if @backname
      res << @list.to_raku
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
      ma        = raku.find_first(:map)
      bg        = ma.find_first(:background)
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
      map   = self.new(list, layers, attrs)
      # map.prepare_animated_tiles(list)
      return map
    end
    
    # Draws the map to the given screen.
    def draw(screen, x, y)
      if @background
        screen.blit(0, 0, @background) # draw background if any simply at (0,0).
      end
      
      stop  = @layers.size
      index = 0
      while index < stop          # while loops are faster than iterators.
        layer  = @layers[index]
        layer.draw(screen, x, y)
        index += 1
      end
    end
    
    # Loads the map from the given open file handle.
    def self.load_file(file)
      text        = file.read
      return nil, "Could not read file." unless text
      raku, err   = Raku.parse_to_node(text)
      return nil, err unless raku
      result, err = self.new_from_raku(raku)
      return result, err
    end
    
    # Loads the map from the given file name.
    def self.load(filename)
      file        = File.open(filename, "rt")
      return nil, "Could not open file." unless file
      res, mesg = self.load_file(file)
      file.close
      return res, mesg
    end
    
    # Saves the map to the given open file handle.
    def save_file(file)
      return nil, "Could not open file." unless file
      text        = self.to_raku
      return nil, "Could not create text" unless text 
      return file.write(text), "OK!"
    end
    
    # Saves the map from the given file name.
    def save_to(filename)
      file        = File.open(filename, "wt")
      return nil, "Could not open file." unless file
      res, mesg = self.save_file(file)
      file.close
      return res, mesg
    end
    
    # Returns the width of the widest layer
    def wide
      w = 0
      @layers.each do | l |
        w = l.wide if l.wide > w
      end
      return w
    end
    
    # Returns the height of the highest layer
    def high
      h = 0
      @layers.each do | l |
        h = l.high if l.high > h
      end
      return h
    end
    
  end
end



