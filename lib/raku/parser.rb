#
# Parser for the Raku dta definition, scripting and  and programming
# language. Like Lisp or TCL but better. ;-)
# Raku's grammar is a grammar verified to be LL(1) to guarantee parse speed.
# An LL(1) grammar can be parsed by a predictive parser, which runs in 
# linear time, or O(n) time.
#  
# Verified with : http://smlweb.cpsc.ucalgary.ca/start.html
#  
=begin

PROGRAM -> STATEMENT PROGRAM | .
STATEMENT -> EXPRESSION | BLOCK | EMPTY_LINE |  comment .
EXPRESSION -> VALUE PARAMLIST NL. 
PARAMLIST -> PARAMETER PARAMLIST | .
PARAMETER -> BLOCK | VALUE .
EMPTY_LINE -> NL .
BLOCK -> do PROGRAM end | ob PROGRAM cb | ( PROGRAM ) | oa PROGRAM ca.
NL -> nl | semicolon .
VALUE -> string | integer | float | symbol | operator .

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
=begin idea for possible later extension that actually parses operations :
MAIN -> PROGRAM eof.
PROGRAM -> STATEMENT PROGRAM | .
STATEMENT -> EXPRESSION | BLOCK | EMPTY_LINE.
EXPRESSION -> VALUE PARAMLIST_OR_OP NL. 
PARAMLIST_OR_OP -> OP_EXPR | PARAMLIST .
OP_EXPR -> OPERATOR EXPRESSION .
OPERATOR -> operator | period | comma | colon .
PARAMLIST -> PARAMETER PARAMLIST | .
PARAMETER -> BLOCK | VALUE .
EMPTY_LINE -> NL .
BLOCK -> do PROGRAM end .
NL -> nl .
VALUE -> string | integer | float | symbol  .
=end


module Raku
  class Parser
  
    IGNORE = [ :ws, :esc_nl]
  
    attr_reader :token
    attr_reader :result
    
    # Creates a new node
    def node(kind, value = nil)
      return Node.new(kind, value)
    end
    
    # Creates a new failiure
    def fail_node(message)
      node = Node.new(:error, message)
      node.fail!(message)
      return node
    end
  
    def initialize(program)
      @lexer  = Lexer.new(program)
      @result = node(:root)
      @token  = nil
      advance #advance once on initializing the parser
    end
    
    # updates the current token and returns it
    def advance
      @token  = @lexer.lex_skip(*IGNORE)
      return give_up("Lex error") if @token.fail?
      return @token
    end
   
    # give up parsing
    def give_up(reason="")
      msg = "Parse Error: #{reason}" 
      if @token 
      msg = "Parse Error in #{@token.kind} at #{@token.line}:#{@token.col}:\
       #{reason}"
      end
      # we use throw here in stead of exceptions since a parse error is 
      # not exceptional, but expected.
      # throw(:parse_error, fail_node(msg))
      return fail_node(msg)
    end
    
    # returns the kind of the current token
    def token_kind
      return nil unless @token 
      return @token.kind
    end
    
    # returns nil if the token's kind id not kind. 
    # otherwise returns the current token.
    def have?(*kinds)
      return self.token if kinds.member?(self.token_kind)
      return nil
    end
    
    # checks if the token kind is in kinds, 
    # if not, returns nil and does nothing else.
    # if it is, returns a new node containing 
    # the current token, and then advances parsing
    # by calling advnce
    def accept(*kinds)
      return nil unless have?(*kinds)
      res = node(@token.kind, @token.value)
      self.advance
      return res
    end
    
    # calls accept, but calls give_up if accept returns nil
    def expect(*kinds)
      res = accept(*kinds)
      return res if res
      return give_up("Expected one of #{kinds}")
    end
    
    def parse_value
      aid = accept(:integer, :float, :string, :symbol, :operator, :colon,
:comma, :period)
      return aid if aid
      return nil
    end

    def parse_nl
      return accept(:nl, :semicolon)
    end
    
    def parse_block_in(open, close)
      return nil unless accept(open)
      res = node(:block, open) 
      pro = parse_program
      pro.children.each { |child| res << child }
      # was res << pro, but the above removes the useless program in the block
      return nil unless accept(close)
      return res
    end
    
    def parse_block
      return parse_block_in(:lcurly, :rcurly) || 
             parse_block_in(:lparen, :rparen) ||
             parse_block_in(:lbracket, :rbracket)
    end

    def parse_parameter
      aid = parse_value 
      return aid if aid
      aid = parse_block 
      return aid if aid
      # Argument must be a basic or a block. If not, fail
      return give_up("Parameter expected")
    end
    
    def parse_paramlist
      res   = node(:paramlist)
      until have?(:nl, :semicolon)
        return give_up("Unexpected end of file.") if have?(:eof) 
      # end of paramlist on nl or semicolon
        param = parse_parameter
        return give_up("Missing parameter.") unless param
      # if it's not the end, parse a parameter
      # Followed by a paramlist
        res << param
      end  
      return res
    end  
         
    def parse_blank
      return node(:blank) if parse_nl
      return nil
    end

    def parse_expression
      result = node(:expression)
      val = parse_value
      return nil unless val
      result << val
      params = parse_paramlist
      return give_up("Missing parameters.") unless params
      result << params
      got = expect(:nl) # there must be an nl after all params
      return got if got.fail?
      return result
    end
    
    def parse_statement
      aid = parse_expression || parse_block || parse_blank || accept(:comment)
      return aid if aid
      return give_up("Could not parse statement.")
    end 
  
    
    def parse_program
      prog = node(:program)
      until have?(:rcurly, :rbracket, :rparen, :eof)
        stat = parse_statement # NOT expression!
        return give_up("Could not parse statement") unless stat
        prog << stat unless stat.kind == :blank
        # don't add blanks to program 
      end
      return prog
    end
    
    def parse_raku
      error = catch(:parse_error) do
        prog = parse_program
        got  = expect(:eof)
        return got if got.fail?
        return prog
      end
      warn "Parse error!"
      return error
    end
    
    def parse
      res = parse_raku
      return res
    end
    
  end # class Parser
end # module Raku
