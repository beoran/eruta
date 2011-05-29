module Tima
  class Tile
    
    attr_reader   :index
    # boolean properties in a hash 
    attr_reader   :flags
    # currently active image
    attr_reader   :image
    
    attr_accessor :direction
    attr_accessor :animation
    
    def initialize(index, images=[], newflags={})
      @type     = type
      @index    = index
      @images   = images
      @image    = @images.first
      @max      = 0
      @now      = 0
      # Below are the tile's effects.
      @flags    = { :wall  => false, :jump  => false, :ledge => false,
                    :swim  => false, :pain  => false, :up    => false,
                    :down  => false, :block => false }
      @flags.merge(newflag)
      @animation= nil
    end

    # Advances the animation of this tile. 
    # Note that this may only be called if the tile has an animation, for performance reasons 
    def advance()
      @animation.update
      @now    = @animation.offset
      @image  = @images[@now]
    end

    def to_raku
      res = StringIO.new('')
      res.printf("        tile #@index")
      list = [:walk, :swim, :jump, :ledge, :pain, :up, :down]
      for prop in list
        val = self.send(prop)
        res.print(" #{prop}") if (val)
      end
      res.puts
      return res.string
    end

    def self.new_from_raku(rtile, image = nil)
      attrs = rtile.data
      index = attrs.shift.to_i
      props = {}
      for attr in attrs do
        props[attr] = true
      end
      tile  = self.new(index, image, props)
      # Set attribute that was found to true. Other attributes stay false.
      return tile
    end

    def animated?
      return !(@animation.nil?)
    end
  
  end # class Tile
end # module Tima

