module Raku
   # A token is the result of a single step of the lexer
   class Token
     include Fail
     attr_reader :kind
     attr_reader :value
     attr_reader :line
     attr_reader :col

     def initialize(kind, value = "", line = 0, col = 0)
       @kind   = kind.to_sym
       @value  = value
       @line   = line
       @col    = col
       if kind == :fail then
        self.fail!(value)
       end
     end
     
     def eof?
      self.kind == :eof 
     end
   end
end
