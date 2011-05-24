
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
    
    if data.is_a? Array
      data.reject { |d| d.nil? }
      if data.first.is_a? Symbol
        key = data.shift
        res[key] = {}
        data.each do |val|
          aid = self.to_hash(val)
          res[key] = aid if aid 
        end
        res = res.first while res.is_a?(Array) && res.size < 1 
        # simplify a bit 
        return res
      else
        key   = :_ 
        res[key] = {}
        data.each do |val|
          aid = self.to_hash(val) 
          res[key] = aid if aid
        end
        res = res.first while  res.is_a?(Array) &&  res.size < 1 
        # simplify a bit 
        return res 
      end
    else 
      return data 
    end  
  end
  
  
  
end
