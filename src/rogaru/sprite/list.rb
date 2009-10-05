module Rogaru
  class Sprite
    # List of sprites
    class List
      #Creates a new empty sprite list
      def initialize
        # All known sprites, ordered by name
        @sprites   = {}        
        # Sprites ordered in drawing layers
        @layers    = []
      end
      
      # Sorts the layer for this sprite by it's y values
      def sort_layer(sprite)
        @layers[sprite.z].sort_by {|s| s.y }
      end  
      
      # Puts a sprite into a layer
      def add_sprite_in_layer(sprite)
        @layers[sprite.z]   ||= []
        @layers[sprite.z]    << sprite
        sort_layer(sprite)
      end  
            
      # Adds a sprite to this list
      def add_sprite(sprite)
        @sprites[sprite.name] = sprite
        add_sprite_in_layer(sprite)
        return sprite
      end
      
      # Makes a new sprite adds it to this list
      def new_sprite(name, x, y, z) 
        sprite = Sprite.new(self, name, x, y, z)
        return self.add_sprite(sprite)
      end
      
      # *Must* be called whenever the sprite's Z value changes,
      # to ensure the sprite ends up in the right layer
      def on_z_change(sprite, old_z, new_z)
        return false if new_z == old_z
        @layers[old_z].delete(sprite)
        add_sprite_in_layer(sprite)
        return true
      end
      
      # *Must* be called whenever the sprite's Y value changes,
      # to ensure the sprite is drawn correctly      
      def on_y_change(sprite, old_y, new_y)
        return false if new_y == old_y
        sort_layer(sprite)
        return true
      end
      
      # Draws one layer sprites with the given index
      def draw_layer(screen, index)
        layer = @layers[index]
        return false unless layer #Layer that doesn't exist is OK
        for sprite in layer do 
          sprite.draw(screen)
        end  
      end
      
      
      # Looks up the sprite by name
      def [](name)
        return @sprites[name.to_sym]
      end
      
      # Returns a new instance of self and loads the sprites
      def self.load
        sprites = self.new()
        sprites.load
        return sprites
      end  
     
      # Loads the sprite images in the sprite folder
      def load()
        folder      = SPRITE_DIR
        dir         = Dir.new(folder)   
        fileaid     = dir.select { |f| f =~ /.png\Z/ }
        fileaid.sort! 
        # Sort filenames alphabetically for consistent order.
        for filename in fileaid 
          fullname    = File.join(folder, filename)
          res = load_filename(fullname)
          #puts "Loading #{fullname}: #{res ? 'OK': 'FAIL'}"
        end        
      end

      
      NAME_WARNING = "When loading sprite malformed filename. Needs to be of form spritename_part_heightxwidth. Hight and width are the sizes of the individual frames of the sprite, and must both be bigger than 0."
      DEFAULT_WIDE = 32
      DEFAULT_HIGH = 64
      
      # Loads all sprite frames from the given filename,  and returns it. Will make a new sprite if needed.
      def load_filename(filename)
        sprite   = nil  
        basename = File.basename(filename, '.*')
        # Get the base filename without extension        
        aid      = basename.split('_')
        # Split the base name. Info for file is encoded in filename.
        name     = aid[0]
        part     = aid[1]
        size     = aid[2]
        unless name && part
          # warn NAME_WARNING 
          return sprite         
        end
        # Size is optional, assumed to be 64 high by 32 wide
        if size
          said    = size.split('x')
          unless said[0] && said[1]
            # warn NAME_WARNING
            return sprite
          end
          wide    = said[0].to_i
          high    = said[1].to_i
          unless high > 0 && wide > 0
            # warn NAME_WARNING
            return false          
          end
        else #use default size values
          high    = DEFAULT_HIGH
          wide    = DEFAULT_WIDE
        end
        # Look up the sprite to load the frames into 
        sprite    = self[name]
        # If there is no such sprite yet, make a new one
        sprite  ||= self.new_sprite(name, 0, 0, 0)
        sprite.load_poses(filename, part, wide, high)
        return sprite
      end
    end
  end
end  
