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
  name "world's 
 \\"end\\"" foo bar
  layer {
    z 1
    w 8
    h 8
    data {
      row 0 10 20 30 40 50 60 70 80 
      row 1 11 21 31 41 51 61 71 81
      row 2 12 23 33 40 50 60 78 80
      row 3 14 20 30 40 50 60 78 80
      row 4 15 20 30 40 50 60 78 80
      row 5 16 20 30 40 50 60 78 80
      row 6 17 20 30 40 50 60 78 80
      row 7 18 20 30 40 50 60 78 80
      row 8 21
    }
  }
  layer {
    z 2
  }
} else ( nothing )
# Single Line comment 
#( Multi line comment
)#
/* C style multi line comment. */
// C style single line comment.

foo bar \
quux
foo baz
}

=begin
map test {
  name "world's 
 \" end \"" 
  layer {
    z 1 
    w 8 
    h 8 
    data {
      row 0 10 20 30 40 50 60 70 80 
      row 1 11 21 31 41 51 61 71 81 
      row 2 12 23 33 40 50 60 78 80 
      row 3 14 20 30 40 50 60 78 80 
      row 4 15 20 30 40 50 60 78 80 
      row 5 16 20 30 40 50 60 78 80 
      row 6 17 20 30 40 50 60 78 80 
      row 7 18 20 30 40 50 60 78 80 
      row 8 21 
    } 
  } 
  layer {
    z 2  
  } else {
  nothing 
} 
foo bar quux 
foo baz

=end

prog8 = %{
  foo bar ( val 10 30 )
  foo baz ( val 20 )
}

res, err = Raku::Parser.parse(prog5)

assert { res  }
assert { !err }
p err if err
# p res
n = Raku.to_node(res)

assert { n.find_first_key(:map)     }
assert { !n.find_first_key(:banana) }

assert { n.find_all_key(:foo)     }
assert { n.find_all_key(:foo).size == 2   }
assert { !n.find_all_key(:banana)   }

assert { n.find_first(:map, :layer) }
p n.find_first(:map, :layer, :data)

assert { n.find_all(:map, :layer) }
assert { n.find_all(:map, :layer).size == 2 }

# n.to_graph(:each_block).display
rak = n.to_raku
# puts rak 
res2, err2 = Raku::Parser.parse(rak)
assert { res2 } 
assert { !err2 }
p err2 if err2
n2 = Raku.to_node(res2)
# n2.to_graph(:each_block).display

assert('Round trip of to_raku') { res == res2 }

