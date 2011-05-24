require 'stringio'

module Rogaru 
  module Tilemap 
    class Tileset
    
      attr_reader :tilewide
      attr_reader :tilehigh
      
      def self.set_tile_directory(tiledir)
        @@tiledir = tiledir    
      end

      def self.get_tile_directory()
        return @@tiledir    
      end
    
      include Enumerable 
      TILESET_DIR = File.join('data','image','tile')
      # Default tileset directory.  XXX: use Settings to load this.
      attr_reader :name
      attr_reader :tiles
      attr_reader :start
      attr_reader :index
      attr_reader :filenames
      # Makes a new tileset wich uses tiles from the tile directories 
      #  with the given array of names, and tile dimensions.
      # Optionally add an array of Tiles to get the info from  
      def initialize(names,  tilewide = 32, tilehigh = 32, do_load = true, load_start = {})
        @names      = names
        @tilewide   = tilewide
        @tilehigh   = tilehigh
        @tiles      = {} 
        @folders    = []
        @filenames  = []
        @index      = 0
        @indexes    = []  
        @load_start = load_start
        load if do_load
      end
      
      # Loads the tile images in the tile folder
      def load()
        for name in @names do  
          start_index = @load_start[name]
          folder      = File.join(TILESET_DIR, name)
          load_folder(folder, start_index)
          @folders << folder
        end
      end  
      
      # Loads all tile images forom one tile folder
     def load_folder(folder, load_start = nil)
        dir         = Dir.new(folder)   
        fileaid     =  dir.select { |f| f =~ /.png\Z/ }
        fileaid.sort! 
        # Sort filenames alphabetically for consistent order.
        for filename in fileaid do 
          fullname    = File.join(folder, filename)
          load_filename(fullname, load_start)
        end
        @filenames += fileaid
      end
      
      # Loads all tiles from the given filename
      def load_filename(filename, load_start = nil)
        if filename =~ /anim.png\Z/
          return load_anim_filename(filename, load_start)
        else
          return load_static_filename(filename, load_start)
        end
      end
        
      # Loads this tile image as an animation
      def load_anim_filename(filename,  load_start = nil)
        return false
      end
      
      # Copy the tile info from the tile to the tile at the given tile index.  
      def set_tile_info(index, tile)
        mytile = @tiles[index]
        mytile.copy_info(tile) if mytile
      end

      protected
      # Directly set the tile at this index  
      def set_tile!(index, tile)
        @tiles[index] = tile
      end
      
      # Adds a tile and returns it and it's index, lowlevel function for
      # Refactoring add_anim and add_tile. Don't use it otherwise.
      # XXX handling of tiles is a bit hackish since they don't conntain 
      # surface info, just property info
      def add_tile_lowlevel()
        idx           = @index
        tile          = self.get_tile(idx)
        if tile # this tile already exists, just clear it
          tile.clear_surfaces
        else # make a new tile 
          tile           = Tilemap::Tile.new(idx, :solid)    
          set_tile!(idx, tile)
          @indexes      << idx        
          @index         += 1          
        end        
        return tile, idx
      end
      
      public
      # Adds a tile animation from an array of images to this tile set
      def add_anim(images)        
        tile, idx     = add_tile_lowlevel()
        images.each   { |img| tile.add(img) }
        return idx, tile
      end
      
      # Adds a tile image to this tile set, with the given animation info
      def add_tile(image, animation=nil)
        tile, idx     = add_tile_lowlevel()
        tile.add(image)
        tile.animation = animation
        return idx, tile
      end
      
      # parses the filename to get the start index
      def start_for_filename(filename)
        parts = filename.split('_')
        aid   = parts[1] # second part of filename should be start index
        return aid.to_i if aid
        return 0
      end
      
      # Loads this tile image as a set of static tiles.
      def load_static_filename(filename, load_start = nil)
        ani_info  = nil
        ani_count = nil
        ani_tile  = nil
        # Helpers for loading animations 
        if load_start
          @start  = load_start 
          puts "load_start: #{load_start}"
        else
          @start  = start_for_filename(filename)
        end

        @index  = @start
        surface = Gari::Image.loadraw(filename)
        return false unless surface
        printf "Copying rectangles for tile set..."
        (0...(surface.h)).step(@tilehigh)   do |y|
          (0...(surface.w)).step(@tilewide) do |x|
            tile    = surface.copy_rectangle(x, y, @tilewide, @tilehigh)
            oktile  = tile.optimize(:alpha)
            isanim  = Rogaru::Tilemap::Tile::Animation.is_animation_tile?(oktile)
            if isanim
              puts("Loading animation!")
              tile_index = @index              
              ani_info   = Rogaru::Tilemap::Tile::Animation.new(tile_index, oktile)
            else  
              if ani_info # Load frames into tile animation
                if ani_tile # If we have an animation tile...
                  # add one more frame to the tile 
                  ani_tile.add(oktile)
                  ani_count += 1  
                else # No animation tile, make one 
                  idx, ani_tile = add_tile(oktile, ani_info)
                  ani_count  = 1
                end
                # Stop collecting frames for the animation,
                # if we have all the frames we need...
                if ani_count >= ani_info.size
                  p ani_info
                  p ani_tile
                  # Clear these flag variables
                  ani_info  = nil 
                  ani_count = nil
                  ani_tile  = nil
                end  
              else # it's a normal, non-animated tile
                idx, tile = add_tile(oktile, nil)   
              end
              # load with preexisting animation info (from info tile), 
              # and reset that animation information
            end
            printf "."
            # XXX: should detect if alpha is needed, 
            # probably in Sisa::Surface 
          end
        end
        puts "Done!"
        return false
      end
      
      # Returns the integer tile index of the first tile 
      # in this tile set.
      def first_index  
        return @indexes.min
      end  
      
      # Returns the integer tile index of the last tile 
      # in this tile set.
      def last_index  
        return @indexes.max
      end  
      
      # Returns the smallest integer index of the tile that  
      # has a tile index strictly higher than i.
      # In other words, it's a way to get the next tile after i.
      # Remark: tile indexes need not be contiguous.
      # Returns nil if no such element is available.
      def next_index(i)
        return @indexes.select{ |e| e > i }.min
      end
      
      # Returns the highest integer index of the tile that  
      # has a tile index strictly lower than i.
      # In other words, it's a way to get the tile before i.  
      # Remark: tile indexes need not be contiguous.
      # Returns nil if no such element is available.      
      def previous_index(i)
        return @indexes.select{ |e| e < i }.max
      end
      
      # Returns the tile at tile index i or nil if there is no such tile   
      # Negative tile indexes always return nil (guaranteed) 
      def get_tile(i)
        return @tiles[i]
      end

      # Returns the tile at tile index i or nil if there is no such tile   
      alias :'[]'  :get_tile
      
      # Returns whether or not the tile at index i is animated. 
      # Returns nil if there is no such tile, false if it is not 
      # animated, true if it is.
      def animated?(i)
        tile = get_tile(i)
        return nil unless tile
        return tile.animated? 
      end
      
      # Iterator of Enumerable.
      def each()
        iaid = first_index
        if block_given?
          while iaid
            yield self[iaid]
            iaid = next_index(iaid)
          end  
        else
          raise "Can't return an enumerator (yet)."
        end        
      end

      # Save to REXML  XML element
      def to_xml() 
        xml   = REXML::Element.new 'tileset';
        xml.attributes['tilewide']  = @tilewide
        xml.attributes['tilehigh']  = @tilehigh
        xml_names = xml.add_element 'names'
        for name in @names do
          xml_name       = xml_names.add_element('name')
          xml_name.text = name
        end  
        xml_tiles = xml.add_element 'tiles'
        self.each() do | tile |
          next unless tile
          xml_tiles.add_element(tile.to_xml())
        end
        return xml
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
      
      # Translates a tile index from tmx to our index. 
      def index_from_tmx(tileindex)
      end

      # Loads self from REXML xml document
      def self.new_from_xml(xml)
        p xml  
        tilewide = xml.attributes['tilewide'].to_i rescue 32
        tilehigh = xml.attributes['tilehigh'].to_i rescue 32
        names    = []
        xml.elements.each('names/name') do | xml_name |
          p xml_name
          names << xml_name.text.strip
        end      
        tileset = self.new(names, tilewide, tilehigh, true)    
        xml.elements.each('tiles/tile') do | xml_tile |
          tile = Tilemap::Tile.new_from_xml(xml_tile)
          tileset.set_tile_info(tile.index, tile)
        end       
        return tileset
      end

      # Loads self from REXML xml , which is a tmx (www.mapeditor.org) document
      def self.new_from_tmx(tmx)
        p tmx
        sets      = []
        names     = []
        tilewide  = 32
        tilehigh  = 32
        tiles     = []
        load_start= {}
        mul       = 0
        tmx.elements.each('tileset') do | tmx_tileset |  
          tilewide = tmx_tileset.attributes['tilewidth'].to_i rescue 32
          tilehigh = tmx_tileset.attributes['tileheight'].to_i rescue 32
          firstgid = tmx_tileset.attributes['firstgid'].to_i rescue 0
          name     = tmx_tileset.attributes['name'].strip
          load_start[name] = firstgid 
          tmx_tileset.elements.each('tile') do | tmx_tile |
              tile_id = tmx_tile.attributes['id'].to_i
              tindex  = firstgid   
              puts firstgid
              tile  = Tilemap::Tile.new_from_tmx(tmx_tile, firstgid)
              p tile.index
              p tile.walk
              tiles << tile
          end
          names    << name
          sets     << { name: name , firstgid: firstgid, tiles: tiles}  
          mul += 1000
        end
        
        tileset = self.new(names, tilewide, tilehigh, true, load_start)

        for tile in tiles do
          p tile.index
          p tile.walk
          tileset.set_tile_info(tile.index, tile)
        end


        
        return tileset
      end   
      
      # Returns the amount of ties in this tileset.
      # Warning: DO NOT USE THIS FOR ITERATION! 
      def size
        return @tiles.size
      end
        
    end # class Tileset
  end # module Tilemap
