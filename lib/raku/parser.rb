#
# Parser for the Raku data definition, scripting and programming
# language. Like Lisp or TCL but better. ;-)
# Raku's grammar is a grammar verified to be LL(1) to guarantee parse speed.
# An LL(1) grammar can be parsed by a predictive parser, which runs in 
# linear time, or O(n) time.
#  
# Verified with : http://smlweb.cpsc.ucalgary.ca/start.html
#  
=begin

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
  # A manual predictive parser for the Raku language.
  class Parser
  
    IGNORE = [:ws, :esc_nl]

    # Creates a new failiure
    def fail_node(message)
      node    = Node.new(:error, message)
      node.fail!(message) 
      return node
    end
    
     # give up parsing
    def give_up(reason="", token=nil)
      msg = "End of file: Parse Error: #{reason}" 
      if token 
        msg = "#{token.line}:#{token.col}: " + "Parse error in #{token.kind}: " +  
              reason
        if token.fail?
          msg << " #{token.fail_message}"
        end       
      end
      # we use throw here in stead of exceptions since a parse error is 
      # not exceptional, but expected.
      throw(:parse_error, fail_node(msg))
    end

  
    def initialize()
    end
    
    # Gets a token from the token list. throws :parse_error if no
    # token is on the tokens list
    def fetch(tokens)
      token = tokens.first
      return give_up("Unexpected end of file") if token.nil?
      return give_up("Lex error", token) if token.fail?
      return token
    end
    
    # Gets a token that has a kind in kinds
    def accept(tokens, *kinds)
      token = fetch(tokens)
      if kinds.member?(token.kind)
        tokens.shift
        return { token.kind => token.value }
      end
      return nil
    end
    
    # Gets a token that must be in kinds, or it will raise a parse error
    def need(tokens, *kinds)
      result = accept(tokens, *kinds)
      return result if result
      return give_up("Unexpected token!", tokens.first)
    end
   
    def parse_value_of(tokens, kind, convert)
      res = accept(tokens, kind) 
      return res unless res
      return res[kind].send(convert)
    end
   
    def parse_integer(tokens)
      return parse_value_of(tokens, :integer, :to_i)
    end
    
    def parse_float(tokens)
      return parse_value_of(tokens, :float, :to_f)
    end

    STRING_ESCAPES = {
      'n' => "\n",
      't' => "\t",
      'r' => "\r",
      'a' => "\a",
      'e' => "\e",
    }

    def parse_string(tokens)
      res = accept(tokens, :string) 
      return res unless res
      str = res[:string].to_s
      str = str[1, str.size - 2]
      str.gsub!(%r{\\.}) do |match| 
        char = match[1] 
        aid  = STRING_ESCAPES[char]
        aid || char
      end  
      return str
    end
    
    def parse_symbol(tokens)    
      return parse_value_of(tokens, :symbol, :to_sym)
    end

    def parse_operator(tokens)
      return parse_value_of(tokens, :operator, :to_sym)
    end

    def parse_value(tokens)
      return parse_integer(tokens) || 
             parse_float(tokens)   ||
             parse_string(tokens)  ||
             parse_symbol(tokens)  ||
             parse_operator(tokens)
   #   accept(tokens, :integer, :float, :string, :symbol, :operator)
    end

    def parse_nl(tokens)
      return accept(tokens, :nl, :semicolon)
    end
    
    def parse_block_in(tokens, open, close)
      return nil unless accept(tokens, open)
      pro = parse_statements(tokens)
      res = pro 
      ok  = need(tokens, close)
      return res 
    end
    
    def parse_block(tokens)
      return parse_block_in(tokens, :do       , :end)    || 
             parse_block_in(tokens, :lbrace   , :rbrace) || 
             parse_block_in(tokens, :lparen   , :rparen) ||
             parse_block_in(tokens, :lbracket , :rbracket)
    end

    def parse_parameter(tokens)
      block = parse_block(tokens) 
      return block if block
      value = parse_value(tokens)
      return value if value
      # No parameter there. Params are optional
      return nil
    end
    
    def parse_paramlist(tokens)
      res   = []
      loop do
        if accept(tokens, :eof)
          return give_up("Unexpected end of file in parameter list.", tokens.first)
        end   
        param = parse_parameter(tokens)
        return res unless param
        res << param
      end  
      return res
    end  
         
    def parse_blank_line(tokens)
      return :blank if parse_nl(tokens)
      return nil
    end

    def parse_expression(tokens)
      result = {}
      val    = parse_value(tokens)
      return nil unless val # expression starts with a value 
      result = []
      result << val
      params = parse_paramlist(tokens)
      return give_up("Missing parameters.") unless params
      result += params
      ok     = parse_nl(tokens)
      return give_up("Expected a newline", tokens.first) unless ok
      return result
    end
    
    def parse_statement(tokens)
      aid = parse_expression(tokens) || 
            parse_block(tokens)      || 
            parse_blank_line(tokens) || 
            accept(tokens, :comment)
      return aid if aid
      return nil
    end 
  
    
    def parse_statements(tokens)
      res = []
      stat = parse_statement(tokens)
      while stat 
         # NOT expression!
        unless stat == :blank || (stat.is_a?(Hash) && stat[:comment])
          res <<= stat  
        end        
        # don't add blanks and comments to program
        stat = parse_statement(tokens) 
      end
      res = res.first while res.respond_to?(:first) && res.size == 1
      return res
    end
    
    
    def parse_program(tokens)
      prog = parse_statements(tokens)
      got  = need(tokens, :eof)
      return prog
    end
    
    def hashify(results)
      res = {}
      for line in results do
        if line.is_a? Array
          key = line.shift
          res[key] = hashify(line)
        else
          
        end
      end
    end
    
    # Parses the tokens into a program. 
    # Returns an array of s-expressions and nil.
    # Will return nil and an error object in case of a parse error.
    # Get the error message with fail_message.
    def parse(tokens)
      error = catch(:parse_error) do
        res = parse_program(tokens)
        return res, nil
      end
      return nil, error 
    end
    
    # Parses the text into a program. 
    # Returns an array of s-expressions and nil.
    # Will return nil and an error object in case of a parse error.
    # Get the error message with fail_message.
    def self.parse_text(text)
      parser = Raku::Parser.new
      tokens = Raku::Lexer.lex_all(text, :ws, :esc_nl)
      return parser.parse(text)
    end  

  end # class Parser
end # module Raku
