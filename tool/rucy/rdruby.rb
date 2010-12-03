
# An immutable tree
class Tree
  attr_reader :parent
  attr_reader :children
  attr_reader :data
  
  def initialize(data, children = [], parent = nil)
    @parent   = parent
    @children = children
    @data     = data
  end

  def new_child(data)
    retrun Tree.new(data, [], self)
  end
  
  def add(data)
    child = new_child(data)
    return add_child(data)
  end
  
  def add_child(child)
    return Tree.new(@data, @children + child, @parent)
  end
  
  def each
    children.each do  |child| yield child end
  end
  
end


class RParser

  # Lexical tokens. All lexical tokens are terminal tokens.
  
  
  class Token
    attr_reader :name
    def initialize(name)
      @name     = name
      @regexp   = Regexp.new('\\A' + regexp.source) 
      @value    = value
    end
  end 
  
  class Terminal < Token
    def initialize(name, regexp, value, start=0, stop=0, lineno=0)
      super name
      @regexp   = Regexp.new('\\A' + regexp.source) 
      @value    = value
    end
    
    def match(input)
      return @regexp.match(input)
    end
    
  end
  
  class Rule < Token
    def initialize(name)
      super name
    end
    
    def match(input)
    end
  end
  
  class Rule
    attr_reader :name
    attr_reader :tokens
    
    def initialize(name, *tokens)
      @name     = name.to_s
      @tokens   = [] || tokens
    end   
  end



end


if $0== __FILE__




end







# class Rubytoken 
#   def initialize(type, data)
#     @type = type
#     @data = data
#   end
# end
# 
# Rubyparser = RParser.new do
#    token(/\s+/) { |m| Rubytoken.new(:whitespace, m)  }
#    token(/end/)
#    token(/else/)
#    token(/case/)
#    token(/ensure/)
#    token(/module/)
#    token(/elsif/)
#    token(/def/)
#    token(/rescue/)
#    token(/not/)
#    token(/then/)
#    token(/yield/)
#    token(/for/)
#    token(/self/)
#    token(/false/)
#    token(/retry/)
#    token(/return/)
#    token(/true/)
#    token(/if/)
#    token(/defined?/)
#    token(/super/)
#    token(/undef/)
#    token(/break/)
#    token(/in/)
#    token(/nil/)
#    token(/until/)
#    token(/unless/)
#    token(/or/)
#    token(/next/)
#    token(/when/)
#    token(/redo/)
#    token(/and/)
#    token(/begin/)
#    token(/__LINE__/)
#    token(/class/)
#    token(/__FILE__/)
#    token(/END/)
#    token(/BEGIN/)
#    token(/while/)
#    token(/alias/)
#    
#    token(/\d[\d\_]+/)                       { |m| Rubytoken.new(:integer, m)  }
#    token(/\d[\d\_]+(\.\d+)?([eE]-?[\d])?/)  { |m| Rubytoken.new(:float, m)    }
#    token(/./)                               { |m| Rubytoken.new(:unknown, m)  }
# 
#    start :statements do
#      match(:statement, :statements ) { |a, b| Rubytoken.new(:statements, [a,b]) }
#    end
#    
#    rule :statement do
#     match() { |t| Rubytoken.new(:statement, t) }
#    end
#    
# end
# 
# program = %q{10}
# 
# 
# p Rubyparser.parse(program)