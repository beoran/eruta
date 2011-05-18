require 'test_helper.rb'
require 'raku'

assert { Raku::Node }
node  = Raku::Node.new('test', 20)
node2 = Raku::Node.new('test2', 40)

assert { node.kind == :test }
assert { node.value == 20 }
assert { node.children.empty? }
node << node2 
assert { !node.children.empty? }
assert { node.children.first == node2 }


node.to_graph.display
