module Gari
 
  # Extra functionality for Gari events.  
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
    
    # Returns the key press unicode value as a text
    def text
      aid = [self.unicode].pack("U*")
      # for Rubinius
      return aid.force_encoding("utf-8") if aid.respond_to?(:force_encoding)      
      return aid
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
    
    # Returns the key as a symbol    
    def keysym
      Gari::Key.key2sym(self.key) 
    end
    
    # Returns the modifier as a symbol 
    def modsym
      Gari::Mod.mod2sym(self.key) 
    end
  end # Event
end # Gari
