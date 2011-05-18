require 'test_helper'
require 'raku'

assert { Raku::LL } 

ll = Raku::LL.new

ll.choose(:VALUE, :string, :integer, :float, :symbol, :operator)
ll.choose(:NL, :nl, :semicolon)
ll.list(:DO_BLOCK, :do, :PROGRAM, :end)
ll.list(:PAREN_BLOCK, :lparen, :PROGRAM, :rparen)
ll.list(:BRACE_BLOCK, :lbrace, :PROGRAM, :rbrace)
ll.list(:BRACKET_BLOCK, :lbracket, :PROGRAM, :rbracket)
ll.choose(:BLOCK, :DO_BLOCK, :PAREN_BLOCK, :BRACE_BLOCK, :BRACKET_BLOCK)
ll.choose(:PARAMETER, :BLOCK, :VALUE)
ll.list(:PARAMETERS_NOTEMPTY, :PARAMETER, :PARAMETERS)
ll.choose(:PARAMETERS, :PARAMETERS_NOTEMPTY, :empty)
ll.list(:EXPRESSION, :VALUE, :PARAMETERS, :NL)
ll.choose(:STATEMENT, :EXPRESSION, :BLOCK, :NL)
ll.list(:STATEMENTS_NOTEMPTY, :STATEMENT, :STATEMENTS)
ll.choose(:STATEMENTS, :STATEMENTS_NOTEMPTY, :empty)
ll.list(:PROGRAM, :STATEMENTS, :eof)

p ll.rules
p ll.first_set_for(:VALUE)
p ll.first_set_for(:DO_BLOCK)
p ll.first_set_for(:PAREN_BLOCK)
p ll.first_set_for(:PROGRAM)
p ll.first_set_for(:PARAMETERS)
p ll.follow_set_for(:PARAMETERS)

ll.rules.each do |name, rule|
  p name
  p ll.first_set_for(name)
  p ll.follow_set_for(name)
end


# parameter       = block | value
# parameters      = Link.new(:parameters)
# empty           = Empty.new(:empty)
# parameters.rule = parameter >> parameters | empty 
# nl              = rule(:nl)
# expression      = value >> parameters >> nl
# empty_line      = nl
# statement       = expression | block | empty_line
# statement.name  = :statement
# statements      = Link.new(:statements)
# statements.rule = statement >> statements | empty
# eof             = rule(:eof)
# program.rule    = statements >> eof 
# 



