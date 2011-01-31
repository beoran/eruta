module Gari
  # Valid key mods (possibly OR'd together)
  module Mod 
    MOD_NONE      = 0x0000
    MOD_LSHIFT    = 0x0001
    MOD_RSHIFT    = 0x0002
    MOD_LCTRL     = 0x0040
    MOD_RCTRL     = 0x0080
    MOD_LALT      = 0x0100
    MOD_RALT      = 0x0200
    MOD_LMETA     = 0x0400
    MOD_RMETA     = 0x0800
    MOD_NUM       = 0x1000
    MOD_CAPS      = 0x2000
    MOD_MODE      = 0x4000
    MOD_RESERVED  = 0x8000
    
    
    # Gets a symbol with the name of the key modifier
    # XXX: doesn't work for combined values 
    def self.mod2sym(key)
      return @key2sym[key] if @key2sym 
      @key2sym = {} 
      # use a bit of metaprograming to generate this hash 
      self.constants.each  do |c|
        sym = c.to_s.gsub(/\AMOD_/, '').downcase.to_sym
        val = self.const_get(c)
        @key2sym[val] = sym
      end
       
      return @key2sym[key]  
    end
    
    # Gets the numerical key value for the symbol 
    # XXX: doesn't work for combined values
    def self.sym2mod(sym)
      cname = "MOD_#{sym.upcase}".to_sym
      return self.const_get(cname)
    end
  end
end