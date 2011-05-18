
require 'nanograph'

module Raku
  # A node is a result of the parser that can contain sub-results
  class Node
    include Raku::Fail
    
    attr_reader :kind
    attr_reader :value
    attr_reader :children
    attr_accessor :parent
    
    def initialize(type, value, parent = nil)
      @kind     = type.to_sym
      @value    = value
      @children = []
    end
    
    def << (child)
      @children << child
      child.parent = self
    end
    
    # Walks the tree in depth-first fashion, yielding it's child nodes
    def walk
      yield self
      proc = Proc.new
      @children.each { |child| child.walk(&proc) }
    end
  
    # converts the node to a nanograph graph for visualisation
    def to_graph
      graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
                                  rankdir: "LR")
  
      graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, 
                        style: :rounded }
      self.walk do |n| 
        if n.parent 
          from = 
          graph.node(n.parent.object_id, "#{n.parent.kind} #{n.parent.value}") 
          to   = 
          graph.node(n.object_id, "#{n.kind} #{n.value}")
          from >> to
        end
      end
      return graph
    end
    
    def self.error(message)
      node = self.new(:error, nil, nil)
      node.fail!(message)
      return node
    end

  end # class Node
end # module Raku.
