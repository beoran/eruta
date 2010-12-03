# An tree

#require 'enumerable'

class Tree
  
  attr_reader :parent
  attr_reader :children
  attr_reader :data
  attr_reader :root
  
  def initialize(data, children = [], parent = nil)
    @parent   = parent
    if parent
      @root = parent.root
    else  
      @root  = self
    end  
    # No parent means self is the root
    @children = children
    @data     = data
  end

  def new_child(data)
    return Tree.new(data, [], self)
  end
  
  def add(data)
    child = new_child(data)
    return add_child(child)
  end
  
  alias << :add
  
  def add_child(child)
     @children << child
     return self
  end
  
  def each(&block)
    @children.each do |child| 
      block.call(child)
    end
  end
  
  def first
    return @children.first
  end
  
  def last
    return @children.last
  end
  
  def <=>(other)
    return @data <=> other.data
  end
  
  def sort
    sorted_children = []
    @children.each do |child| sorted_children << child.sort end
    return Tree.new(@data, sorted_children.sort, @parent)
  end
  
  def inspect
    return "Tree with #{@children.size} leaves, and data: #{@data}"
  end
  
end


if $0 == __FILE__
  t   = Tree.new('root of tree')
  t   = t << 'branch3'
  t   = t << 'branch2'
  t   = t << 'branch1'
  st  = t.first << 'branch1-1'
  p t
  t.each { |b| p b } 
  p t.first
  p t.first.parent if t.first
  p t.last.data
  p t.sort.last.data
  p t.last.root
  p t.first.first
  p st
  p st.root
    
  
    
  
end

