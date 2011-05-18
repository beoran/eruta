
# Raku is a data export and scripting language, a bit like TCL or JSON, 
# but better, since it is parsable qwiin O(n) time with a predictive parser. 
module Raku
  autoload :Fail  , 'raku/fail'
  autoload :Lexer , 'raku/lexer'
  autoload :LL    , 'raku/ll'
  autoload :Node  , 'raku/node'
  autoload :Parser, 'raku/parser'
  autoload :Rule  , 'raku/rule'
  autoload :Token , 'raku/token'
end
