# encoding : UTF-8
require 'test_helper.rb'

require 'raku'

assert { Raku::Parser } 

def parse(text) 
  parser    = Raku::Parser.new
  tokens    = Raku::Lexer.lex_all(text, :ws, :esc_nl)
  res, err  = parser.parse(tokens)
  if err
    p "error :", err
    return false 
  end
  return res
end

def noparse(text) 
  parser = Raku::Parser.new
  tokens = Raku::Lexer.lex_all(text, :ws, :esc_nl)
  res, err  = parser.parse(tokens)
  return false if res
  return err
end


prog2 = %Q{
# map 1
  my map 2 2 2 2.0 {
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
assert { parse( "12340.5\n")   } 
assert { parse( "12340 5\n")   } 
assert { parse( "a12340 \n")   }
assert { parse( '"Hello!"' + "\n")  }
assert { parse( '"Hello \" escape! "' + "\n")  }
# xxx: below fails with lex error
# assert { parse( "'Hello!'")  }
assert { parse( '>^-^<' + "\n")     }
res = [[ :'>^-^<']]
assert { parse( '>^-^<' + "\n")  ==  res }
assert { noparse( ")\n")      }

assert { parse(%{  map 
})  } 

assert { parse(%{  map 
})  } 


assert { parse(%{  map 1 2 3 
})  } 


assert { noparse( %{  map 1 2 3 ( 4 ) 5 6 ) )
}) }

assert { parse( %{  map 1 2 3 ( 4 5 6 )
}) }

# This will work:, a newline is needed after the 4 5 6 expression
assert { parse( %{  map 1 2 3 ( 4 5 6 
)
}) }

p parse( %{  map 1 2 3 ( 4 5 6 
)
})



# Should not parse: 
assert { noparse( %{  map  
)
}) }

assert { parse( prog2) } 
assert { parse( prog3) }

p parse( prog2) 

prog7 = %{ 
map {
  name "world map"
  size 100 100 32 32
  layer {
    z 1
    size 8 8 32 32
    data {
      row 10 20 30 40 50 60 78 80 
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
    }
  }
}
}


prog5 = %{ 
map {
  name "world map 2"
  layer {
    z 1
    w 8
    h 8
    data {
      row 10 20 30 40 50 60 78 80 
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
      row 10 20 30 40 50 60 78 80
    }
  }
}
}

prog6 = %{
let a $ ( 5 + 10 ) 
}

assert { parse( prog5) }

res = parse(prog5)
p res 
p Raku.to_hash(res) 

assert { parse( prog6) }

res = parse(prog6)
p res 



# parser = Raku::Parser.new(prog3)
# result = nil
# assert { parser }
# assert { result = parser.parse } 
# assert { result }
# p result 
# 
# parser = Raku::Parser.new(prog2)
# result = nil
# assert { parser }
# assert { result = parser.parse } 
# assert { result }
#  
# # 
# # require 'nanograph'
# # graph     = Nanograph.graph(fontsize: 10, splines: true, overlap: false,
# #                             rankdir: "LR")
# # 
# # graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, 
# #                    style: :rounded }
# # result.walk do |n| 
# #   if n.parent 
# #     from = graph.node(n.parent.object_id, "#{n.parent.kind} #{n.parent.value}") 
# #     to   = graph.node(n.object_id, "#{n.kind} #{n.value}")
# #     from >> to
# #   end
# # end
# 
# # result.to_graph.display
# 
# prog4 = %Q[
#   map 2 2 2 2.0 {
#     layer 1 [
#       1 2
#       2 1
#     ]
# 
#     layer 2 do
#       2 1
#       1 2
#     end}
# ]
#     
# 
# parser = Raku::Parser.new(prog4)
# result = nil
# assert { parser }
# assert { result = parser.parse }
# assert { result }
# result.to_graph.display

 
