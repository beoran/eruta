
# Style is eveything relzted to the look and feel of a widget, 
# That is the foreground, background, border colors as well as 
# the font
module Zori 
  class Style
    # Style colors is a class that groups the colors of the style
    class Colors
          
      attr_accessor :text
      attr_accessor :background
      attr_accessor :border
      attr_accessor :highlight
      attr_accessor :shadow
      
      def initialize(tx = nil, bg = nil, br = nil, hi = nil, sh = nil)
        @text       = tx || Sisa::Color::White
        @background = bg || Sisa::Color::DarkGreen
         # PaleGreen # GoldenWhisp # PaleGoldenRod
        @border     = br || Sisa::Color::Gray
        @highlight  = hi || Sisa::Color::White
        @shadow     = sh || Sisa::Color::ShadowGray
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
    end
    
    attr_reader   :font
    attr_reader   :colors
    attr_reader   :margin
    attr_reader   :padding
    attr_accessor :alignment
    
    DEFAULT_FONTNAME        = 'dejavuserif' # 
    DEFAULT_MARGIN          = 4
    DEFAULT_PADDING         = 4
    DEFAULT_ALIGNMENT       = :left  
    DEFAULT_BACKGROUND      = 'green.png'
    
    def self.font_dir
      return File.join( '..', 'share', 'font', '..')
    end
    
    def initialize(fontname = DEFAULT_FONTNAME, fontsize = 12, colors_in = nil, background = DEFAULT_BACKGROUND)
      if fontsize == :font_handle
        @font       = fontname
      else
        fontname  ||= DEFAULT_FONTNAME
        fontsize  ||= 12
        @font       = Sisa::Font.load(fontname, fontsize)
      end        
      @colors     = colors_in  || Zori::Style::Colors.new
      @margin     = DEFAULT_MARGIN
      @padding    = DEFAULT_PADDING
      @alignment  = DEFAULT_ALIGNMENT
      load_background(background)
      yield self if block_given?
    end
    
    def load_background(back)
      if back.is_a? String        
        @bgimage    = Hanao.load_image('background', back) rescue nil;
      else  
        @bgimage  = back
      end  
    end  
    
    
    # Returns a deep copy of self 
    def deep_copy()
      new_colors = self.colors.deep_copy();
      return self.class.new(@font.name, @font.pointsize, new_colors, @bgimage)
    end
    
    # Returns a deep copy of self
    def clone()
      p @colors
      result           = self.class.new(@font.name, @font.size, @colors.clone)
      result.alignment = self.alignment
      return result
    end
    
    def font_name()
      return self.font.name
    end
    
    def font_pointsize()
      return self.font.pointsize
    end
    
    def colors_text()
      return self.colors.text
    end  
    
    def colors_background()
      return self.colors.background
    end  
    
    def colors_highlight()
      return self.colors.highlight
    end  
    
    def colors_border()
      return self.colors.border
    end  
    
    def colors_shadow()
      return self.colors.shadow
    end
      
    # draws a text with this style's active font and colors
    def draw_text(target, x, y, text) 
      self.font.draw(target, x, y, text, colors_text, colors_background)
    end  
    
    # renders a text with this style's active font and colors  
    def render_text(text) 
      self.font.render(text, colors_text, colors_background)
    end 
     
    # Draws a border  
     
    # Draws a border with the correct style settings 
    def draw_border(target, widget)
      x , y , w , h = * widget.dimensions
      x += 1 ; y += 1 ; w -= 2 ; h -= 2       
      target.put_rectangle(x, y, w, h,  @colors.border)
      self.draw_outset(target, widget)
    end 
    
    def draw_outset(target, widget)
      x , y , w , h = * widget.dimensions
      w -= 1 ; h -= 1
      target.line(x    , y    , w     , 0     , @colors.highlight)
      target.line(x    , y    , 0     , h     , @colors.highlight)
      target.line(x + w, y    , 0     , h     , @colors.shadow)
      target.line(x    , y + h, w     , 0     , @colors.shadow)
    end     
    
    def draw_inset(target, widget)
      x , y , w , h = * widget.dimensions
      w -= 1 ; h -= 1
      target.line(x    , y    , w     , 0     , @colors.shadow)
      target.line(x    , y    , 0     , h     , @colors.shadow)
      target.line(x + w, y    , 0     , h     , @colors.highlight)
      target.line(x    , y + h, w     , 0     , @colors.highlight)
    end
    
    # Draws the background color for this widget on the target  
    def draw_background_color(target, widget)
      return unless self.colors.background
      target.fill_rectangle(widget.x, widget.y, widget.w, widget.h, self.colors.background)      
    end
    
    # Draws the background image for this widget on the target
    def draw_background_image(target, widget)
      return unless @bgimage
      xscale = widget.w.to_f / ( @bgimage.w.to_f + 1)
      yscale = widget.h.to_f / ( @bgimage.h.to_f + 1) 
      @bgimage.rotozoom_blit(target, widget.x, widget.y, xscale, yscale)
    end
    
    # Draws the background (image or color) for this widget
    def draw_background(target, widget)
      if @bgimage
        return draw_background_image(target, widget)
      else
        return draw_background_color(target, widget)
      end
    end
    
    
        
    
    
  end # class Style
end # module Zori

if $0 == __FILE__
  require 'stringio'
  
  
  class Mtest
    def initialize(name, size)
      @name = name
      @size = size 
    end
    
    def self._load(data)
      arr = Marshal.load(data)
      p arr
      return self.new(*arr)
    end
    
    def _dump(depth=1)
      return Marshal.dump([@name, @size]) 
    end
  end
  
  mtest = Mtest.new('font', 12)
  p mtest
  s = Marshal.dump(mtest)
  p s
  o = Marshal.load(s)
  p o
  
  
end
    