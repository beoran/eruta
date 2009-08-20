# Module for handling DOT graphs
module Dot
  # Helper function to process attributes
  def self.attributes_to_dot(attributes, label = nil)
    if (attributes.nil? || attributes.empty?) && (label.nil? || label.empty?) 
      return ""
    end
    
    res = " [ "    
    if label && (!label.empty?)
      res << ' ' + 'label="' + label.to_s + '"'
    end
    for key, value in attributes do
      res << ' ' + key.to_s + '="' + value.to_s + '"'
    end    
    res << " ] \n"
    return res
  end  
  
  def self.graph(attrs={})
    return Graph.new(attrs)
  end

  # anything that has attributes 
  module Attributed
  end 

  class Node
    include Attributed
    attr_accessor :attributes
    attr_accessor :graph
    attr_accessor :id
    attr_accessor :label
    

    def initialize(graph, id, label = nil, attrs = {})
      @graph      = graph
      @id         = id
      @label      = label.gsub(/[\r\n]/,'\n').gsub('\\','\\\\') || id
      @attributes = attrs
    end
    
    def to_dot_id
      return '  "' + id  + '"'
    end
    
    def to_dot()
      res         =  to_dot_id + Dot.attributes_to_dot(@attributes, @label) 
      return res
    end
    
    # Looks up the node_id as a node, or adds a new node with node_id to this 
    # node's graph and links an edge from self to it
    def edge(node_id, dir = true)
      othernode = node_id
      unless node_id.is_a? ::Dot::Node
        othernode = @graph.new_node(othernode.to_s)
      end
      @graph.new_edge(self, othernode, dir)
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
  
  class Edge 
   
    attr_accessor :attributes
    attr_accessor :graph
    attr_accessor :from
    attr_accessor :to
    attr_accessor :directed
    
    def initialize(graph, from, to, dir = true, attrs = {} )
      @graph      = graph
      @from       = from
      @to         = to
      @attributes = attrs
      @directed   = dir
      @label      = nil
    end
    
    def to_dot
      dirstring = " -- " 
      if directed 
        dirstring = " -> "
      end
      
      return @from.to_dot_id + dirstring + @to.to_dot_id + " \n  "
    end
    
  end  
   
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
      grats = Dot.attributes_to_dot(@attributes)
      res << "graph " + grats + "\n" unless grats.empty?
      nats  = Dot.attributes_to_dot(@nodeattr)
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
    
    def display()
      pipe = IO.popen('dot -Tpng -o/tmp/disp.png', 'wt+')
      pipe.write(self.to_dot)
      pipe.close 
      
      if $?.exitstatus > 0
        puts self.to_dot
      else 
        system('display /tmp/disp.png &') 
      end
      
    end
    
    def el_to_label(el)
      return el.class.inspect + "/" + el.extension_modules.inspect + ":\\n" +  el.text_value.gsub("\n", '\\n').gsub('\\','\\\\') + " "
    end  
    
    def from_treetop_element(parentnode, el)
      if el.terminal?
=begin      
        unless el.boring? || el.text_value.size < 2 
          leafnode = self.node(nil, el_to_label(el))
          parentnode >> leafnode
        else 
          # puts "Boring node detected: " + el_to_label(el)  
        end
=end        
      else
        if el.boring? 
          for subel in el.elements
            from_treetop_element(parentnode, subel)
          end
        else
          opts  = {}
          label = el_to_label(el)
          newparent = self.node(nil, label , opts)
          parentnode >> newparent             
          for subel in el.elements
            from_treetop_element(newparent, subel)
          end
        end    
      end
    end
    
    def from_treetop(results)
      unless results
        self.attributes[:label] = 'Parse error!'
        return false 
      end
      rootnode = self.node('root', 'Olivine')
      from_treetop_element(rootnode, results)
      # Walk tree recursively, depth-first
      return true 
    end
    
  end

end


if __FILE__ == $0
  graph     = Dot.graph(fontsize: 10, splines: true, overlap: false, rankdir: "LR")
  graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, style: :rounded}
  graph.node(nil , "Hello, hello") >> 'world' >> '!'
  graph.node('node_1') >> 'world 2'
  graph.display
end

