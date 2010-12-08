# Ruby extensions to the gari library

require 'gari.so'
require 'gari/image'

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
  end

end










