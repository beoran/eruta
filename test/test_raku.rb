require 'test_helper.rb'

require 'raku'

assert { Raku } 
assert { Raku::Lexer } 

  
  
 

prog5 = %{ 
map {
  name "world map"
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

res = Raku::Parser.parse(prog5)

assert { res }
p res
p Raku.to_hash(res)







