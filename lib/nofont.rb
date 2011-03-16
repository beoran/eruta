# encoding: utf-8
#
# Draw text without using fonst. In stead, use a DSL to define a font's 
# drawing based on rectangles. Designed to be mostly compatible with Gari::Font
#
#
#
class Nofont

  # NOTE: prerendering every glyph could seriously speed this up. 
  class Glyph
    attr_accessor :wide
    attr_accessor :high
    attr_reader   :dots
    # Font the glyph belongs to
    attr_reader   :font
    
    def initialize(font)
      @wide = 0
      @high = 0 
      @dots = []
      @font = font
    end
    
    # width of a single glyph
    def width_for_drawing()
      return (self.wide + 1 * self.font.wscale).to_i
    end
    
    # Parses pixel text and fills in dots
    def parse(pixels)
      aid       = pixels.split(/[\r\n]/)
      raise "Pixels not useful" unless aid
      lines     = aid.map{ |e| e.gsub(/[ \t]+/, '') }.reject { |e| e.size == 0 }
      self.high = lines.size 
      raise "Lines not useful" unless self.high > 0
      self.wide = 0
      y         = 0
      for line in lines do
        wnow      = line.size 
        self.wide = wnow if wnow > self.wide
        x         = 0
        line.each_char do |c|
          if c =~ /[a-zA-Z0-9]/ # we found a pixel
            dot = [x, y]
            self.dots << dot
          end
          x       += 1
        end
        y += 1
      end
      return self
    end
    
    def underline(screen, x, y, color)
      # Underline if needed 
      if (self.font.style & UNDERLINE) != 0
        screen.line(x, y + self.font.lineskip, self.width_for_drawing, 0, color)
      end      
    end
    
    def draw(screen, x, y, color)
      # bold and italic factors. 
      bof = (self.font.style & BOLD) == 0 ? 1.0 : 1.5
      itf = 0.0
      if (self.font.style & BOLD) != 0 
        itf = (self.width_for_drawing.to_f / self.font.lineskip.to_f)
      end   
      self..dots.each do | dot |
       dotx, doty = dot[0], dot[1]
       dy   = (doty * self.font.hscale)
       dx   = (dotx * self.font.wscale - (itf * dy))
       outy = y + dy.to_i
       outx = x + dx.to_i
       screen.slab(outx, outy, self.font.wscale * bof, self.font.hscale * bof, color) 
      end
      underline(screen, x, y, color)
      return self 
    end
    
    # Blends a grlyph
    def blend(screen, x, y, color)
      # bold and italic factors. 
      bof = (self.font.style & BOLD) == 0 ? 1.0 : 1.5
      itf = 0.0
      if (self.font.style & BOLD) != 0 
        itf = (self.width_for_drawing.to_f / self.font.lineskip.to_f)
      end   
      self..dots.each do | dot |
       dotx, doty = dot[0], dot[1]
       dy   = (doty * self.font.hscale)
       dx   = (dotx * self.font.wscale - (itf * dy))
       outy = y + dy.to_i
       outx = x + dx.to_i
       screen.blendslab(outx, outy, 
                   self.font.wscale * bof, self.font.hscale * bof, color) 
      end
      underline(screen, x, y, color)
      return self 
    end
    
  
  end
  
  
  ALPHA_SOLID = :solid
  ALPHA_CLEAR = :clear
  BLENDED     = :blended
  SHADED      = :shaded
  SOLID       = :solid
  NORMAL      = 0
  ITALIC      = 1
  BOLD        = 2
  UNDERLINE   = 4
  
  def self.error
    return 0
  end
    
  # XXX: implement this if needed  
  def render
  end
    
    
  # Width of a single glyph
  def width_of_glyph(name)
    info = lookup(name)
    return @wide unless info
    return info.width_for_drawing
  end
    
  # width of a given utf8 encoded text
  def width_of(text)
    result = 0
    text.each_char do |c|
      result += width_of_glyph(c)
    end
    return result
  end
  
  def height
    return self.lineskip
  end
  
  # XXX: implement this if needed
  def ascent
    return self.lineskip
  end
  
  # XXX: implement this if needed
  def descent
    return 0
  end
  
  
  # This kind of font only has one face to it
  def faces
    return 1
  end
    

  attr_reader   :linehigh
  attr_accessor :mode
  attr_accessor :style
  
  # Distance to skip between lines.
  def lineskip
    return @linehigh * @hscale
  end

  def initialize(wscale = 1, hscale = 2, &block)
    @glyphs   = {} 
    @wscale   = wscale
    @hscale   = hscale
    @linehigh = 0
    @mode     = SOLID
    @style    = NORMAL # ITALIC  + BOLD + UNDERLINE
    instance_exec(&block) 
  end
  
  # How much the font is scaled in the width direction
  def wscale
    return @wscale
  end
  
  # How much the font is scaled in the height direction
  def hscale
    return @hscale
  end
  
  
  # Define one glyph for this font.
  def glyph(name, pixels) 
    info = Glyph.new(self)   
    info.parse(pixels)
    @linehigh = info.high if info.high > @linehigh
    @glyphs[name.to_s] = info
    return info
  end
  
  # Define several glyphs in one block 
  def glyphs(names, pixels)
    aid = pixels.split(/[\r\n]/).reject { |l| l.size == 0 }
    arr = aid.map { |l| l.split(/[| \t]+/).reject { |l| l.size == 0 } } 
    # munge the pixels, rows separated by |, spaces or tabs...
    for i in (0...(names.size)) do
      name   = names[i]
      subarr = arr.map { |e| e[i] }
      onepix = subarr.join("\n")
      # will raise exeptions if format is wrong...
      self.glyph(name, onepix)
    end  
  end
  
  def lookup(name)
    @glyphs[name.to_s]
  end
  
  def draw_glyph(screen, x, y, name, color)
    info = self.lookup(name)
    return nil unless info
    return info.draw(screen, x, y, color) 
  end
 
  def blend_glyph(screen, x, y, name, color)
    info = self.lookup(name)
    return nil unless info
    return info.blend(screen, x, y, color) 
  end
 
  
  def draw(screen, x, y, text, color)
    dx     = 0
    text.each_char do |c|
      info = draw_glyph(screen, x + dx, y, c, color)
      w    = info ? info.wide + 1 : 1 
      dx  += (w * @wscale).to_i
    end 
  end
  
  def blend(screen, x, y, text, color)
    dx     = 0
    text.each_char do |c|
      info = blend_glyph(screen, x + dx, y, c, color)
      w    = info ? info.wide + 1 : 1 
      dx  += (w * @wscale).to_i
    end 
  end


  def self.default(wscale = 1, hscale = 2)
    @default ||= make_default(wscale, hscale)
    return @default
  end
    
  # ASCII characters: 
  # !"#$%&'()*+,-./0123456789:;<=>?@
  # ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`
  # abcdefghijklmnopqrstuvwxyz{|}~ 

  # Uses the DSL to define a 5x8 bitmap font.
  def self.make_default(wscale = 1, hscale = 2)
  
    return self.new(wscale, hscale) do
       glyphs [' ', '!', '#', '$', '%', '&', "'", '(', ')', '*'] ,
        "
        ..... ..x.. ..... ..x.. .x..x .xxx. ..x.. ..x.. ..x.. .....
        ..... ..x.. .x.x. .xxxx x.x.x x.... ..x.. .x... ...x. ..x..
        ..... ..x.. xxxxx x.x.. .x.x. .x... ..... x.... ....x x.x.x
        ..... ..x.. .x.x. .xxx. ..x.. x.x.x ..... x.... ....x .xxx.
        ..... ..... xxxxx ..x.x .x.x. x..x. ..... x.... ....x x.x.x
        ..... ..x.. .x.x. xxxx. x.x.x .xx.x ..... .x... ...x. ..x..
        ..... ..x.. ..... ..x.. x..x. ..... ..... ..x.. ..x.. .....
        "
        glyphs %w{+ , - . / 0 1 2 3 4}, 
        "
        ..... ..... ..... ..... ....x ..... ..... ..... ..... .....
        ..x.. ..... ..... ..... ....x .xxx. ..x.. .xxx. xxxx. ...x.
        ..x.. ..... ..... ..... ....x x...x .xx.. x...x ....x ..xx.
        xxxxx ..... xxxxx ..... ...x. x.x.x x.x.. ...x. xxxx. .x.x.
        ..x.. ..... ..... ..... ..x.. x...x ..x.. ..x.. ....x xxxxx
        ..x.. ..... ..... ..... .x... .xxx. xxxxx xxxxx xxxx. ...x.
        ..... ...x. ..... ..x.. x.... ..... ..... ..... ..... .....
        ..... ..x.. ..... ..x.. x.... ..... ..... ..... ..... .....
        "
        
        glyphs %w{5 6 7 8 9 : ; < = >},
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        xxxxx .xxxx xxxxx .xxx. .xxx. ..x.. ...x. ...x. ..... .x...
        x.... x.... ...x. x...x x...x ..x.. ...x. ..x.. xxxxx ..x..
        xxxx. xxxx. xxxxx .xxx. .xxxx ..... ..... .x... ..... ...x.
        ....x x...x .x... x...x ....x ..... ..... x.... ..... ....x
        xxxx. .xxx. .x... .xxx. xxxx. ..... ..... .x... xxxxx ...x.
        ..... ..... ..... ..... ..... ..x.. ...x. ..x.. ..... ..x..
        ..... ..... ..... ..... ..... ..x.. ..x.. ...x. ..... .x...
        "
        
        glyphs %w{? @}, 
         "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        .xxx. .xxx. ..... ..... ..... ..... ..... ..... ..... .....
        x...x x..xx ..... ..... ..... ..... ..... ..... ..... .....
        ...x. x.x.x ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. x.x.x ..... ..... ..... ..... ..... ..... ..... .....
        ..... x..xx ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. x.... ..... ..... ..... ..... ..... ..... ..... .....
        ..... .xxxx ..... ..... ..... ..... ..... ..... ..... .....
        "


       glyphs %w{A B C D E F G H I J}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..x.. xxxx. .xxx. xxxx. xxxxx xxxxx .xxx. x...x xxxxx xxxxx
        .x.x. x...x x...x x...x x.... x.... x.... x...x ..x.. ....x
        x...x xxxx. x.... x...x xxxx. xxx.. x..xx xxxxx ..x.. ....x
        xxxxx x...x x...x x...x x.... x.... x...x x...x ..x.. x...x
        x...x xxxx. .xxx. xxxx. xxxxx x.... .xxx. x...x xxxxx .xxx.
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
       glyphs %w{K L M N O P Q R S T}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        x...x x.... x...x x...x .xxx. xxxx. .xxx. xxxx. .xxxx xxxxx
        x..x. x.... xx.xx xx..x x...x x...x x...x x...x x.... ..x..
        xxx.. x.... x.x.x x.x.x x...x xxxx. x...x xxxx. .xxx. ..x..
        x..x. x.... x...x x..xx x...x x.... x..xx x...x ....x ..x..
        x...x xxxxx x...x x...x .xxx. x.... .xxxx x...x xxxx. ..x..
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
       glyphs %w{U V W X Y Z [ \\ ] ^}, 
        "
        ..... ..... ..... ..... ..... ..... xxxx. x.... .xxxx ..x..
        x...x x...x x...x x...x x...x xxxxx x.... x.... ....x .x.x.
        x...x x...x x...x .x.x. .x.x. ...x. x.... .x... ....x x...x
        x...x x...x x...x ..x.. ..x.. ..x.. x.... ..x.. ....x .....
        x...x .x.x. x.x.x .x.x. ..x.. .x... x.... ...x. ....x .....
        .xxx. ..x.. .x.x. x...x ..x.. xxxxx x.... ....x ....x .....
        ..... ..... ..... ..... ..... ..... x.... ....x ....x .....
        ..... ..... ..... ..... ..... ..... xxxx. ....x .xxxx .....
        "
        
        glyphs %w{a b c d e f g h i j}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... x.... ..... ....x ..... ...xx ..... x.... ..x.. ...x.
        .xxxx xxxx. .xxxx .xxxx .xxx. ..x.. .xxxx xxxx. ..... .....
        x...x x...x x.... x...x x..xx ..x.. x...x x...x ..x.. ...x.
        x...x x...x x.... x...x xxx.. xxxxx x...x x...x ..x.. ...x.
        .xxxx xxxx. .xxxx .xxxx .xxxx ..x.. .xxxx x...x ..x.. ...x.
        ..... ..... ..... ..... ..... ..x.. ....x ..... ..... x..x.
        ..... ..... ..... ..... ..... ..x.. xxxx. ..... ..... .xx..
        "
        glyphs %w{k l m n o p q r s t}, 
        "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        x.... ..x.. ..... ..... ..... ..... ..... ..... ..... .x...
        x..x. ..x.. xx.x. xxxx. .xxx. xxxx. .xxxx xxxx. .xxx. xxxxx
        xxx.. ..x.. x.x.x x...x x...x x...x x...x x...x xx... .x...
        x..x. ..x.. x.x.x x...x x...x x...x x...x x.... ..xxx .x..x
        x...x ..x.. x.x.x x...x .xxx. xxxx. .xxxx x.... xxxx. ..xx.
        ..... ...x. ..... ..... ..... x.... ....x ..... ..... .....
        ..... ..... ..... ..... ..... x.... ....x ..... ..... .....
        "
        
        glyphs %w{u v w x y z { | } ~}, 
        "
        ..... ..... ..... ..... ..... ..... ...xx ..x.. xx... .....
        ..... ..... ..... ..... ..... ..... ..x.. ..x.. ..x.. .....
        x...x x...x x...x x..x. x...x xxxxx ..x.. ..x.. ..x.. .....
        x...x x...x x.x.x .xx.. x...x ...x. xx... ..x.. ...xx .x.x.
        x...x .x.x. x.x.x .xx.. x...x ..x.. xx... ..x.. ...xx x.x..
        .xxxx ..x.. .x.x. x..x. .xxxx xxxxx ..x.. ..x.. ..x.. .....
        ..... ..... ..... ..... ....x ..... ..x.. ..x.. ..x.. .....
        ..... ..... ..... ..... xxxx. ..... ...xx ..x.. xx... .....
        "
        
        # Some utf8 encoded accented letters
        glyphs %w{ä ë ï ö ü ÿ Ä Ë Ï Ö Ü Ÿ}, 
        "
        .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x.
        ..... ..... ..... ..... ..... ..... ..x.. xxxxx xxxxx .xxx. x...x x...x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        glyphs %w{á é í ó ú ý Á É Í Ó Ú Ý}, 
        "
        ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x. ...x.
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. xxxxx xxxxx .xxx. x.x.x x.x.x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
        
        glyphs %w{à è ì ò ù ỳ À È Ò Ù Ỳ}, 
        "
        .x... .x... .x... .x... .x... .x... .x... .x... .x... .x... .x... .x...
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. xxxxx xxxxx .xxx. x.x.x x.x.x
        .xxxx .xxx. ..... .xxx. x...x x...x .x.x. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "

        glyphs %w{â ê î ô û ŷ Â Ê Î Ô Û Ŷ}, 
        "
        ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x.. ..x..
        .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. .x.x. xxxxx xxxxx .xxx. xx.xx xx.xx
        .xxxx .xxx. ..... .xxx. x...x x...x .xxx. x.... ..x.. x...x x...x .x.x.
        x...x x..xx ..x.. x...x x...x x...x x...x xxxxx ..x.. x...x x...x ..x..
        x...x xxx.. ..x.. x...x x...x x...x xxxxx x.... ..x.. x...x x...x ..x..
        .xxxx .xxxx ..x.. .xxx. .xxxx .xxxx x...x xxxxx xxxxx .xxx. .xxx. ..x..
        ..... ..... ..... ..... ..... ....x ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... .xxx. ..... ..... ..... ..... ..... .....
        "
                
        
        
=begin
         "
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        ..... ..... ..... ..... ..... ..... ..... ..... ..... .....
        "
=end
        
    end 
  end
end