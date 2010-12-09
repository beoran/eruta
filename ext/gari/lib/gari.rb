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
  
  
  class Event
    KIND_TO_TYPE = { NONE     => :none        , ACTIVE       => :active,
                   KEYDOWN    => :keydown     , KEYUP        => :keyup,
                   MOUSEPRESS => :mousepress  , MOUSERELEASE => :mouserelease,
                   MOUSEMOVE  => :mousemove   , MOUSESCROLL  => :mousescroll,
                   JOYMOVE    => :joymove     , JOYPRESS     => :joypress,
                   JOYRELEASE => :joyrelease  , RESIZE       => :resize,
                   EXPOSE     => :expose      , QUIT         => :quit,
                   USER       => :user        , SYSTEM       => :system,
                  }
    # returns the type of this event, as a symbol  
    def type
      return KIND_TO_TYPE[self.kind]
    end
    
    # returns the key press unicode value as a text
    def text
      [self.unicode].pack("U*").force_encoding("utf-8")
    end
    
    # This "macro" below ensures that instances of the Event class get 
    # metods like quit?, active?, keyup?, keydown?, etc, etc, based 
    # on the table above 
    KIND_TO_TYPE.each do |v, k|
      mname = (k.to_s + "?").to_sym 
      define_method(mname) do
        return self.kind == v 
      end
    end
    
    
  end

end










