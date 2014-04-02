module Zori
  # Capability stores the capabilities of a widget or Hanao.
  class Capability
    
    # Initializes the capabilities. 
    # Capabilities are normally fixed for every widget
    # so we dontn't need setters.
    def initialize(caps={})
      @caps = { :drag => false, :focus => false }
      @caps = @caps.merge(caps)
    end
    
    def can?(capa)
      return @caps[capa.to_sym]
    end
    
    def drag?
      return do?(:drag)
    end
    
    def focus?
      return do?(:focus)
    end
    
  end
end
