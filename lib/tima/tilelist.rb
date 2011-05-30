require 'fimyfi'

module Tima
  # A tile list is a list of tiles, indexed by number. 
  # It holds references to the individual tiles. 
  # These tiles are loaded from several image files. 
  class Tilelist
  
    TILE_WIDE = 32
    TILE_HIGH = 32
    
    def initialize(filenames)
      @filenames = filenames
      unless filenames && filenames.size > 0
        @filenames = [ 'tile_world_1000.png' ]
      end  
      @images    = {}
      @tiles     = {}
      load_filenames(@filenames)
    end
    
    def self.index_for_filename(filename)
      m = filename.match(/_([0-9]+)\./)
      return nil unless m
      return m.to_i
    end
    
    # Sets the image to use for an index
    def set_image(index, image)
      @images[index] = image
    end
    
    # Sets the tile info to use for an index
    def set_image(index, tile)
      @tiles[index] = tile
    end
    
    # Loads the image for the filename, and retuns a hash of index to 
    # the images of the tiles
    def load_filename_image(filename)
      result = {}
      index  = index_for_filename(filename)
      return nil, "File name not valid." unless index
      image = Gari::Image.loadraw(File.join(Fimyfi.tile_dir,  filename))
      return nil, "Could not open file"  unless surface
      (0...(image.h)).step(TILE_HIGH)   do |y|
        (0...(image.w)).step(TILE_WIDE) do |x|
          aidimage  = image.copy_rectangle(x, y, TILE_WIDE, TILE_HIGH)
          optimage  = aidimage.optimize(:alpha)
          # XXX: should detect if alpha is needed,
          # probably in Gari::Image
          result[index]  = optimage
          index         += 1
        end
      end
      return result, nil
    end
    
    # Loads all filenames for this tilelist
    def load_filenames(filenames)
      for filename in filenames.sort do
        aid, err = load_filename_image(filename)
        unless aid
          warn err
        else
          @images.merge(aid)
        end 
      end
    end
    
    # Converts all images in the images hash to tiles
    def images_to_tiles(images)
      tiles = {}
      return nil unless images
      for index, image in images do
        tile          = Tile.new(index, [image])
        tiles[index]  = tile
      end
      return tiles
    end
    
    # Save to Raku format
    def to_raku
      res = StringIO.new('')
      res.puts("  tileset {")
      res.puts("    tilesize #{@tilewide} #{@tilehigh}")
      res.puts("    names {")
      for name in @names do
        res.puts("      name #{name}")
      end
      res.puts("    }")
      res.puts("    tiles {")
      self.each() do | tile |
        next unless tile
        res << tile.to_raku
      end
      res.puts("    }")
      res.puts("  }")
      return res.string
    end
    
    # FIXME: stub
    def self.new_from_raku(raku)
      rats = raku.find_first(:map, :tileset)
      p rats
      return nil unless rats
      tsiz = rats.find_first(:tilesize)
      return nil unless tsiz
      tilewide, tilehigh = *tsiz.data
      names  = []
      rnames = rats.find_all(:names, :name)
      for rname in rnames do
        name = rname.data[0]
        names << name if name
      end
      p names, tilewide, tilehigh
      tileset = self.new(names)
      rtiles  = rats.find_all(:tiles, :tile)
      for rtile in rtiles
        tile = Tilemap::Tile.new_from_raku(rtile)
        tileset.set_tile_info(tile.index, tile)
      end
      return tileset
    end

  end
end
    