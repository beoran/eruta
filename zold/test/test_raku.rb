require 'test_helper.rb'

require 'nanograph'
require 'raku'


assert { Raku } 
assert { Raku::Lexer } 

  
  
 

prog5 = %{ 
map test {
  name "world map"
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
}
foo bar
}

prog8 = %{
  foo bar ( val 10 30 )
  foo baz ( val 20 )
}

res, err = Raku::Parser.parse(prog5)

assert { res  }
assert { !err }
p res


# n.to_graph.display







