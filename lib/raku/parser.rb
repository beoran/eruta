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
  
    attr_reader :now
    attr_reader :next
    attr_reader :result
    
    def node(kind, value = nil)
      return Node.new(kind, value)
    end
  
    def initialize(program)
      @lexer  = Lexer.new(program)
      @result = node(:root)
      @now    = nil
      @next   = nil
    end
    
    def advance
      @now    = @next 
      @next   = @lexer.lex_skip(*IGNORE)
      return give_up("Lex error") if @next.fail?
      return @now
    end
   
    def give_up(reason="")
      raise "Parse Error at #{@next.line}:#{@next.col}: #{reason}"
    end
    
    def peek()
      unless @next # scan for the first time
        advance
      end
      return @next
    end
    
    def peek?(kind)
      return self.peek.kind == kind
    end

    def want(kind)
      # XXX: still buggy somewhere...
      # the problem is that there is no function for look ahead ad if it
      #is not what i want, even eof, fail and rollback the lexer (unscan)
      unless @next # scan for the first time
        advance
      end
     
      if @next.kind == kind
        # all ok, advance
        advance
        return @now
      else
        # not ok, don't advance.
        return nil
      end
    end
    
    def one_of(*kinds)
    end
    # raise "Parse error: unexpected #{@now.kind} at #{@now.line}:#{@now.col}"
    
    def parse_basic
      res = want(:integer)
      return res if res 
      res = want(:float)
      return res if res
      res = want(:string)
      return res if res
      res = want(:symbol)
      return res if res
      res = want(:operator)
      return res if res
      return nil
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
      while (!want(:nl) && (!want(:eof)))     
        aid     = parse_argument
        result << aid
      end
      return result 
    end
    
    def parse_empty_statements
      aid = want(:nl)
      while aid  
        aid = want(:nl)
      end
    end
    
    #the bug is here: we have only 1 token lookahead, but this function assumes 
    # unlimited (3) rollback pints. Bad indeed! :p
    def parse_statement
      result = node(:statement)
      # handle empty statements ? 
      aid    = parse_basic
      return nil unless aid
      aid    = parse_arguments
      result << aid
      return result
    end
    
    def parse_statements
      result = node(:statements)
      res    = parse_statement
      return nil if !res
      while res 
        res = parse_statement
        result << res if res
      end
      return result
    end 
    
    def parse_block
      result = node(:block)
      aid    = want(:lcurly)
      return nil unless aid
      result << aid
      # parse_empty_statements
      aid    = parse_statements
      return give_up("No statements in block.") unless aid  
      result << aid
      aid    = want(:rcurly)
      return give_up("No end of block found.") unless aid
      result << aid
      return result 
    end
    
    def parse_block_or_statements
      #parse_empty_statements
      res = parse_block
      return res if res
      p "block or statements:", @now, @next
      res = parse_statements
      return res if res
      return give_up("Could not parse statements or blocks.") 
    end
    
    def parse_program
      result = node(:program)
      res   = parse_block_or_statements
      while res 
        res = parse_block_or_statements
        result << res if res
      end
      return result
    end
    
    def parse
      res = parse_program
      @result << res if res
      return @result
    end
    
  end # class Parser
end # module Raku
