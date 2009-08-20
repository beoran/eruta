#
# Mostly RGB(A) based model for a color, for use with low level SDL functions
# 

module Sisa

  class Color
    attr_reader :r
    attr_reader :g
    attr_reader :b
    attr_reader :a
    
    
    def self.parse(rgb)
       # TODO
    end    
    
    # Allocates a new color with the given RGB values.
    def initialize(r_in, g_in, b_in, a_in = nil)
      @r        = r_in
      @g        = g_in
      @b        = b_in
      @a        = a_in
    end
    
    # Convert to an array with the respecive r, g, and b  values 
    # Also includes and a if a is not nil.
    def to_a
      if @a 
        return [@r, @g, @b, @a]
      else 
        return [@r, @g, @b]
      end 
    end
    
    # Makes a random dark color 
    def self.random_dark
      return self.new(rand(128),rand(128),rand(128))
    end
    
    # Comparisons with other colors
    def ==(other)      
      return self.to_a == other.to_a  
    end
    
    TRANSPARENT       = Color.new(255, 0, 255, SDL::ALPHA_TRANSPARENT)
    ALPHA_TRANSPARENT = SDL::ALPHA_TRANSPARENT
    ALPHA_OPAQUE      = SDL::ALPHA_OPAQUE
    
    # Color for animated tile info tile (it has a purplish background)
    TILE_ANIMATION    = Color.new(0xaa, 0x11, 0xee, SDL::ALPHA_OPAQUE)
    # Animation program is esxpressed as colored pixels. White means, next tile.
    ANIMATION_NEXT    = Color.new(0xff, 0xff, 0xff, SDL::ALPHA_OPAQUE)
    # Black means, rewind animation to first tile.
    ANIMATION_REWIND  = Color.new(0x00, 0x00, 0x00, SDL::ALPHA_OPAQUE)
    
    # Css named colors , except for LightGoldenRodYellow, 
    # which is abbreviated as LightGoldenrod
    
    ShadowGray        = Color.new( 64,  64,  64)
    AliceBlue         = Color.new(240, 248, 255)
    AntiqueWhite      = Color.new(250, 235, 215)
    Aqua              = Color.new(0,   255, 255)
    Aquamarine        = Color.new(127, 255, 212)
    Azure             = Color.new(240, 255, 255)
    Beige             = Color.new(245, 245, 220)
    Bisque            = Color.new(255, 228, 196)
    Black             = Color.new(  0,   0,   0)
    BlanchedAlmond    = Color.new(255, 235, 205)
    Blue              = Color.new(  0,   0, 255)
    BlueViolet        = Color.new(138,  43, 226)
    Brown             = Color.new(165,  42,  42)
    BurlyWood         = Color.new(222, 184, 135)
    CadetBlue         = Color.new( 95, 158, 160)
    Chartreuse        = Color.new(127, 255,   0)
    Chocolate         = Color.new(210, 105,  30)
    Coral             = Color.new(255, 127,  80)
    CornflowerBlue    = Color.new(100, 149, 237)
    Cornsilk          = Color.new(255, 248, 220)
    Crimson           = Color.new(237, 164,  61)
    Cyan              = Color.new(  0, 255, 255)
    DarkBlue          = Color.new(  0,   0, 139)
    DarkCyan          = Color.new(  0, 139, 139)
    DarkGoldenRod     = Color.new(184, 134,  11)
    DarkGray          = Color.new(169, 169, 169)
    DarkGrey          = Color.new(169, 169, 169)    
    DarkGreen         = Color.new(  0, 100,   0)
    DarkKhaki         = Color.new(189, 183, 107)
    DarkMagenta       = Color.new(139,   0, 139)
    DarkOliveGreen    = Color.new(85,  107,  47)
    DarkOrange        = Color.new(255, 140,   0)
    DarkOrchid        = Color.new(153,  50, 204)
    DarkRed           = Color.new(139,   0,   0)
    DarkSalmon        = Color.new(233, 150, 122)
    DarkSeaGreen      = Color.new(143, 188, 143)
    DarkSlateBlue     = Color.new( 72,  61, 139)
    DarkSlateGray     = Color.new( 47,  79,  79)
    DarkSlateGrey     = Color.new( 47,  79,  79)
    DarkTurquoise     = Color.new(  0, 206, 209)
    DarkViolet        = Color.new(148,   0, 211)
    DeepPink          = Color.new(255,  20, 147)
    DeepSkyBlue       = Color.new(  0, 191, 255)
    DimGray           = Color.new(105, 105, 105)
    DimGrey           = Color.new(105, 105, 105)
    DodgerBlue        = Color.new( 30, 144, 255)
    FireBrick         = Color.new(178,  34,  34)
    FloralWhite       = Color.new(255, 250, 240)
    ForestGreen       = Color.new( 34, 139,  34)
    Fuchsia           = Color.new(255,   0, 255)
    Gainsboro         = Color.new(220, 220, 220)
    GhostWhite        = Color.new(248, 248, 255)
    Gold              = Color.new(255, 215,   0)
    GoldenRod         = Color.new(218, 165,  32)
    Gray              = Color.new(128, 128, 128)
    Grey              = Color.new(128, 128, 128)
    Green             = Color.new(  0, 128,   0)
    GreenYellow       = Color.new(173, 255,  47)
    HoneyDew          = Color.new(240, 255, 240)
    HotPink           = Color.new(255, 105, 180)
    IndianRed         = Color.new(205,  92,  92)
    Indigo            = Color.new(75 ,   0, 130)
    Ivory             = Color.new(255, 255, 240)
    Khaki             = Color.new(240, 230, 140)
    Lavender          = Color.new(230, 230, 250)
    LavenderBlush     = Color.new(255, 240, 245)
    LawnGreen         = Color.new(124, 252,   0)
    LemonChiffon      = Color.new(255, 250, 205)
    LightBlue         = Color.new(173, 216, 230)
    LightCoral        = Color.new(240, 128, 128)
    LightCyan         = Color.new(224, 255, 255)
    LightGoldenRod    = Color.new(250, 250, 210)
    LightGray         = Color.new(211, 211, 211)
    LightGrey         = Color.new(211, 211, 211)
    LightGreen        = Color.new(144, 238, 144)
    LightPink         = Color.new(255, 182, 193)
    LightSalmon       = Color.new(255, 160, 122)
    LightSeaGreen     = Color.new( 32, 178, 170)
    LightSkyBlue      = Color.new(135, 206, 250)
    LightSlateGray    = Color.new(119, 136, 153)
    LightSlateGrey    = Color.new(119, 136, 153)
    LightSteelBlue    = Color.new(176, 196, 222)
    LightYellow       = Color.new(255, 255, 224)
    Lime              = Color.new(  0, 255,   0)
    LimeGreen         = Color.new( 50, 205,  50)
    Linen             = Color.new(250, 240, 230)
    Magenta           = Color.new(255,   0, 255)
    Maroon            = Color.new(128,   0,   0)
    MediumAquaMarine  = Color.new(102, 205, 170)
    MediumBlue        = Color.new(  0,   0, 205)
    MediumOrchid      = Color.new(186,  85, 211)
    MediumPurple      = Color.new(147, 112, 219)
    MediumSeaGreen    = Color.new( 60, 179, 113)
    MediumSlateBlue   = Color.new(123, 104, 238)
    MediumSpringGreen = Color.new(  0, 250, 154)
    MediumTurquoise   = Color.new( 72, 209, 204)
    MediumVioletRed   = Color.new(199,  21, 133)
    MidnightBlue      = Color.new( 25,  25, 112)
    MintCream         = Color.new(245, 255, 250)
    MistyRose         = Color.new(255, 228, 225)
    Moccasin          = Color.new(255, 228, 181)
    NavajoWhite       = Color.new(255, 222, 173)
    Navy              = Color.new(  0,   0, 128)
    OldLace           = Color.new(253, 245, 230)
    Olive             = Color.new(128, 128,   0)
    OliveDrab         = Color.new(107, 142,  35)
    Orange            = Color.new(255, 165,   0)
    OrangeRed         = Color.new(255,  69,   0)
    Orchid            = Color.new(218, 112, 214)
    PaleGoldenRod     = Color.new(238, 232, 170)
    PaleGreen         = Color.new(152, 251, 152)
    PaleTurquoise     = Color.new(175, 238, 238)
    PaleVioletRed     = Color.new(219, 112, 147)
    PapayaWhip        = Color.new(255, 239, 213)
    PeachPuff         = Color.new(255, 218, 185)
    Peru              = Color.new(205, 133,  63)
    Pink              = Color.new(255, 192, 203)
    Plum              = Color.new(221, 160, 221)
    PowderBlue        = Color.new(176, 224, 230)
    Purple            = Color.new(128,   0, 128)
    Red               = Color.new(255,   0,   0)
    RosyBrown         = Color.new(188, 143, 143)
    RoyalBlue         = Color.new( 65, 105, 225)
    SaddleBrown       = Color.new(139, 69,   19)
    Salmon            = Color.new(250, 128, 114)
    SandyBrown        = Color.new(244, 164,  96)
    SeaGreen          = Color.new( 46, 139,  87)
    SeaShell          = Color.new(255, 245, 238)
    Sienna            = Color.new(160,  82,  45)
    Silver            = Color.new(192, 192, 192)
    SkyBlue           = Color.new(135, 206, 235)
    SlateBlue         = Color.new(106,  90, 205)
    SlateGray         = Color.new(112, 128, 144)
    SlateGrey         = Color.new(112, 128, 144)
    Snow              = Color.new(255, 250, 250)
    SpringGreen       = Color.new(  0, 255, 127)
    SteelBlue         = Color.new( 70, 130, 180)
    Tan               = Color.new(210, 180, 140)
    Teal              = Color.new(  0, 128, 128)
    Thistle           = Color.new(216, 191, 216)
    Tomato            = Color.new(255,  99, 71)
    Turquoise         = Color.new( 64, 224, 208)
    Violet            = Color.new(238, 130, 238)
    Wheat             = Color.new(245, 222, 179)
    White             = Color.new(255, 255, 255)
    WhiteSmoke        = Color.new(245, 245, 245)
    Yellow            = Color.new(255, 255, 0)
    YellowGreen       = Color.new(154, 205, 50)
    PaleGoldenWhisp   = Color.new(238, 232, 170, 128)

  end  
end  
