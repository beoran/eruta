require 'rubygems'


module Rubygame

end

# Module for modeling colors. Forward declaration.
module Rubygame::Color

end


# Abstract parent class for the color model classes. Contains useful helpers and operators.
 
class Rubygame::Color::Model
  
  # Class method that creates a new instance of this color model from an object of the 
  # standard Rubygame::Color::Rgb model of that color
  def self.from_rgb(rgb)
    raise "Abstract base function called"
    # This is an abstract base function.
  end
  
  # Convents a color from a different model to this model, passing over RGB
  def self.from_model(other)
    return self.from_rgb(other.to_rgb)
  end
  
  # Class method that creates a new instance of this color model from an 
  # array containing the corresponding r, g, b values in that order
  # Does exactly the same as return self.from_rgb(Rubygame::Color::Rgb.new(a[0], a[1], a[2])) 
  def self.from_a(a)
    return self.from_rgb(Rubygame::Color::Rgb.new(a[0], a[1], a[2])) 
  end
  
  # Returns this color converted to the color model +model+
  def to_model(target_model)
    return target_model.from_rgb(self.to_rgb)
  end
  
  # Returns the color other converted to the current color model
  def to_my_model(other)
    return other.to_model(self.class)
  end
  
  # Converts the color in this model object to the standard Rubygame::Color::Rgb model
  def to_rgb()
    return self.to_model(Rubygame::Color::Rgb)
  end
  
  def to_rgbn
    return self.to_model(Rubygame::Color::RgbNormalized)
  end
  
  def to_hsv
    return self.to_model(Rubygame::Color::Hsv)
  end
  
  def to_hsl
    return self.to_model(Rubygame::Color::Hsl)
  end
  
  def to_hsi
    return self.to_model(Rubygame::Color::Hsi)
  end
  
  def to_hsp
    return self.to_model(Rubygame::Color::Hsp)
  end
  
  def to_icy
    return self.to_model(Rubygame::Color::Icy)
  end
    
  # Returns the color as an array containing the corresponding r, g, b values in that order
  # Does exactly thesame as colormodel.to_rgb.to_a
  def to_a
    self.to_rgb.to_a
  end  
  
  def to_ary
    self.to_a
  end  
  
  #
  # Compares colors to see if they are equal. To do this, the color model will be 
  # converted to Rgb and then compared.
  #
  def ==(other)
    self.to_rgb == other.to_rgb
  end

  # Additive color mixing. To do this, the color model will be 
  # converted to Rgbn and then added to each other. The result is a color with
  # the same model as the first operand.
  def +(other)
    self.to_my_model(self.to_rgbn + other.to_rgbn)
  end
  
  # Additive color mixing. To do this, the color model will be 
  # converted to Rbgn and then added to each other. The result is a color with
  # the same model as the first operand.
  # Substractive color mixing
  def -(other)
    self.to_my_model(self.to_rgbn - other.to_rgbn)
  end  
  
  #
  # The opposite of this color. This is defined as White - self
  #
  def -@
    return Rubygame::Color::White - self
  end 
  
  # Multiplicative color mixing. To do this, the color model will be 
  # converted to Rgbn and then mutiplied to each other. The result is a color with
  # the same model as the first operand.
  def *(other)
    self.to_my_model(self.to_rgbn * other.to_rgbn)
  end
  
  # Dibisive color mixing. To do this, the color model will be 
  # converted to Rgbn and then mutiplied to each other. The result is a color with
  # the same model as the first operand.
  def / (other)
    self.to_my_model(self.to_rgbn / other.to_rgbn)
  end
  
end



