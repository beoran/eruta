
# Raku is a data export and scripting language, a bit like TCL or JSON, 
# but better, since it is parsable qwiin O(n) time with a predictive parser. 
module Raku
  autoload :Fail  , 'raku/fail'
  autoload :Lexer , 'raku/lexer'
  autoload :LL    , 'raku/ll'
  autoload :Node  , 'raku/node'
  autoload :Parser, 'raku/parser'
  autoload :Rule  , 'raku/rule'
  autoload :Token , 'raku/token'
  
  # Trasformed parsed S-expressions to a hash for easier lookup 
  def self.to_hash(data)
    res ||= {}
    key   = nil
    if data.is_a?(Array)
      if data.first.is_a?(Symbol)
        key = data.shift
        return { key => to_hash(data) } 
      else 
        return data.map do | e | 
           self.to_hash(e)
        end
      end
    else 
      return data 
    end  
  end
  
  
  
end
