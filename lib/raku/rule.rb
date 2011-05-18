require 'nanograph' 

module Raku
  class Rule
    
    attr_accessor :name
    
    def initialize(name, match = nil)
      @name  = name.to_s
      @match = match || name.to_sym
      # loop detection
      @loop  = false
    end
    
    def each
      yield nil
    end
   
   def to_graph_node(graph)
      mynode = graph.node(self.object_id, "#{@name}")
      self.each do |child|
        if child
          childnode = child.to_graph_node(graph)
          mynode >> childnode
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
      return Node.error("Lex error: #{token.inspect}") if !token || token.fail?
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
      if tokens.empty? || tokens.first.kind == :eof
        return Node.error("Unexpected end of file: #{tokens.first.inspect}")
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
      @choices = choices
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
      choices    = @choices
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
          # no neet to shift, only simple rule nodes do that tokens.shift
          return parnode
        end  
      end
      return nil # not matched  
    end
  end
  
  class List < Rule
    attr_reader :list
    
    def initialize(name, *list)
      super(name)
      @list = list
      @first_set_loop = false
    end
    
    def each
      # loop detection, fatal in lists 
      if @loop
        raise "Loop detected in list!"
      else   
        @loop = true 
        @list.each(&Proc.new)
        @loop = false
      end  
    end
    
    def >>(other)
      list    = @list
      if (other.respond_to? :list)
        list += other.list
      else 
        list << other
      end 
      return List.new("#{self.name}>>#{other.name}", *list)
    end
    
    def first_set
      set = @list.first.first_set
      return set 
    end

      
    def parse(tokens, parent = nil)
      parnode = Node.new(self.name, :list, parent)
      for rule in @list do
        res = rule.parse(tokens, parnode)
        if res 
          return Node.error("Excpected #{rule.name}") if res.fail?
          parnode << res
        else
          return nil
        end  
      end
      return parnode
    end
  
  end
end  