# RGB color model is the default color model, which models colors with 
# their red green and blue components, each value ranging from 0 to 255 as is 
# used by as is used by SDL.
class Rubygame::Color::Rgb < Rubygame::Color::Model
   
  attr_reader :r
  attr_reader :g
  attr_reader :b
  
  RMAX = 255
  BMAX = 255
  GMAX = 255 
  
  def initialize(r, g, b)
    @r = r.round
    @g = g.round
    @b = b.round
  end  

  def self.from_rgb(rgbmodel)
    return rgbmodel
    # No conversion needed
  end

  def to_rgb
    return Rubygame::Color::Rgb.new(@r,@g,@b)
    # no need for conversion, so just return self ,since all classes are immutable
  end
  
  def to_a
    return [r, g, b]
  end
  
  def ==(other)
    other_rgb = other.to_rgb
    return (@r == other_rgb.r) && (@g == other_rgb.g) && (@b == other_rgb.b)
  end
  
  def ===(other)
    other_rgb = other.to_rgb
    return (@r == other_rgb.r) && (@g == other_rgb.g) && (@b == other_rgb.b)
  end
  
  
   def <=>(other)
    dr = (other.r<=>@r)
    dg = (other.g<=>@g)
    db = (other.b<=>@b)
    return dr unless dr == 0
    return dg unless dg == 0
    return db unless db == 0
    return 0
  end
  
  
end

# Normalized RGB color model is a color model related to RGB, which models colors with 
# their red green and blue components, but with each value ranging from 0 to 1 
# this class is useful for calculation conversions to the RGB color model, and for doing 
# mathematics with colors.

class Rubygame::Color::RgbNormalized < Rubygame::Color::Model
   
  attr_reader :rn
  attr_reader :gn
  attr_reader :bn
  
  RMAX = 255
  BMAX = 255
  GMAX = 255 
  
  def initialize(rn, gn, bn)
    rn, gn, bn = clamp(rn, gn, bn)
    @rn = rn
    @gn = gn
    @bn = bn
  end  
  
  def clamp(rn, gn, bn)
    rn        = 1.0 if rn > 1.0
    gn        = 1.0 if gn > 1.0
    bn        = 1.0 if bn > 1.0
    rn        = 0.0 if rn < 0.0
    gn        = 0.0 if gn < 0.0
    bn        = 0.0 if bn < 0.0    
    return rn, gn, bn
  end
  
  def self.from_rgb(rgbmodel)
    rn = rgbmodel.r / 255.0
    gn = rgbmodel.g / 255.0
    bn = rgbmodel.b / 255.0
    return  Rubygame::Color::RgbNormalized.new(rn, gn, bn)
  end

  def to_rgb
    r = (@rn * 255.0).round
    g = (@gn * 255.0).round
    b = (@bn * 255.0).round
    return Rubygame::Color::Rgb.new(r, g, b)    
  end

  def +(other)
    other_rgbn  = other.to_rgbn
    rn          = @rn + other_rgbn.rn 
    gn          = @gn + other_rgbn.gn
    bn          = @bn + other_rgbn.bn
    rn, gn, bn  = clamp(rn, gn, bn)
    return Rubygame::Color::RgbNormalized.new(rn, gn ,bn)
  end
  
  def -(other)
    other_rgbn  = other.to_rgbn
    rn          = @rn - other_rgbn.rn 
    gn          = @gn - other_rgbn.gn
    bn          = @bn - other_rgbn.bn
    rn, gn, bn  = clamp(rn, gn, bn)
    return Rubygame::Color::RgbNormalized.new(rn, gn ,bn)
  end

  def *(other)
    other_rgbn = other.to_rgbn
    rn         = @rn + other_rgbn.rn 
    gn         = @gn + other_rgbn.gn
    bn         = @bn + other_rgbn.bn
    rn, gn, bn = clamp(rn, gn, bn)
    return Rubygame::Color::RgbNormalized.new(rn,gn,bn)
  end
  
  private
  def divide_component(cn, cdiv)
    return 1.0 if cdiv == 0.0 
    return cn / cdiv
  end
  
  public
  def / (other)
    other_rgbn = other.to_rgbn
    rn         = divide_component(@rn, other_rgbn.rn)
    gn         = divide_component(@gn, other_rgbn.gn)
    bn         = divide_component(@bn, other_rgbn.bn)    
    rn, gn, bn  = clamp(rn, gn, bn)
    return Rubygame::Color::RgbNormalized.new(rn,gn,bn)
  end
   
  def <=>(other)
    myl    = @rn+@gn+@bn
    otherl = other.rn+other.gn+other.bn
    return ( myl <=> otherl)
  end

end

