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

PROGRAM -> STATEMENTS eof.
STATEMENTS -> STATEMENT STATEMENTS | .
STATEMENT -> EXPRESSION | BLOCK | EMPTY_LINE |  comment .
EXPRESSION -> VALUE PARAMETERS NL. 
PARAMETERS_NONEMPTY -> PARAMETER PARAMETERS.
PARAMETERS-> PARAMETERS_NONEMPTY | .
PARAMETER -> BLOCK | VALUE .
EMPTY_LINE -> NL .
DO_BLOCK -> do STATEMENTS end .
BLOCK ->  DO_BLOCK | ob STATEMENTS cb | op STATEMENTS cp | oa STATEMENTS ca.
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
    
    # Creates a new faiiure
    def fail_node(message)
      node = Node.new(:error, message)
      node.fail!(message)
      return node
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
      throw(:parse_error, fail_node(msg))
    end
    
    
    def get_token(tokens)
      tok = tokens.first
      give_up("Unexpected end of file.") unless tok
      return tok
    end
    
    def parse_value(tokens)
      tok = get_token(tokens)
      if [:integer, :float, :string, :symbol, :operator].member? tok.kind
        tokens.shift
        return { tok.kind => tok.value}
      end
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
    
    def parse_statement(tokens, result = nil)
      aid = parse_expression || parse_block || parse_blank || accept(:comment)
      return aid if aid
      return give_up("Could not parse statement.")
    end 
    
    def parse_statements(tokens, result = nil)
      result ||= { }
      result[:statements] = []
      subres = parse_statement(tokens, result)
      
      
      
      
  
    
    def parse_program(tokens, result = nil)
      result ||= { }
      result[:program] = []
      subres = parse_statements(tokens, result[:program])       
      result[:program] << subres if subres
      return result
    end
    
    def parse(tokens, result = nil)
      error = catch(:parse_error) do
        return parse_program(tokens, result)
      end
      warn "Parse error!"
      return error
    end
    
  end # class Parser
end # module Raku
