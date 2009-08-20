module Rogaru
  module Tilemap
    class Tile
    
      # An animation program for a tile. Loaded from an "animation tile"
      class Animation
        
        # Amount of frames in this animation
        attr_reader :size
        attr_reader :offset
        
        # Operations in the animation
        WAIT        = 0 # Do nothing (wait 250 ms)
        NEXT        = 1 # Increase the tile index by one
        REWIND      = 2 # Rewind the tile index to the first tile animation 
        
        def self.is_animation_tile?(image)
          corner = image.get_point(0,0)
          return corner == Sisa::Color::TILE_ANIMATION
        end
        
        def load_animation(image)
          for index in (0..31)  
            point = image.get_point(index,1)
            if point == Sisa::Color::ANIMATION_NEXT
              @animation << NEXT
              @size += 1 # Animation size increases by 1 if we have a next animation step
            elsif point == Sisa::Color::ANIMATION_REWIND
              @animation << REWIND
              break # all done if we get a rewind
            else # Otherwise it's a WAIT
              @animation << WAIT              
            end
          end  
          # Be sure to end the animation with a rewind if it was missing!
          @animation << REWIND if @animation.last != REWIND
        end
        
        # Initializes this animation info from a tile index and the animation 
        # information tile 
        def initialize(tile_index, image)
          @tile_index = tile_index  # Animation starts at this index 
          @animation  = []
          @index      = 0           # Index to the current step in the animation program
          @offset     = 0           # Offset to the tile to display
          @size       = 1           # Size starts out at 1 
          load_animation(image)
        end
        
        # Updates the tile's animation according to it's animation program
        def update()
          instruction  = @animation[@index]
          case instruction
            when REWIND # Back to first tile
              @index = 0
              @offset = 0
            when NEXT # Move to next tile in animation            
              @index  += 1
              @offset += 1
            else # Wait, only increasing the index
              @index += 1
          end
        end
      end
  
  def self.set_tile_directory(tiledir)
    @@tiledir = tiledir    
  end

  def self.get_tile_directory()
    return @@tiledir    
  end
  
  attr_reader   :index
  attr_reader   :type
  attr_accessor :walk
  attr_accessor :swim  
  attr_accessor :jump
  attr_accessor :ledge
  attr_accessor :pain
  attr_accessor :up
  attr_accessor :down
  attr_accessor :block 
  attr_accessor :direction
  attr_accessor :animation
  
  def initialize(index = nil, type = :solid) 
    @type     = type
    @index    = index
    @images   = []
    @image    = nil
    @max      = 0
    @now      = 0
    # Below are the tile's effects.
    @walk     = true    # The tile can be walked over.
    @jump     = true    # The tile can be jumped over.
    @ledge    = false   # The tile will cause a jump when walked over.
    @swim     = false   # The tile is some kind of swimmable liquid.
    @pain     = false   # Walking over the tile causes pain.
    @up       = false   # The tile is a step-up to the next layer.
    @down     = false   # The tile is a step-down to the previous layer.
    @block    = false   # The tile blocks anything from being pushed inside it.
    @direction= nil     # The direction of this tile's effect.
    @animation= nil  
  end
  
  # Copies the tile's properties from another tile,without changing the 
  # images
  def copy_info(tile)
    @walk     = tile.walk
    @swim     = tile.swim
    @jump     = tile.jump
    @ledge    = tile.ledge
    @pain     = tile.pain
    @up       = tile.up
    @down     = tile.down
    @type     = tile.type
    @direction= tile.direction 
  end

  # Removes all surfaces from this tile
  def clear_surfaces
    @images = []
    @max    = 0
    @now    = 0
  end
  
  # adds a surface to the animations of this tile
  def add(surface)    
    @images.push(surface)
    @image = @images[0]
    @max += 1
    return @max - 1
  end
  
  # Gets the Surface of the tile at the given animation phase
  def get_phase(phase) 
    return @image
  end  
  
  # Get the currently active Surface for this tile
  def get()
    return @image
  end
  
  # Advances the animation of this tile. 
  # Note that this may only be called if the tile has an animation, for performance reasons 
  def advance()
    @animation.update
    @now    = @animation.offset
    @image  = @images[@now]
  end

  def self.xml_get_yesno_property(xml, attribute, default)
    xml_prop = xml.attributes[attribute]
    if xml_prop.nil? or xml_prop.empty?
      value = default
    elsif xml_prop == 'no'
      value = false
    elsif xml_prop == 'yes'  
      value = true
    else 
      value = default
    end
    return value
  end

  def self.xml_set_yesno_property(value)
    return 'yes' if value
    return 'no'
  end

  def to_xml() 
    xml   = REXML::Element.new 'tile';
    xml.attributes['index']  = @index
    xml.attributes['walk']   = Tilemap::Tile.xml_set_yesno_property(self.walk)
    xml.attributes['swim']   = Tilemap::Tile.xml_set_yesno_property(self.swim)
    xml.attributes['jump']   = Tilemap::Tile.xml_set_yesno_property(self.jump)
    xml.attributes['ledge']  = Tilemap::Tile.xml_set_yesno_property(self.ledge)
    xml.attributes['up']     = Tilemap::Tile.xml_set_yesno_property(self.up)
    xml.attributes['down']   = Tilemap::Tile.xml_set_yesno_property(self.down)
    xml.attributes['pain']   = Tilemap::Tile.xml_set_yesno_property(self.pain)
    return xml
  end  

  
  def self.new_from_xml(xml)
    tile_dir    = Tilemap::Tile.get_tile_directory
    index       = xml.attributes['index'].to_i 
    tile        = Tilemap::Tile.new(index)
    tile.walk   = self.xml_get_yesno_property(xml, 'walk' , true )
    tile.swim   = self.xml_get_yesno_property(xml, 'swim' , false)
    tile.jump   = self.xml_get_yesno_property(xml, 'jump' , true )
    tile.ledge  = self.xml_get_yesno_property(xml, 'ledge', false)
    tile.up     = self.xml_get_yesno_property(xml, 'up'   , false)
    tile.down   = self.xml_get_yesno_property(xml, 'down' , false)
    tile.pain   = self.xml_get_yesno_property(xml, 'pain' , false)
    tile.block  = self.xml_get_yesno_property(xml, 'block', false)
    return tile
  end

  def self.tmx_get_properties(tmx)
    result = {}
    tmx.elements.each('properties/property') do | tmx_prop |
      name  = tmx_prop.attributes['name'].strip        
      value = tmx_prop.attributes['value'].strip
      result[name] = value 
    end
    return result
  end

  def self.tmx_yesno_property(props, name, default)
      value = props[name]
      if value ==  'yes'
        return true
      elsif value == 'no'
        return false
      else 
        return default
      end
  end  

  # loads tile info a tmx document (www.mapeditor.org)  
  def self.new_from_tmx(tmx, firstgid)
    props       = tmx_get_properties(tmx)
    tile_dir    = Tilemap::Tile.get_tile_directory
    index       = tmx.attributes['id'].to_i + firstgid 
    tile        = Tilemap::Tile.new(index)
    tile.walk   = self.tmx_yesno_property(props, 'walk' , true)
    tile.swim   = self.tmx_yesno_property(props, 'swim' , false)
    tile.jump   = self.tmx_yesno_property(props, 'jump' , true)
    tile.ledge  = self.tmx_yesno_property(props, 'ledge', false)
    tile.up     = self.tmx_yesno_property(props, 'up'   , false)
    tile.down   = self.tmx_yesno_property(props, 'down' , false)
    tile.pain   = self.tmx_yesno_property(props, 'pain' , false)
    tile.block  = self.tmx_yesno_property(props, 'block', false)
    return tile
  end  

  def animated?()
    return !(@animation.nil?)
  end
  
    end # class Tile
  end # module Tilemap
end # module Rogaru