# The Hue Saturation Value model is convenient for a more natural representation of colors
class Rubygame::Color::Hsv < Rubygame::Color::Model
  attr_reader :h
  attr_reader :s
  attr_reader :v
  
  def initialize(h, s, v)
    @h = h
    @s = s
    @v = v
  end  

  def self.from_rgb(rgbmodel)
    rgbn = Rubygame::Color::RgbNormalized.from_rgb(rgbmodel)
    rn, gn, bn = rgbn.rn, rgbn.gn, rgbn.bn
    # Normalize the color model, and use some local variables for compactness
    rgbn_arr  = [rn, gn, bn]
    max       = rgbn_arr.max
    min       = rgbn_arr.min
    # Get minimum of of normalised r, g or b    
    # Calculate hue, using the Hsl color model, since Hsv and Hsl hue are the same.
    h = Rubygame::Color::Hsl.calulate_hue(rn, gn, bn, max, min)
    
    # Calculate saturation
    if max == 0.0
      s = 0.0
    else
      s = 1.0 - (min / max)
    end  
    
    # Calulate value. Yeah, that's easy. :)
    v = max
    
    # Finally return the new color
    return Rubygame::Color::Hsv.new(h, s, v)    
  end

  def self.calulate_rgbn(hi, v, p, t, q)
    return v, t, p if hi == 0
    return q, v, p if hi == 1
    return p, v, t if hi == 2
    return p, q, v if hi == 3
    return t, p, v if hi == 4
    return v, p, q if hi == 5
    raise "Should not happen" # We should never get here
  end
  
  def to_rgb
    hi = (@h / 60.0).floor % 6
    # Determine what part of the "color hexagon" the hue is in.
    f  = (@h / 60.0) - hi
    # Fractional part
    p  = @v * (1.0 - @s)
    q  = @v * (1.0 - (f * @s))
    t  = @v * (1.0 - ((1.0 - f) * @s))
    rn, gn, bn = Rubygame::Color::Hsv.calulate_rgbn(hi, @v, p, t, q)
    # Finally calulate the normalized r, b and g values
    rgbn = Rubygame::Color::RgbNormalized.new(rn, gn, bn)
    # Make a normalized RGB    
    return rgbn.to_rgb
    # And convert it to plain rgb        
  end
  
end

