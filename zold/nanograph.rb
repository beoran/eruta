# Tiny module for handling Graphviz Module DOT graphs
# Useful when implementing parsers to visualise your AST's.
require 'tempfile'

module Nanograph
  # Helper function to process attributes
  def self.attributes_to_dot(attributes, label = nil)
    if label
      attributes[:label] = label
    end   
      
    if (attributes.nil? || attributes.empty?) 
      return ""
    end
    
    res = " [ "    
    for key, value in attributes do
      res << ' ' + key.to_s + '="' + value.to_s + '"'
    end    
    res << " ] \n"
    return res
  end  
  
  # Creates a new graph with the given attributes.
  def self.graph(attrs={})
    return Graph.new(attrs)
  end

  # A node in a graph.
  class Node
    attr_accessor :attributes
    attr_accessor :graph
    attr_accessor :id
    attr_accessor :label
    

    def initialize(graph, id, label = nil, attrs = {})
      @graph      = graph
      @id         = id
      if label
        @label    = label
      else 
        @label = id.to_s
      end
      @label.gsub!(/[\r\n]/,'\n')
      @label.gsub!('\\','\\\\')
      @label.gsub!('"','``')
      @attributes = attrs
    end
    
    # Converts the node id to dot format.
    def to_dot_id
      return '  "' + id.to_s  + '"'
    end
    
    # Converts the node to dot format.
    def to_dot()
      res         =  to_dot_id + 
                  Nanograph.attributes_to_dot(@attributes, @label) 
      return res
    end
    
    # Looks up the node_id as a node, or adds a new node with node_id to this 
    # node's graph and links an edge from self to it
    # If dir is true, it's a directed edge, otherwise it's an undirected edge.
    def edge(node_id, dir = true, attrs= {})
      othernode = node_id
      unless node_id.is_a? ::Nanograph::Node
        othernode = @graph.new_node(othernode.to_s)
      end
      @graph.new_edge(self, othernode, dir, attrs)
      return othernode 
    end
    
    # Syntactic sygar for self.graph.egde(self, othernode, true )
    # Returns othernode for chaining
    def >> (othernode) 
      return self.edge(othernode, true) 
    end
    
    # Syntactic sygar for self.graph.egde(self, othernode, false )
    # Returns othernode for chaining
    def - (othernode) 
      return self.edge(othernode, false)
    end
    
  end
  
  # Models an edge between two graph nodes.
  class Edge 
   
    attr_accessor :attributes
    attr_accessor :graph
    attr_accessor :from
    attr_accessor :to
    attr_accessor :directed
    
    # Initializes the Edge
    def initialize(graph, from, to, dir = true, attrs = {} )
      @graph      = graph
      @from       = from
      @to         = to
      @attributes = attrs
      @directed   = dir
      @label      = nil
    end
    
    # Converts the edge to dot format.
    def to_dot
      dirstring = " -- " 
      if directed 
        dirstring = " -> "
      end
      attrstr = Nanograph.attributes_to_dot(@attributes)
      return @from.to_dot_id + dirstring + @to.to_dot_id + " #{attrstr} \n  "
    end
    
  end  
  
  # Class that models the whole graph.  
  class Graph
    attr_accessor :attributes
    attr_accessor :nodes
    attr_accessor :edges
    attr_accessor :nodeattr
    attr_accessor :edgeattr
    
    
    def initialize(attrs= {} )
      @attributes = attrs
      @nodeattr   = {}
      @edgeattr   = {}
      @nodes      = {}
      @edges      = []
      @label      = nil
      @autonode   = 0  
    end
    
    # Adds node to the graph 
    def new_node(id, label = nil, attr={})
      newnode             = Node.new(self, id, label, attr)
      @nodes[newnode.id]  = newnode
      return newnode 
    end
    
    # Adds a new node if it doesn't exist. Otherwise returns the existing node
    # if the node id is nil, autogenerate it 
    def node(id = nil, label = nil, attr={})
      unless id
        @autonode += 1 
        id = "node_#@autonode"
      end  
      nod  = @nodes[id]
      return nod if nod
      return new_node(id, label, attr) 
    end  
    
    # Adds an edge to the graph
    def new_edge(from, to, dir = true, attr={})
      newedge = Edge.new(self, from, to, dir, attr)
      @edges  << newedge
      return newedge 
    end
    
    # Adds an edge by ids. Raises an exception if IDs are not found 
    def edge(from_id, to_id, dir = true, attr = {})
      from    = @nodes[from_id]
      to      = @nodes[to_id]
      raise "No such node #{from_id}" unless from
      raise "No such node #{to_id}"   unless to
      return new_edge(from, to, dir, attr)    
    end
    
    # graph to dot graph
    def to_dot
      res   = "digraph { \n"
      grats = Nanograph.attributes_to_dot(@attributes)
      res << "graph " + grats + "\n" unless grats.empty?
      nats  = Nanograph.attributes_to_dot(@nodeattr)
      res << "node " + nats + "\n" unless nats.empty?
      for k, n in @nodes
        res << n.to_dot + "\n"
      end        
      for e in @edges 
        res << e.to_dot + "\n"
      end 
      res << "\n}\n"
      return res
    end
    
    # converts graph to png file usung the dot tool
    # which must be in your path somewhere
    def to_png(filename)
      pipe = IO.popen("dot -Tpng -o#{filename}", 'wt+')
      pipe.write(self.to_dot)
      pipe.close
      return nil if $?.exitstatus > 0
      return filename 
    end
    
    # displays the graph using dot and imagemogick display commands
    def display()
      f = Tempfile.new('nanograph_')
      n = f.path
      o = self.to_png(n) 
      unless o
        puts self.to_dot
      else 
        system("display #{n} &")
        sleep 2
      end
      ensure 
       f.close if f
    end
    
    
  end

end
