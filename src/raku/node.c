#include "raku_node.h"
#include "si_block.h"
#include "si_tree.h"



struct RakuNode_ { 
  RakuNodeKind   kind;
  SiTree       * node;
};

RakuNode * rakunode_init(RakuNode * self, RakuNodeKind kind) { 
  self->kind     = kind;
  self->node     = sitree_new(self);
  return self;
}

/*
  
  
      @data       = []
      @children   = []
      @all        = []
      @level      = 0
    end
    
    # Iterates over this node's children. Gives a data-centric view of the 
    # subnodes, which _block_ nodes skipped.
    def each
      @children.each do |c|
        yield c
      end
    end
    
    # Iterates over the node's subnodes in @all, revealing the _block_ subnodes.
    # Ideal for a programming-language view of the language or for to_raku.
    def each_block
      @all.each do |c|
        yield c if c.is_a?(Node)
      end
    end
    
    def to_s
      return "*#@name"
    end
      
    
    # Adds a child node to this node.
    def <<(child)
      if child.is_a?(Node)
        if child.block? 
          child.each do |sub|
            @children << sub
          end
        else
          @children << child
        end
        @all      << child # .name
      else
        @data     << child
        @all      << child
      end
    end
    
    # returns true if this is a root node, ie, a node with the name :_root_
    def root?
      return self.name == :_root_
    end
    
    # returns true if this is a block node, ie, a node with the name :_block_
    def block?
      return self.name == :_block_
    end
    
    
    # indents the given string with 2 * level
    def ind(str, level)
      return str if level < 1
      return (" " * (level)) + str 
    end
    
    # Quotes a string for use in Raku
    def quote_string(str)
      return '"' + str.gsub('"', '\\"') + '"'
    end
    
    # Converts node to raku for saving. Level is used for proper indenting.
    def to_raku(level = -2)
      level += 1
      res    = ""
      unless self.root? || self.block?
        res << ind("#{self.name} ", level)
      end  
      if self.block?
        res << "{\n"
      end 
      for datum in self.all do 
        if datum.respond_to?(:to_raku) || datum.is_a?(Node)
          res << datum.to_raku(level)
        elsif datum.is_a?(String)
          res << quote_string(datum)
          res << ' '
        else
          res << "#{datum} "
        end
      end
      if self.block?
        res << ind("} ", level - 1)
      else  
        res << "\n"
      end  
      level -= 1
      return res
    end
    
    # Converts the Node to a Nanograph for visualisation.
    def to_graph(iter = :each)
      graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
                                  rankdir: "LR")
      graph.nodeattr = { fontsize: 9, width:0, height:0, shape: :box, 
                        style: :rounded }
      self.to_graph_node(graph, iter)
      return graph
    end
    
    # Converts self to a graph node, not including the child nodes.
    def to_graph_node_self(graph)
      aaid = @all.join(',')
      return graph.node(object_id, "{#{@name}|#{aaid}}", :shape => :record)
    end  

    # Converts self and child nodes to a graph.
    def to_graph_node(graph, iter = :each)
      from = to_graph_node_self(graph)
      self.send(iter) do |c|
        to = c.to_graph_node(graph)
        from >> to
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
    
    # Deep search, returns all matches in an array. Returns nil if no results. 
    def find_all_key(key)
      result = []
      result << self if self.name == key
      @children.each do |c|
        res = c.find_all_key(key)
        result += res if res
      end 
      return result if result.size > 0
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
    def find_all(*keys)
      nodes       = [ self ]
      result      = []
      for key in keys
        result    = []
        for node in nodes
          found   = node.find_all_key(key)
          result += found
        end  
        nodes  = result
      end   
      return result
    end 

    
    def self.statement_to_node(line)
      return nil, nil unless line
      unless line.is_a? Array
        return line
      end
      if line.first.is_a?(Array)
        return program_to_node(line, :_array_)
      end
      command          = line.shift 
      res              = Node.new(command)
      params           = []
      for arg in line
        if arg.is_a?(Array) # sub array means block parameter
          substats   = program_to_node(arg, :_block_)
          # node << handles this by storing to res.children and res.all
          res        << substats
        else # normal parameter
          # node << handles this by storing to res.data
          res       << arg
        end 
      end
      # params.each { |p| res.data << p } 
      return res, command
    end
    
    def self.program_to_node(data, command = :_root_)
      res = Node.new(command)
      for line in data
        h, command = statement_to_node(line)
        res << h if h
      end
      return res
    end
  
  end # class Node
end # module Raku.


*/



