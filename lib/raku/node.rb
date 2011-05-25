
require 'nanograph'

module Raku
  # A node represents the results of parsing a Raku document in a 
  # document object model (DOM) style.
  class Node
    attr_accessor :name
    attr_reader   :data
    attr_reader   :children
    
    def initialize(name = :root)
      @name       = name
      @data       = []
      @children   = []
    end
    
    # Iterates ofver this node's children.
    def each
      @children.each { |c| yield c }
    end
    
    # Adds a child node to this node.
    def <<(child)
      @children << child
    end
    
    # Converts the Node to a Nanograph for visualisation.
    def to_graph
      graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
                                  rankdir: "LR")
      graph.nodeattr = { fontsize: 9, width:0, height:0, shape: :box, 
                        style: :rounded }
      self.to_graph_node(graph)
      return graph
    end
         
    def to_graph_node(graph, index = 0)
      from = graph.node(object_id, "{#{@name}|#{@data.join(',')}}", :shape => :record)
      jdex = 0
      self.each do |c|
        to = c.to_graph_node(graph, jdex)
        from >> to
        jdex += 1
      end
      return from
    end
    
    def select(key)
      return @children.select { |c| c.name == key }
    end
    
    def reject(key)
      return @children.reject { |c| c.name == key } 
    end
    
    # Deep search, returns first match or nil if not found. 
    def find_first_key(key)
      return self if self.name == key
      @children.each do |c|
        res = c.find_first_key(key)
        return res if res
      end 
      return nil
    end
    
    # Deep search, returns all matches in a new node with name :result,
    # with the results being the children. Returns nil if no results. 
    def find_all_key(key)
      result = Node.new(:result)
      result << self if self.name == key
      @children.each do |c|
        res = c.find_all_key(key)
        result << res if res && res.children.size > 0
      end 
      return result if result.children.size > 0
      return nil
    end
    
    # Deep search with multiple keys. Searches by calling find_first_key recursively.
    def find_first(*keys)
      node     = self
      for key in keys
        result = node.find_first_key(key)
        return nil unless result
        node   = result
      end   
      return result
    end 
    
    # Deep search with multiple keys. Searches by calling find_all_key recursively.
    def find(*keys)
      node     = self
      for key in keys
        result = node.find_all_key(key)
        return nil unless result
        node   = result
      end   
      return result
    end 

    
    def self.statement_to_node(line)
      return nil, nil   unless line
      unless line.is_a? Array
        return line
      end
      if line.first.is_a?(Array)
        return program_to_n(line)
      end    
      command          = line.shift 
      res              = Node.new(command)
      params           = []
      for arg in line
        if arg.is_a?(Array) # sub array means block parameter
          # this will be an array of substatements
          substats   = program_to_node(arg, command)
          res        = substats
        else # normal parameter
          params << arg
        end 
      end
      params.each { |p| res.data << p } 
      return res, command
    end
    
    def self.program_to_node(data, command = :root)
      res = Node.new(command)
      for line in data
        h, command = statement_to_node(line)
        res << h if h
      end
      return res
    end
  
  end # class Node
end # module Raku.
