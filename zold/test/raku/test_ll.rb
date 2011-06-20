require 'test_helper'
require 'raku'

assert { Raku::LL } 

ll = Raku::LL.new

ll.choose(:VALUE, :string, :integer, :float, :symbol, :operator)
ll.choose(:NL, :nl, :semicolon)
ll.list(:DO_BLOCK, :do, :STATEMENTS, :end)
ll.list(:PAREN_BLOCK, :lparen, :STATEMENTS, :rparen)
ll.list(:BRACE_BLOCK, :lbrace, :STATEMENTS, :rbrace)
ll.list(:BRACKET_BLOCK, :lbracket, :STATEMENTS, :rbracket)
ll.choose(:BLOCK, :DO_BLOCK, :PAREN_BLOCK, :BRACE_BLOCK, :BRACKET_BLOCK)
ll.choose(:PARAMETER, :BLOCK, :VALUE)
ll.list(:PARAMETERS_NOTEMPTY, :PARAMETER, :PARAMETERS)
ll.choose(:PARAMETERS, :PARAMETERS_NOTEMPTY, :empty)
ll.list(:EXPRESSION, :VALUE, :PARAMETERS, :NL)
ll.choose(:STATEMENT, :EXPRESSION, :BLOCK, :NL)
ll.list(:STATEMENTS_NOTEMPTY, :STATEMENT, :STATEMENTS)
ll.choose(:STATEMENTS, :STATEMENTS_NOTEMPTY, :empty)
ll.list(:PROGRAM, :STATEMENTS, :eof)

# p ll.rules
# p ll.first_set_for(:VALUE)
# p ll.first_set_for(:DO_BLOCK)
# p ll.first_set_for(:PAREN_BLOCK)
# p ll.first_set_for(:PARAMETERS)
# p ll.follow_set_for(:PARAMETERS)
# p "first:", ll.first_set_for(:PROGRAM)
# p "follow:", ll.follow_set_for(:PROGRAM)

ll.rules.each do |name, rule|
  p name
  p ll.first_set_for(name)
  p ll.follow_set_for(name)
end

# p ll.all_terminals
# p ll.all_nonterminals
# p ll.make_parsing_table

res   = Raku::Lexer.lex_all("foo bar ; ", :ws, :nl)
val   = ll.parse(:PROGRAM, *res)
p res.count
p "Parse result:", val 


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
=begin
  $ operator  symbol  float integer string  ; nl  ca  oa  cp  op  cb  ob  end do  comment eof
PROGRAM       PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof      PROGRAM → STATEMENTS eof      PROGRAM → STATEMENTS eof      PROGRAM → STATEMENTS eof      PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof  PROGRAM → STATEMENTS eof
STATEMENTS      STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY      STATEMENTS → STATEMENTS_NONEMPTY      STATEMENTS → STATEMENTS_NONEMPTY      STATEMENTS → STATEMENTS_NONEMPTY      STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → STATEMENTS_NONEMPTY  STATEMENTS → &epsilon
STATEMENTS_NONEMPTY       STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS      STATEMENTS_NONEMPTY → STATEMENT STATEMENTS      STATEMENTS_NONEMPTY → STATEMENT STATEMENTS      STATEMENTS_NONEMPTY → STATEMENT STATEMENTS      STATEMENTS_NONEMPTY → STATEMENT STATEMENTS  STATEMENTS_NONEMPTY → STATEMENT STATEMENTS   
STATEMENT       STATEMENT → EXPRESSION  STATEMENT → EXPRESSION  STATEMENT → EXPRESSION  STATEMENT → EXPRESSION  STATEMENT → EXPRESSION  STATEMENT → EMPTY_LINE  STATEMENT → EMPTY_LINE      STATEMENT → BLOCK       STATEMENT → BLOCK       STATEMENT → BLOCK       STATEMENT → BLOCK   STATEMENT → comment    
EXPRESSION      EXPRESSION → VALUE PARAMETERS NL  EXPRESSION → VALUE PARAMETERS NL  EXPRESSION → VALUE PARAMETERS NL  EXPRESSION → VALUE PARAMETERS NL  EXPRESSION → VALUE PARAMETERS NL                                               
PARAMETERS_NONEMPTY       PARAMETERS_NONEMPTY → PARAMETER PARAMETERS  PARAMETERS_NONEMPTY → PARAMETER PARAMETERS  PARAMETERS_NONEMPTY → PARAMETER PARAMETERS  PARAMETERS_NONEMPTY → PARAMETER PARAMETERS  PARAMETERS_NONEMPTY → PARAMETER PARAMETERS              PARAMETERS_NONEMPTY → PARAMETER PARAMETERS      PARAMETERS_NONEMPTY → PARAMETER PARAMETERS      PARAMETERS_NONEMPTY → PARAMETER PARAMETERS      PARAMETERS_NONEMPTY → PARAMETER PARAMETERS       
PARAMETERS      PARAMETERS → PARAMETERS_NONEMPTY  PARAMETERS → PARAMETERS_NONEMPTY  PARAMETERS → PARAMETERS_NONEMPTY  PARAMETERS → PARAMETERS_NONEMPTY  PARAMETERS → PARAMETERS_NONEMPTY  PARAMETERS → &epsilon   PARAMETERS → &epsilon       PARAMETERS → PARAMETERS_NONEMPTY      PARAMETERS → PARAMETERS_NONEMPTY      PARAMETERS → PARAMETERS_NONEMPTY      PARAMETERS → PARAMETERS_NONEMPTY       
PARAMETER       PARAMETER → VALUE   PARAMETER → VALUE   PARAMETER → VALUE   PARAMETER → VALUE   PARAMETER → VALUE               PARAMETER → BLOCK       PARAMETER → BLOCK       PARAMETER → BLOCK       PARAMETER → BLOCK        
EMPTY_LINE                          EMPTY_LINE → NL   EMPTY_LINE → NL                                        
DO_BLOCK                                                              DO_BLOCK → do PROGRAM end        
BLOCK                                       BLOCK → oa PROGRAM ca       BLOCK → op PROGRAM cp       BLOCK → ob PROGRAM cb       BLOCK → DO_BLOCK       
NL                          NL → ;  NL → nl                                        
VALUE       VALUE → operator  VALUE → symbol  VALUE → float   VALUE → integer   VALUE → string                                               
Return home to enter a new grammar. 
=end