# The Hue Saturation Lightness model is convenient for a more natural representation of colors
# It has a certain advantage over Hsv in that it models the concept of "brightness" of a color
# more naturally.
class Rubygame::Color::Hsl < Rubygame::Color::Model
  attr_reader :h
  attr_reader :s
  attr_reader :l
  
  def initialize(h, s, l)
    @h = h
    @s = s
    @l = l
  end  
  
  def self.calulate_hue(rn, gn, bn, max, min)
    if min == max 
      h = 0 
    # Undefined in this case, but set it to zero
    elsif max == rn and gn >= bn
      h = (60 * (gn - bn) / (max - min)) + 0
    elsif max == rn and gn < bn   
      h = (60 * (gn - bn) / (max - min)) + 360
    elsif max == gn
      h = (60 * (bn - rn) / (max - min)) + 120
    elsif max == bn
      h = (60 * (rn - gn) / (max - min)) + 240
    else 
      raise "Should never happen"
    end 
    return h
  end
  
  def self.from_rgb(rgbmodel)
    rgbn = Rubygame::Color::RgbNormalized.from_rgb(rgbmodel)
    rn, gn, bn= rgbn.rn, rgbn.gn, rgbn.bn
    # Normalize the color model, and use some local variables for compactness
    rgbn_arr  = [rn, gn, bn]
    max       = rgbn_arr.max
    min       = rgbn_arr.min
    # Get minimum of of normalised r, g or b    
    # Calculate hue
    h = Rubygame::Color::Hsl.calulate_hue(rn, gn, bn, max, min)
    
    # calculate lightness
    l = (max + min) / 2.0
    
    # calulate saturation
    if l == 0.0 or max == min
      s = 0
    elsif 0 < l and l <= 0.5 
      s = (max - min) / (max + min)
    else # l > 0.5
      s = (max - min) / (2 - (max + min))
    end
    # Finally return the new color
    return Rubygame::Color::Hsl.new(h, s, l)    
  end

  def self.clamp_t(tc)
    return tc + 1.0 if tc < 0.0
    return tc - 1.0 if tc > 1.0
    return tc
  end
  
  def self.calculate_component(p , q, tc)
    return p + ((q - p) * 6.0 * tc) if tc < (1.0 / 6.0)
    return q if tc < (1.0 / 2.0)
    return p + ((q - p) * 6.0 * (( 2.0 / 3.0) - tc) ) if tc < (2.0 / 3.0)    
    return p 
  end
  
  def to_rgb
    # If the color is acromatic, return already with the lightness value for all components
    if @s == 0.0
      return Rubygame::Color::Rgb.new(@l, @l, @l)    
    end
    # Otherwise, we have to do the long, hard calculation
    q = (@l < 0.5 ? (@l * (1.0 + @s)) : (@l + @s - ( @l * @s ))) 
    # q helper value
    p = (2.0 * @l) - q
    # p helper value
    hn = @h / 360.0 
    # hue normalized to [0...1) 
    tr = hn + (1.0 / 3.0)
    tg = hn
    tb = hn - (1.0 / 3.0)
    tr = Rubygame::Color::Hsl.clamp_t(tr)
    tg = Rubygame::Color::Hsl.clamp_t(tg)
    tb = Rubygame::Color::Hsl.clamp_t(tb)
    # clamp values
    rn = Rubygame::Color::Hsl.calculate_component(p, q, tr)
    gn = Rubygame::Color::Hsl.calculate_component(p, q, tg)
    bn = Rubygame::Color::Hsl.calculate_component(p, q, tb)
    # Finally calulate the normalised components
    rgbn = Rubygame::Color::RgbNormalized.new(rn, gn, bn)
    # Make a normalized RGB    
    return rgbn.to_rgb
    # And convert it to plain rgb     
  end

  def <=>(other)
    dh = (other.h<=>@h)
    ds = (other.s<=>@s)
    dl = (other.l<=>@l)
    return dh unless dh == 0
    return ds unless ds == 0
    return dl unless dl == 0
    return 0
  end
end


class Rubygame::Color::Hvl < Rubygame::Color::Model
  attr_reader :h
  attr_reader :v
  attr_reader :l
  
  def initialize(h, v, l)
    @h = h
    @v = v
    @l = l
  end  
  
  def self.from_rgb(rgbmodel)
    rgbn = Rubygame::Color::RgbNormalized.from_rgb(rgbmodel)
    rn, gn, bn= rgbn.rn, rgbn.gn, rgbn.bn
    # x, y, z = rn, bn, gn
    


    # Normalize the color model, and use some local variables for compactness
    rgbn_arr  = [rn, gn, bn]
    max       = rgbn_arr.max
    min       = rgbn_arr.min
    # Get minimum of of normalised r, g or b    
    # Calculate hue
    h = Rubygame::Color::Hsl.calulate_hue(rn, gn, bn, max, min)
    
    # calculate lightness
    l = (max + min) / 2.0
    
    # calulate saturation
    if l == 0.0 or max == min
      s = 0
    elsif 0 < l and l <= 0.5 
      s = (max - min) / (max + min)
    else # l > 0.5
      s = (max - min) / (2 - (max + min))
    end
    # Finally return the new color
    return Rubygame::Color::Hsl.new(h, s, l)    
  end

  def self.clamp_t(tc)
    return tc + 1.0 if tc < 0.0
    return tc - 1.0 if tc > 1.0
    return tc
  end
  
  def self.calculate_component(p , q, tc)
    return p + ((q - p) * 6.0 * tc) if tc < (1.0 / 6.0)
    return q if tc < (1.0 / 2.0)
    return p + ((q - p) * 6.0 * (( 2.0 / 3.0) - tc) ) if tc < (2.0 / 3.0)    
    return p 
  end
  
  
  def to_rgb
    # If the color is acromatic, return already with the lightness value for all components
    if @s == 0.0
      return Rubygame::Color::Rgb.new(@l, @l, @l)    
    end
    # Otherwise, we have to do the long, hard calculation
    q = (@l < 0.5 ? (@l * (1.0 + @s)) : (@l + @s - ( @l * @s ))) 
    # q helper value
    p = (2.0 * @l) - q
    # p helper value
    hn = @h / 360.0 
    # hue normalized to [0...1) 
    tr = hn + (1.0 / 3.0)
    tg = hn
    tb = hn - (1.0 / 3.0)
    tr = Rubygame::Color::Hsl.clamp_t(tr)
    tg = Rubygame::Color::Hsl.clamp_t(tg)
    tb = Rubygame::Color::Hsl.clamp_t(tb)
    # clamp values
    rn = Rubygame::Color::Hsl.calculate_component(p, q, tr)
    gn = Rubygame::Color::Hsl.calculate_component(p, q, tg)
    bn = Rubygame::Color::Hsl.calculate_component(p, q, tb)
    # Finally calulate the normalised components
    rgbn = Rubygame::Color::RgbNormalized.new(rn, gn, bn)
    # Make a normalized RGB    
    return rgbn.to_rgb
    # And convert it to plain rgb     
  end

  def <=>(other)
    dl = (other.l<=>@l)
    dh = (other.h<=>@h)
    ds = (other.s<=>@s)
    return dh unless dh == 0
    return ds unless ds == 0
    return dl unless dl == 0
    return 0
  end
