require 'test_helper.rb'
require 'raku'

assert { Raku::Node }
node = nil
assert { node  = Raku::Node.new(:test) } 
node2 = nil 
assert { node2 = Raku::Node.new(:test2) } 

assert { node.name == :test }
assert { node.data.empty?  }
assert { node.children.empty? }
node << node2 
assert { !node.children.empty? }
assert { node.children.first == node2 }

2.times do 
  node.each do |sub|
    assert { sub }
    assert { sub == node2 } 
  end  
end

assert { node.children == [node2] }

prog5 = %{ 
map test {
  name "world map"
  layer {
    z 1
    w 8
    h 8
    data {
      0 10 20 30 40 50 60 70 80 
      1 11 21 31 41 51 61 71 81
      2 12 23 33 40 50 60 78 80
      3 14 20 30 40 50 60 78 80
      4 15 20 30 40 50 60 78 80
      5 16 20 30 40 50 60 78 80
      6 17 20 30 40 50 60 78 80
      7 18 20 30 40 50 60 78 80
      8 21
    }
  }
  layer {
    z 2
  }
}
foo bar
foo baz
}

prog8 = %{
  foo bar ( val 10 30 )
  foo baz ( val 20 )
}

res, err = Raku::Parser.parse(prog5)

assert { res  }
assert { !err }
p res
n = Raku.to_node(res)

assert { n.find_first_key(:map)     }
assert { !n.find_first_key(:banana) }

assert { n.find_all_key(:foo)     }
assert { n.find_all_key(:foo).children.size == 2   }
assert { !n.find_all_key(:banana)   }

assert { n.find_first(:map, :layer) }


# n.to_graph.display


