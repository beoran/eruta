require 'test_helper'
require 'raku'

assert { Raku::Rule } 
include Raku

def rule(name, match = nil)
  return Rule.new(name, match)
end

=begin
PROGRAM -> STATEMENTS .
STATEMENTS -> STATEMENT STATEMENTS | .
STATEMENT -> EXPRESSION | BLOCK | EMPTY_LINE |  comment .
EXPRESSION -> VALUE PARAMLIST NL. 
PARAMLIST -> PARAMETERS.
PARAMETERS-> PARAMETER PARAMETERS | .
PARAMETER -> BLOCK | VALUE .
EMPTY_LINE -> NL .
BLOCK -> do PROGRAM end | ob PROGRAM cb | ( PROGRAM ) | oa PROGRAM ca.
NL -> nl | semicolon .
VALUE -> string | integer | float | symbol | operator .
=end

program = Link.new(:program, nil)

value   = rule(:string) | rule(:integer) | rule(:float) | 
          rule(:symbol) | rule(:operator)

do_block        = rule(:do) | program | rule(:end)
do_block.name   = :do_block

paren_block     = rule(:lparen)   | program | rule(:rparen)
paren_block.name= :paren_block

brace_block     = rule(:lbrace)   | program | rule(:rbrace)
brace_block.name= :brace_block
brack_block     = rule(:lbracket) | program | rule(:rbracket)
brack_block.name= :bracket_block

block           = do_block | paren_block | brace_block | brack_block
# block.name      = :block 
# | brace_block | paren_block | bracket_block
parameter       = block | value
parameters      = Link.new(:parameters)
empty           = Empty.new(:empty)
parameters.rule = parameter >> parameters | empty 
nl              = rule(:nl)
expression      = value >> parameters >> nl
empty_line      = nl
statement       = expression | block | empty_line
statement.name  = :statement
statements      = Link.new(:statements)
statements.rule = statement >> statements | empty
eof             = rule(:eof)
program.rule    = statements >> eof 



syntax          = program
# syntax.to_graph.display

prog2 = %Q{
  map 2 2 2 2.0 {
    layer 1 [
      1 2
      2 1
    ]

    layer 2 (
      2 1
      1 2
    )
  }
}

p value.first_set
p block.first_set

# 
# res = Raku::Lexer.lex_all(prog2, :ws, :nl)
# p res.size 
# 
# val = program.parse(res)
# p val