end

class Rubygame::Color::Hsi < Rubygame::Color::Model
  attr_reader :h
  attr_reader :s
  attr_reader :i

  RP = 4.0 / 12.0
  GP = 5.0 / 12.0
  BP = 3.0 / 12.0
  
  def initialize(h, s, i)
    @h  = h
    @s  = s
    @i  = i
  end  
  
  def self.calculate_i(rn, gn, bn)
    return Math.sqrt(rn**2 * RP + gn**2 * GP + bn**2 * BP)
  end
  
  # Calculate saturation
  def self.calculate_saturation(min, max)
    if max == 0.0
      return 0.0
    else
      return  1.0 - (min / max)
    end  
  end 
    
  
  def self.from_rgb(rgbmodel)
    rgbn = Rubygame::Color::RgbNormalized.from_rgb(rgbmodel)
    rn, gn, bn= rgbn.rn, rgbn.gn, rgbn.bn

    # Normalize the color model, and use some local variables for compactness
    rgbn_arr  = [rn, gn, bn]
    max       = rgbn_arr.max
    min       = rgbn_arr.min
    # Get minimum of of normalised r, g or b    
    # Calculate hue
    h = Rubygame::Color::Hsl.calulate_hue(rn, gn, bn, max, min)
    
    # calculate intensity
    i = calculate_i(rn, gn, bn)
    s = calculate_saturation(min, max)
    # Finally return the new color
    return Rubygame::Color::Hsi.new(h, s, i)    
  end

  HUE_LOOKUP  = [ :hue_rgb, :hue_grb, :hue_gbr, :hue_grb, :hue_gbr, :hue_bgr, :hue_brg, :hue_rbg ]
  
  # Determine what part of the "color hexagon" the hue is in, and at what fraction.
  def hue_hexant
    hi      = (@h / 60.0).floor % 6 
    f       = ((@h % 360) / 60.0) - hi
    return HUE_LOOKUP[hi], f  
  end
    
  
  def to_rgb
    raise "to_rgb not implemented"
  end

  def <=>(other)
    di = (other.i<=>@i)
    dh = (other.h<=>@h)
    ds = (other.s<=>@s)
    return dh unless dh == 0
    return ds unless ds == 0
    return di unless di == 0
    return 0
  end
end


