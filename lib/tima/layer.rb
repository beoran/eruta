module Tima
  class Layer
    
    # Width in tile units
    attr_reader :wide_tile
    
    # Height in tile units
    attr_reader :high_tile
    
    # Width in pixels
    attr_reader :wide
    
    # Height in pixels
    attr_reader :high
    
    # The width and height of tiles is 32 pixels
    TILE_HIGH   = 32
    TILE_WIDE   = 32
    
    def initialize(wide_tile, high_tile, tilelist)
      @wide_tile  = wide_tile
      @high_tile  = high_tile
      @list       = tilelist
      @tiles      = Array.new(@high_tile) { Array.new(@wide_tile, nil)  }
      @indexes    = Array.new(@high_tile) { Array.new(@wide_tile,  -1)  }
      @wide       = @wide_tile * TILE_WIDE
      @high       = @high_tile * TILE_HIGH
    end
    
    # Returns true if x_tile and y_tile are outside the bounds
    # of the map
    def outside?(x_tile, y_tile)
      return (x_tile < 0)           || (y_tile < 0)           ||
             (x_tile >= @wide_tile) || (y_tile >= @high_tile)
    end
    
    # Sets the tile at the given tile coordinates to the one 
    def set(x_tile, y_tile, index)
      return nil if outside?(x_tile, y_tile)
      tile                      = @list.get_image(index)
      @tiles[y_tile][x_tile]    = tile
      @indexes[y_tile][x_tile]  = index
      return tile
    end
    
    # Gets the index at the given coordinate
    def get(x_tile, y_tile)
      return nil if outside?(x_tile, y_tile)
      return @indexes[y_tile][x_tile]
    end
    
    # Gets the tile at the given coordinate
    def get_tile(x_tile, y_tile)
      return nil if outside?(x_tile, y_tile)
      return @tiles[y_tile][x_tile]
    end
    
    def get_tilerow(y_tile)
      return nil if outside?(0, y_tile)
      return @tiles[y_tile]
    end
    
    # Saves data to a raku string.
    def to_raku(z)
      res = StringIO.new('')
      res.puts('  layer {')
      res.puts("    z    #{z}")
      res.puts("    size #{@wide_tile} #{@high_tile} #{TILEWIDE} #{TILEHIGH}")
      res.puts('    data {')
      for y in (0...@high_tile) do
        res.puts("      row #{y} " + @indexes[y].join(' '))
      end
      res.puts('    }')
      res.puts('  }')
      return res.string
    end
    
    # Loads a layer from a raku data source.
    def self.new_from_raku(rlayer, list)
      rz = rlayer.find_first(:z)
      z  = rz.data[0].to_i
      rsz= rlayer.find_first(:size)
      wide, high, tilewide, tilehigh = *rsz.data
      layer = self.new(wide, high, list)
      rrows = rlayer.find_all(:data, :row)
      for rrow in rrows do
        y   = rrow.data.shift.to_i
        row = rrow.data
        stop = row.size
        stop = wide if stop > wide
        for x in (0...stop) do
          layer.set(x, y, row[x])
        end
      end
      return layer
    end
    
    # draws the visible part of the layer to the screen.
    def draw(screen, x, y)
      ifake       = 0
      x_start     = ( x / TILE_WIDE )
      y_start     = ( y / TILE_HIGH )
      x_delta     = (screen.w / TILE_WIDE) + 1
      y_delta     = (screen.h / TILE_HIGH) + 1
      x_stop      = x_start + x_delta
      y_stop      = y_start + y_delta
      x_draw      = 0
      y_draw      = 0
      return if (x_start >= @wide_tile or y_start >= @high_tile)
      x_start     = 0 if (x_start < 0)
      y_start     = 0 if (y_start < 0)
      x_stop      = @wide_tile if (x_stop > @wide_tile)
      y_stop      = @high_tile if (y_stop > @high_tile)
      y_draw      = -y + ((y_start - 1) * TILE_HIGH )
      y_index     = y_start
      while y_index < y_stop do
        y_draw   += TILE_HIGH
        x_draw    = -x + ((x_start - 1) * TILE_WIDE )
        row       = @tiles[y_index]
        x_index   = x_start
        while x_index < x_stop
          x_draw  += TILE_WIDE
          tile     = row[x_index]
          unless tile.nil?
            image  = tile # .image
            screen.blit(x_draw, y_draw, image)
          end
          x_index += 1
        end
        y_index   += 1
      end
    end
    
    
  end
end