end  # module Rogaru


if $0 == __FILE__
  require 'rexml/document.rb'
  require 'rexml/element.rb'
  require 'sisa'
  require 'rogaru/tilemap/tile.rb'
  @screen = Sisa::Screen.make(640, 480)
  @screen.fill([0,255,0])
  # ts       = Rogaru::Tilemap::Tileset.new(['control','village'])
  doctext = "<tileset tilehigh='32' tilewide='32'><names><name>control</name><name>village</name></names>
  <tiles><tile down='no' index='0' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='2' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='3' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='4' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='5' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='6' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='7' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='8' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='9' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='10' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='11' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='12' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='13' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='14' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='15' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='16' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='17' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='18' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='19' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1000' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1001' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1002' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1003' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1004' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1005' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1006' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1007' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1008' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1009' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1010' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1011' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1012' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1013' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1014' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1015' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1016' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1017' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1018' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1019' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1020' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1021' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1022' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1023' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1024' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1025' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1026' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1027' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1028' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1029' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1030' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1031' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1032' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1033' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1034' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1035' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1036' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1037' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1038' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1039' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1040' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1041' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1042' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1043' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1044' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1045' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1046' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1047' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1048' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1049' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1050' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1051' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1052' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1053' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1054' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1055' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1056' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1057' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1058' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1059' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1060' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1061' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1062' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1063' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1064' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1065' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1066' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1067' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1068' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1069' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1070' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1071' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1072' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1073' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1074' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1075' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1076' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1077' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1078' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1079' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1080' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1081' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1082' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1083' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1084' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1085' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1086' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1087' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1088' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1089' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1090' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1091' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1092' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1093' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1094' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1095' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1096' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1097' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1098' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1099' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1100' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1101' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1102' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1103' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1104' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1105' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1106' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1107' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1108' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1109' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1110' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1111' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1112' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1113' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1114' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1115' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1116' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1117' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1118' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1119' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1120' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1121' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1122' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1123' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1124' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1125' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1126' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1127' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1128' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1129' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1130' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1131' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1132' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1133' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1134' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1135' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1136' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1137' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1138' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1139' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1140' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1141' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1142' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1143' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1144' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1145' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1146' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1147' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1148' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1149' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1150' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1151' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1152' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1153' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1154' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1155' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1156' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1157' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1158' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1159' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1160' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1161' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1162' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1163' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1164' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1165' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1166' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1167' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1168' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1169' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1170' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1171' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1172' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1173' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1174' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1175' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1176' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1177' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1178' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1179' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1180' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1181' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1182' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1183' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1184' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1185' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1186' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1187' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1188' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1189' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1190' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1191' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1192' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1193' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1194' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1195' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1196' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1197' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1198' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1199' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1200' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1201' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1202' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1203' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1204' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1205' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1206' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1207' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1208' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1209' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1210' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1211' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1212' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1213' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1214' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1215' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1216' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1217' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1218' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1219' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1220' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1221' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1222' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1223' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
<tile down='no' index='1224' jump='yes' ledge='no' pain='no' swim='no' up='no' walk='yes'/>
</tiles></tileset>"

  xml     = REXML::Document.new(doctext)
  ts       = Rogaru::Tilemap::Tileset.new_from_xml(xml.root)
  puts ts.to_xml.to_s
  
  
  puts ts.index
  p ts.filenames
  x = 0
  y = 0
  i = ts.first_index 
  # hile i  do
  ts.each do | tile |
    tile.get.blit(@screen, x, y)
    x += 32 
    if x >= 640 
      x = 0
      y += 32
    end
#     i = ts.next_index(i)
  end
  @screen.flip
  sleep 3

end
