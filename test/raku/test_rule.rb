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
do_rule         = rule(:do)
end_rule        = rule(:end)
do_block        = do_rule | program | end_rule
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


bar       = Link.new(:bar)
bar.rule  = rule(:integer) >> rule(:symbol) >> rule(:eof)

xitem       = rule(:symbol)
xlist       = Link.new(:xlist)
xlist.rule  = (xitem >> xlist) | Empty.new(:empty)


syntax          = program
xlist.to_graph.display

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

res   = Raku::Lexer.lex_all(prog2, :ws, :nl)
val   = value.parse(res)
p val

res   = Raku::Lexer.lex_all("foo bar", :ws, :nl)
val   = xlist.parse(res)
p val, res.count

res = Raku::Lexer.lex_all(prog2, :ws, :nl)
p res.count
val = do_block.parse(res)
p val, res.count


# p value.first_set
# p block.first_set

# 
# 
# p res.size 
# 
# val = program.parse(res)
# p val










