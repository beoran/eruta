module Raku
  class Parser
    class Node
      include Raku::Fail
      
      attr_reader :kind
      attr_reader :value
      attr_reader :children
      
      def initialize(type, value)
        @kind     = type.to_sym
        @value    = value
        @children = []
      end
      
      def << (child)
        @children << child
      end
    end # class Node
  
    IGNORE = [ :ws, :esc_nl]
  
    attr_reader :token
    attr_reader :result
    
    def node(kind, value = nil)
      return Node.new(kind, value)
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
      raise "Parse Error: #{reason}" unless @token    
      raise "Parse Error at #{@token.line}:#{@token.col}: #{reason}"
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
    
    def parse_basic
      return accept(:integer, :float, :string, :symbol, :operator)
    end
    
    def parse_argument
      aid = parse_basic 
      return aid if aid
      aid = parse_block 
      return aid if aid
      # Argument must be a basic or a block. If not, fail
      return give_up("argument expected")
    end
    
    def parse_arguments
      result = node(:arguments)
      # argument lists are optional, but must be ended by a newline
      while (!accept(:nl)) 
        give_up 'Unexpected end of file!' if have?(:eof)
        aid     = parse_argument
        result << aid
      end
      return result 
    end
    
    def parse_empty_statement
      aid = accept(:nl)
    end
    
    #the bug is here: we have only 1 token lookahead, but this function assumes 
    # unlimited (3) rollback pints. Bad indeed! :p
    def parse_statement
      result = node(:statement)
      # handle empty statements ? 
      aid    = parse_basic
      return nil unless aid
      result << aid
      aid    = parse_arguments
      result << aid
      return result
    end
    
    def parse_statements
      result = node(:statements)
      res    = true 
      while res
        res = parse_statement
        result << res if res
      end
      return result
    end 
    
    def parse_block
      result = node(:block)
      aid    = accept(:lcurly)
      return nil unless aid
      result << aid
      # parse_empty_statements
      aid    = parse_statements
      return give_up("No statements in block.") unless aid  
      result << aid
      aid    = expect(:rcurly)
      return give_up("No end of block found.") unless aid
      result << aid
      return result 
    end
    
    def parse_block_or_statement
      res = parse_empty_statement
      p "block or statements:", @token
      return res if res
      res = parse_block
      return res if res
      p "block or statements:", @token
      res = parse_statement
      return res if res
      return give_up("Could not parse statements or blocks.") 
    end
    
    def parse_program
      result = node(:program)
      res    = true # xxx improve this
      while res && !(have?(:eof))
        res = parse_block_or_statement
        result << res if res
      end
      return result
    end
    
    def parse
      p "first token", self.token
      res = parse_program
      @result << res if res
      return @result
    end
    
  end # class Parser
end # module Raku