# The Intensity Cyan Yellow   model models brightbnness in  very accurateway. 
# It is a Spherical coordinate model, with limited angles,
# in which the color cube's black is placed at the ordinal. As such it may generate 
# colors outside of the rgb gamut, but those will be clipped upon conversion.
class Rubygame::Color::Icy < Rubygame::Color::Model
  
  attr_reader :i
  attr_reader :c
  attr_reader :y
  
  # Yellowness and Cyanness must be in the interval 0..90
  def initialize(i, c, y)    
    @i = i
    @c = c
    @y = y 
  end  
  
  def self.radians_to_degrees(radians)
    return (radians / (2 * Math::PI) ) * 360.0
  end
  
  def self.degrees_to_radians(degrees)
    return (degrees / 360.0) * (2 * Math::PI)
  end

  def self.from_rgb(rgbmodel)
    rgbn       = Rubygame::Color::RgbNormalized.from_rgb(rgbmodel)
    rn, gn, bn = rgbn.rn, rgbn.gn, rgbn.bn
    # Normalize the color model, and use some local variables for compactness
    iu         = Math.sqrt(rn**2 + gn**2 +bn**2)
    # Calculate denormalized intensity
    if iu > 0.0
      yr         = Math.atan(gn / iu) 
      # Calculate yellowness angle in radians, using the denormalized intensity    
      cr         = Math.acos(bn / iu)
      # Calculate cyanness angle in radians, using the denormalized intensity    
    else
      yr = 0.0 # Not defined if iu is 0
      cr = 0.0 # Not defined if iu is 0
    end  
    c          = radians_to_degrees(cr)
    y          = radians_to_degrees(yr)    
    # Convert both angles to degrees
    i          = iu / Math.sqrt(3) 
    # Normalize intensity by scaling to 1.
    return self.new(i, c, y)
    # Finally return the new color
  end

  def to_rgb
    iu  = @i * Math.sqrt(3) 
    # Denormalize intensity 
    
    yr  = self.class.degrees_to_radians(@y)
    cr  = self.class.degrees_to_radians(@c)
    # Convert degrees to radians
    rn  = iu * Math.cos(yr) * Math.sin(cr)  
    gn  = iu * Math.sin(yr) * Math.sin(cr)
    bn  = iu * Math.cos(cr)
    # Spherical coordinates to Carthesian.    
    rgbn = Rubygame::Color::RgbNormalized.new(rn, gn, bn)
    # Make a normalized RGB    
    return rgbn.to_rgb
    # And convert it to plain rgb
  end
  
  def <=>(other)
    di = (other.i<=>@i)
    dc = (other.c<=>@c)
    dy = (other.y<=>@y)
    return dy unless dy == 0
    return dc unless dc == 0
    return di unless di == 0
    return 0
  end
  
end


# Module for modelling colors
# 
# Reccomended usage is  include Rubygame::Color 
# You can then use all defined colors, like Black, Gray, etc 
# 

module Rubygame::Color
    
    def rgb(r,g,b)
      return Rubygame::Color::Rgb.new(r,g,b)
    end 
    
    def rgbn(r,g,b)
      return Rubygame::Color::RgbNormalized.new(r,g,b)
    end 
    
    def hsl(h,s,l)
      return Rubygame::Color::Hsl.new(h,s,l)
    end 
    
    def hsv(h,s,v)
      return Rubygame::Color::Hsv.new(h, s, v)
    end 
    
    def icy(i,c,y)
      return Rubygame::Color::Icy.new(i, c, y)
    end 
    
    extend self
    # So we can also use rgb, hsl, hsv, rgbn, and others from our own class.
    
    # Now, we define all the 16 colors of the HTML4 standard
    Black   = rgb(0x00, 0x00, 0x00)  
    Gray    = rgb(0x80, 0x80, 0x80)  
    Maroon  = rgb(0x80, 0x00, 0x00)  
    Olive   = rgb(0x80, 0x80, 0x00)
   	Green   = rgb(0x00, 0x80, 0x00)
    Teal    = rgb(0x00, 0x80, 0x80)
    Navy    = rgb(0x00, 0x00, 0x80) 
    Purple  = rgb(0x80, 0x00, 0x80)    
    Silver  = rgb(0xc0, 0xc0, 0xc0)
    White   = rgb(0xff, 0xff, 0xff)
    Red     = rgb(0xff, 0x00, 0x00) 
    Yellow  = rgb(0xff, 0xff, 0x00)
    Lime    = rgb(0x00, 0xff, 0x00)
    Aqua    = rgb(0x00, 0xff, 0xff)
    Blue    = rgb(0x00, 0x00, 0xff)
    Fuchsia = rgb(0xff, 0x00, 0xff)                 

    
    # And some aliases 
    Cyan    = Aqua
    Magenta = Fuchsia
    Grey    = Gray
