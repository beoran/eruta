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

prog2 = %Q{
  map 2 2 2 2.0 {
    layer 1 {
      1 2
      2 1
    }

    layer 2 {
      2 1
      1 2
    }
  }
}

prog3 = %Q{ map 2 2 2 2.0
}

# parser = Raku::Parser.new(prog2)
# assert { parser } 
# result = nil
# parser.parse_empty_statements
# 
# assert { result = parser.parse_basic }
# assert { result.kind == :symbol }
# p result 
res = Raku::Parser.new("12340.5").parse_basic
p res 
assert { res.kind == :float } 
res = Raku::Parser.new("12340 5").parse_basic
p res 
assert { res.kind == :integer }
res = Raku::Parser.new(" a12340.5").parse_basic
p res 
assert { res.kind == :symbol }
res = Raku::Parser.new('"Hi!"').parse_basic
p res 
assert { res.kind == :string }
assert { res.value == '"Hi!"' }

res = Raku::Parser.new('>^-^<').parse_basic
p res 
assert { res.kind == :operator }
assert { res.value == '>^-^<' }

res = Raku::Parser.new("\n12340.5").parse_basic
assert { !res } 
 
 


# parser = Raku::Parser.new(prog3)
# result = nil
# assert { parser }
# assert { result = parser.parse } 
# assert { result }
# p result 

# parser = Raku::Parser.new('map
# ')
# result = nil
# assert { parser }
# assert { result = parser.parse } 
# assert { result }
# p result 

 
parser = Raku::Parser.new('map
')
result = nil
assert { parser }
assert { result = parser.parse_statement } 
assert { result }
p result 

