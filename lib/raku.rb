
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
  
  
  def self.block_to_hash(block)
    for stat in substats do
      
    end
  end
  
  
  def self.statement_to_hash(line)
    return nil, nil   unless line
    unless line.is_a? Array
      return line
    end
    if line.first.is_a?(Array)
      return program_to_hash(line)
    end    
    command          = line.shift 
    res              = {}
    params           = [] 
    for arg in line
      if arg.is_a?(Array) # sub array means block parameter
        # this will be an array of substatements
        substats = program_to_hash(arg)
        res[:a] = substats
      else # normal parameter
        params << arg
      end 
    end
    res[:_] = params if params && params.size > 0
    return res, command
  end
  
  def self.program_to_hash(data)
    res = []
    for line in data
      h, command = statement_to_hash(line)
      res << { command => h } if h
    end
    return res
  end
  
  
  
  
  # Trasformed parsed S-expressions to a hash for easier lookup 
  def self.to_hash(data)
    return program_to_hash(data.dup)
  end
  
  # Trasformed parsed S-expressions to a Node tree for easier lookup 
  def self.to_node(data)
    return Node.program_to_node(data.dup)
  end
  
  # Calls Raku::Parser.parse followed by sel.to_node
  # Returns the parse tree and nil if ok, nil and an error if parse error.
  def self.parse_to_node(text)
    res, err = Raku::Parser.parse(text)
    return nil, err if err    
    return Node.program_to_node(res), nil
  end
  
  
end
