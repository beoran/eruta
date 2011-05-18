require 'test_helper'
require 'raku'

assert { Raku::LL } 

ll = Raku::LL.new

ll.choose(:VALUE, :string, :integer, :float, :symbol, :operator)
ll.list(:DO_BLOCK, :do, :PROGRAM, :end)
ll.list(:PAREN_BLOCK, :lparen, :PROGRAM, :rparen)
ll.list(:BRACE_BLOCK, :lbrace, :PROGRAM, :rbrace)
ll.list(:BRACKET_BLOCK, :lbracket, :PROGRAM, :rbracket)
ll.choose(:BLOCK, :DO_BLOCK, :PAREN_BLOCK, :BRACE_BLOCK, :BRACKET_BLOCK)
ll.choose(:PARAMETER, :BLOCK, :VALUE)
ll.list(:PARAMETERS_NOTEMPTY, :PARAMETER, :PARAMETERS)
ll.choose(:PARAMETERS, :PARAMETERS_NOTEMPTY, :empty)
ll.list(:EXPRESSION, :VALUE, :PARAMETERS, :nl)
ll.choose(:STATEMENT, :EXPRESSION, :BLOCK, :nl)
ll.list(:STATEMENTS_NOTEMPTY, :STATEMENT, :STATEMENTS)
ll.choose(:STATEMENTS, :STATEMENTS_NOTEMPTY, :empty)
ll.list(:PROGRAM, :STATEMENTS, :eof)

p ll.rules
p ll.first_set_for(:VALUE)

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



