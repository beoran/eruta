require 'test_helper.rb'

require 'raku'

assert { Raku::Parser } 
assert { Raku::Parser::Node }
node = Raku::Parser::Node.new('test', 20)

assert { node.kind == :test }
assert { node.value == 20 }
assert { node.children.empty? }
node << node 
assert { !node.children.empty? }
assert { node.children.first == node }


pa01   = Raku::Parser.new("12340")
assert { pa01.have?(:integer)  } 
assert { !pa01.have?(:float)  } 
assert { pa01.have?(:float, :integer)  } 

prog2 = %Q{
# map 1
  my.map 2 2 2 2.0 {
    layer 1 [
      1 2
      2 1
    ]

    layer 2 (      
      2 1
      1 2
    )
  }
  frobnicate "foo
  bar"
}

prog3 = %Q{ map 2 2 2 2.0
}

# parser = Raku::Parser.new(prog2)
# assert { parser } 
# result = nil
# parser.parse_empty_statements
# 
# assert { result = parser.parse_value }
# assert { result.kind == :symbol }
# p result 
res = Raku::Parser.new("12340.5").parse_value
assert { res.kind == :float } 
res = Raku::Parser.new("12340 5").parse_value
assert { res.kind == :integer }
res = Raku::Parser.new(" a12340.5").parse_value
assert { res.kind == :symbol }
res = Raku::Parser.new('"Hi!"').parse_value
assert { res.kind == :string }
assert { res.value == '"Hi!"' }

res = Raku::Parser.new('>^-^<').parse_value
assert { res.kind == :operator }
assert { res.value == '>^-^<' }

res = Raku::Parser.new("\n12340.5").parse_value
assert { !res } 
 
 



parser = Raku::Parser.new('map
')
result = nil
assert { parser }
assert { result = parser.parse } 
assert { result }
p result 

parser = Raku::Parser.new(prog3)
result = nil
assert { parser }
assert { result = parser.parse } 
assert { result }
p result 

parser = Raku::Parser.new(prog2)
result = nil
assert { parser }
assert { result = parser.parse } 
assert { result }

 

require 'nanograph'
graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
                            rankdir: "LR")
                            
graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, 
                   style: :rounded }
result.walk do |n| 
  if n.parent 
    from = graph.node(n.parent.object_id, "#{n.parent.kind} #{n.parent.value}") 
    to   = graph.node(n.object_id, "#{n.kind} #{n.value}")
    from >> to
  end
end

graph.display

prog4 = %Q[
  map 2 2 2 2.0 {
    layer 1 [
      1 2
      2 1
    ]

    layer 2 (      
      2 1
      1 2
    )
  
]

parser = Raku::Parser.new(prog4)
result = nil
assert { parser }
begin
  result = parser.parse 
  rescue 
  p $!
end  
assert { !result }
 
