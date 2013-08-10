
# Style is eveything relzted to the look and feel of a widget, 
# That is the foreground, background, border colors as well as 
# the font
module Zori 
  class Style
    # Style colors is a class that groups the colors of the style
    class Colors
          
      attr_accessor :foreground
      attr_accessor :background
      attr_accessor :border
      
      def initialize(fg = nil, bg = nil, br = nil, hi = nil, sh = nil)
        @foreground = fg || [255,255,255]
        @background = bg || [0,0,128]
         # PaleGreen # GoldenWhisp # PaleGoldenRod
        @border     = br || [191,191,191]
      end
      
      # Returns a deep copy of self
      def deep_copy()
        return self.class.new(@text, @background, @border, @highlight, @shadow)
      end
      
    end  
    

    
    # A spacer is used to leave space in the given direction between widgets 
    # or between a widget and it's content
    class Spacers
      attr_accessor :top
      attr_accessor :left
      attr_accessor :bottom
      attr_accessor :right
      
      def initialize(t = 0, l = nil, b = nil, r = nil)
        @top      =  t || 0
        @bottom   =  b || @top
        @left     =  l || @top
        @right    =  r || @left  
      end 
      
      # Returns a deep copy of self
      def deep_copy()
        return self.class.new(@top, @left, @bottom, @right)
      end
    end
    
    attr_reader   :font
    attr_reader   :colors
    attr_reader   :margin
    attr_reader   :padding
    attr_accessor :alignment
    # 
    DEFAULT_FONTNAME        = 'dejavuserif'
    DEFAULT_MARGIN          = 4
    DEFAULT_PADDING         = 4
    DEFAULT_ALIGNMENT       = :left
    DEFAULT_BACKGROUND      = 'green.png'
    
    def self.font_dir
      return '/font'
    end
    
    def initialize(font = nil, colors_in = nil, background = DEFAULT_BACKGROUND)
      @font       = font
      @colors     = colors_in  || Zori::Style::Colors.new
      @margin     = DEFAULT_MARGIN
      @padding    = DEFAULT_PADDING
      @alignment  = DEFAULT_ALIGNMENT
      load_background(background)
      yield self if block_given?
    end
    
    def load_background(back)
      if back.is_a? String
        @bgimage  = Zori::Hanao.load_image('ui', 'background', back) rescue nil;
      else
        @bgimage  = back
      end
    end
    
    
    # Returns a deep copy of self 
    def deep_copy()
      new_colors = self.colors.deep_copy();
      return self.class.new(@font, new_colors, @bgimage)
    end

    def colors_foreground()
      return self.colors.foreground
    end
    
    def colors_highlight()
      return self.colors.highlight
    end
    
    def colors_border()
      return self.colors.border
    end
    
    # Sets the graph node object to the given Zori style.
    def to_graph(node)
      node.background_color = @colors.background
      node.border_color     = @colors.border
      node.color            = @colors.foreground
      # XXX implement this
      # node.border_thickness = @thickness
      Eruta::Graph.border_thickness_(@id, t)
    end
    
    def angle=(a)
      Eruta::Graph.angle_(@id, r, g, b, a)
    end
    
    def size=(size)
      w, h = *size
      Eruta::Graph.size_(@id, w, h)
    end
    
    def self.default
      @default ||= self.new()
      return @default
    end

  end # class Style
end # module Zori

