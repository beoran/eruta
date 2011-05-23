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
statements      = Link.new(:statements)

value   = rule(:string) | rule(:integer) | rule(:float) | 
          rule(:symbol) | rule(:operator)
value.name      = :value
do_rule         = rule(:do)
end_rule        = rule(:end)
do_block        = do_rule >> statements >> end_rule
do_block.name   = :do_block

paren_block     = rule(:lparen)   >> statements >> rule(:rparen)
paren_block.name= :paren_block

brace_block     = rule(:lbrace)   >> statements >> rule(:rbrace)
brace_block.name= :brace_block
brack_block     = rule(:lbracket) >> statements >> rule(:rbracket)
brack_block.name= :bracket_block

block           = do_block | paren_block | brace_block | brack_block
block.name      = :block 
# | brace_block | paren_block | bracket_block
parameter       = value | block
parameter.name  = :parameter
empty           = Empty.new(:empty) 
parameters      = Link.new(:parameters)
parameters.rule       = parameter >> parameters | empty
parameters.rule.name  = :parameters 
nl              = rule(:nl) | rule(:semicolon)
nl.name         = :nl
expression      = value >> parameters >> nl
expression.name = :expression
empty_line      = Link.new(:empty_line)
empty_line.rule = nl
empty_line.rule.name = :empty_line
statement       = expression | block | empty_line
statement.name  = :statement
nestats         = statement >> statements
nestats.name    = :statements_ne
statements.rule = nestats | empty
statements.rule.name  = :statements
eof             = rule(:eof)
program.rule    = statements >> eof
program.rule.name  = :program 


bar       = Link.new(:bar)
bar.rule  = rule(:integer) >> rule(:symbol) >> rule(:eof)

xitem       = rule(:symbol)
xlist       = Link.new(:xlist)
xlist.rule  = (xitem >> xlist) | Empty.new(:empty)


#syntax          = program
#syntax.to_graph.display

prog2 = %Q{  
map "world" 2 2 2 2.0 {
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

# res   = Raku::Lexer.lex_all("foo bar ;", :ws, :nl)
# val   = xlist.parse(res)
# p val, res.count

# p parameter.choices.map { |r| r.name }
# p parameter.choices.count

res = Raku::Lexer.lex_all(prog2, :ws)
p res.count
val = program.parse(res)
p "parse failed?", val.fail?
if val.fail?
  puts val.fail_message
else
  p val.to_h  
  # val.to_graph.display
end

# p value.first_set
# p block.first_set

# 
# 
# p res.size 
# 
# val = program.parse(res)
# p val