end

if $0 == __FILE__

def rgb2lcv(r,g,b)
  return ((r+g+b)/3.0), ((g+b)/2.0), ((r+b)/2.0)
end

def lcy2rgb(l,c,y)
  r = l-c
  b = l-y
  g = y+c-l
  return r,g,b
end



require 'rubygame'

include Rubygame::Color

=begin
perhue = []
h = {}

(0..360).step(15) do |hh|
  (0.0..1.0).step(1.0) do |ss|
    (0.0..1.0).step(0.1) do |ll|
      col = hsl(hh,ss,ll).to_rgb
      hsi = col.to_hsi      
      hround = hsi.h.round
      iround = (hsi.i*100).round
      sround = (hsi.s*100).round
      # if ((iround % 10) == 0) and ((hround % 15) == 0) and ((sround % 100) == 0)
        perhue[hround] ||= []
        perhue[hround][iround] ||= []
        perhue[hround][iround] << [ col, hsi, "H_#{hround}_I_#{iround}_S_#{sround}" ]
        p iround
        p hround
        p col
      # end
    end
  end
end      

p perhue

cols = []

for hround in (0...perhue.size)
  iarr = perhue[hround]
  next unless iarr
  for iround in (0...iarr.size)
    colarr = iarr[iround] 
    next unless colarr
    for col, hsi, name  in colarr      
        cols << [ col , name  ]  # "H_#{hround}_I_#{iround}"
    end  
  end
end

p cols  


palname = 'AA_COLORS_HSI_HUGE'
f = File.open(palname + '.gpl', 'w')
f.puts "GIMP Palette"
f.puts "Name: #{palname}"
f.puts "Columns: 40"
f.puts "#"
cols.each{ |ca| c=ca[0]; n=ca[1] ; f.printf("%#3d %#3d %#3d\t#{n}\n",c.r, c.g, c.b) }
f.close



colsn = []

(0..90).step(15)   do |yy|
  (0..90).step(15) do |cc|
    (0..7).each   do |ii| 
      col = icy(ii / 7.0, cc, yy)
      colsn << col
    end
  end 
end

cols = colsn # .map {|col| col.to_rgb }.sort.uniq

=end  

hues        = [ 0   , 15  , 30  , 45 , 60 , 90 , 120, 150, 180, 195, 210, 240, 270, 300, 330 ]
lfactors    = [ 0.0 , 0.0 , 0.0 , 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0 ]

lumens      = [ 90, 80, 70, 60, 50, 40, 30, 20 , 10 ]


saturations = [ 20 , 35, 55, 95 ] # [ 20 , 35, 55, 75, 95 ]
saturations2= [ 90 , 40 ]
values      = [ 95 , 55, 35, 20 ]  # [ 95 , 75, 55, 35, 20 ] 

columns     = values.size * saturations.size

cols        = []

satmax      = saturations2.max
valmax      = values.max
lummax      = lumens.max

for hh in hues do
  lf = lfactors.shift
  for ss in saturations do
    for vv in values do
      # lladjust = ll + lf 
      # lladjust = 100 if lladjust > 100 
      # colhsv = Rubygame::Color.hsl(hh, ss / 100.0, lladjust / 100.0)
      colhsv = Rubygame::Color.hsv(hh, ss / 100.0, vv / 100.0)
      cols << colhsv  if ss >= satmax or vv >= valmax
    end
  end
end      

palname = 'AA_ERUTA_MINIRAMP'
f = File.open(palname + '.gpl', 'w')
f.puts "GIMP Palette"
f.puts "Name: #{palname}"
f.puts "Columns: #{columns}"
f.puts "#"
cols.each do |chsv| 
  c = chsv.to_rgb  
  f.printf("%#3d %#3d %#3d\tH#{(chsv.h)}_S#{(chsv.s*100).round}_V#{(chsv.v*100).round}\n", c.r, c.g, c.b) 
end
f.close



end
