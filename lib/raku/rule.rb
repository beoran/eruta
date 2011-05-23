require 'nanograph' 

module Raku
  class Rule
    
    attr_accessor :name
    
    def self.parse_error(message = "", token = nil)
      m = message
      if token 
        m = "#{token.line}:#{token.col}: " + message + " #{token.kind} "
      else
        
      end
      raise m 
      return Node.error(m)
    end 
    
    def initialize(name, match = nil)
      @name  = name.to_sym
      @match = match || name.to_sym
      # loop detection
      @loop  = false
      @edge_color = :black
    end
    
    def each
      yield nil
    end
   
   def to_graph_node(graph)
      mynode = graph.node(self.object_id, "#{@name}")
      self.each do |child|
        if child
          childnode = child.to_graph_node(graph)
          mynode.edge(childnode, true, :color => @edge_color)
        end
      end
      return mynode 
   end
   
   # converts the node to a nanograph graph for visualisation
   def to_graph
     graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
                                  rankdir: "LR")
      graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, 
                        style: :rounded }
      to_graph_node(graph) 
      return graph
    end
    
    def | (other)
      return Choice.new("#{self.name}|\n#{other.name}", self, other)
    end
    
    def >> (other)
      return List.new("#{self.name}>>\n#{other.name}", self, other)
    end
    
    def first_set
      set = [ self.name.to_sym ] 
    end
    
    def parse(tokens, parent = nil)
      token  = tokens.first
      return Rule.parse_error("Lex error", token) if token.fail?
      return Rule.parse_error("Unexpected end of file.") unless token
      if token.kind == @match
        tokens.shift # on to next token
        return Node.new(self.name, token.value, parent)
      else 
        return nil
      end
    end
    
  end
  
  class Empty < Rule
    
    def first_set
      set = [ :empty ]
    end
    
    def parse(tokens, parent)
      return Node.new(self.name, nil, parent)
    end
  end
  
  # Links to another (perhaps nonexisting) rule
  class Link < Rule
    attr_accessor :rule
    def initialize(name, rule = nil)
      super(name)
      @rule = rule      
    end
    
    def each
      # loop detection, links should be the only loop-creating artefacts
      if @loop
        # warn "Loop detected"
        yield nil
      else   
        @loop = true 
        yield @rule
        @loop = false
      end
    end
    
    def first_set
      return @rule.first_set
    end
   
    def parse(tokens, parent = nil)
      if tokens.empty? # || tokens.first.kind == :eof
        return Rule.parse_error("Unexpected end of file")
      end   
      parnode = Node.new(self.name, :link, parent)
      res     = @rule.parse(tokens, parnode)
      return nil unless res
      return res if res.fail?
      # No need to tokens.shift, because the linked rule will do it.
      parnode << res
      return parnode
       
    end
    
  end 
  
  class Choice < Rule
    attr_reader :choices
    
    def initialize(name, *choices)
      super(name)
      @choices    = choices
      @edge_color = :blue
    end
    
    def each  
      # loop detection, fatal in choices 
      if @loop
        raise "Loop detected in choice!"
      else   
        @loop = true 
        @choices.each(&Proc.new)
        @loop = false
      end  
    end
    
    def | (other)
      choices    = @choices.to_a
      if (other.respond_to? :choices)
        choices += other.choices
      else 
        choices << other
      end
      return Choice.new("#{self.name}|#{other.name}", *choices)
    end
    
    def first_set
      sets = @choices.map{ |c| c.first_set }
      set = sets.flatten.sort.uniq - [ :empty ]
      return set 
    end

    
    def parse(tokens, parent = nil)
      parnode = Node.new(self.name, :choice, parent)
      for rule in @choices do
        res = rule.parse(tokens, parnode)
        if res 
          return res if res.fail?
          parnode << res
          return parnode
        end  
      end
      # return Rule.parse_error("Unexpected token in #{self.name}", tokens.first)
      return nil # not matched 
    end
  end
  
  class List < Rule
    attr_reader :seq
    
    def initialize(name, *list)
      super(name)
      @seq = list.to_a
      @first_set_loop = false
      @edge_color = :red
    end
    
    def each
      # loop detection, fatal in lists 
      if @loop
        raise "Loop detected in list!"
      else   
        @loop = true 
        @seq.each(&Proc.new)
        @loop = false
      end  
    end
    
    def >>(other)
      seq    = @seq
      if (other.respond_to? :list)
        seq += other.seq
      else 
        seq << other
      end 
      return List.new("#{self.name}>>#{other.name}", *seq)
    end
    
    def first_set
      set = @list.first.first_set
      return set 
    end

      
    def parse(tokens, parent = nil)
      parnode = Node.new(self.name, :list, parent)
      @seq.each do | ru |
        res = ru.parse(tokens, parnode)
        return nil unless res
        if res.fail?
          tkf = tokens.first.kind
          return Node.error("Excpected #{ru.name}, got #{tkf} #{res.inspect} ")
        end
        parnode << res
      end
      return parnode
    end
  
  end
end  