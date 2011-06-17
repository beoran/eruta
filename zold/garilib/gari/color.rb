module Gari
  class Color
     def <=>(other)
       res = self.a <=> other.a
       return res if res != 0
       res = self.r <=> other.r
       return res if res != 0
       res = self.g <=> other.g
       return res if res != 0
       res = self.b <=> other.b
       return res
     end
     
     def ==(other)
      self.a == other.a && self.r == other.r && 
      self.g == other.g && self.b == other.b 
     end
     
     def to_s
      "<Gari::Color r:#{self.r} g:#{self.g} b:#{self.b} a:#{self.a}"
     end
     
     def inspect
      to_s
     end
     
     White = self.rgb(255, 255, 255)
     Green = self.rgb(  0, 255,   0)
     Red   = self.rgb(255,   0,   0)
     Blue  = self.rgb(  0,   0, 255)
  end
end # module Gari