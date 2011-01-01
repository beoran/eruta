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
    end
    
    def want(kind)
      # XXX: still buggy somewhere...
      # the problem is that there is no function for look ahead ad if it
      #is not what i want, even eof, fail and rollback the lexer (unscan)
      unless @next # scan for the first time
        @now    = nil
        @next   = @lexer.lex_skip(*IGNORE)
      end
      if @next.fail?
        raise "Lex error: at #{@next.line}:#{@next.col}"
      end
      
#       if @next.eof? && kind != :eof
#         raise "Parse error: unexpected #{@next.kind} at
#{@next.line}:#{@next.col}"
#       end  
      if @next.kind == kind
        # all ok, advance
        advance
        return @now
      else
        # not ok, don't advance.
        return nil
      end
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
      return nil
    end
    
    def parse_arguments
      result = node(:arguments)
      aid    = parse_argument
      return result unless aid
      while aid
        result << aid if aid
        aid = parse_argument
      end
      return result 
    end
    
    def parse_empty_statements
      aid = want(:nl)
      while aid  
        aid = want(:nl)
      end
    end
    
    def parse_statement
      result = node(:statement)
      aid     = want(:nl)
      return result if aid
      warn 'stat not empty'
      # handle empty statements
      aid    = parse_basic
      return nil unless aid
      warn 'stat starts with basic'
      result << aid
      aid    = parse_arguments
      return nil unless aid
      warn 'stat args ok'
      result << aid
      aid    = want(:nl)
      return nil unless aid
      warn 'stat ends with newline'
      result << aid
      return result
    end
    
    def parse_statements
      result = node(:statements)
      res   = parse_statement
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
      parse_empty_statements
      aid    = parse_statements
      return nil unless aid  
      result << aid
      aid    = want(:rcurly)
      return nil unless aid
      result << aid
      return result 
    end
    
    def parse_block_or_statements
      parse_empty_statements
      res = parse_block
      return res if res
      res = parse_statements
      return res if res
      raise "Could not parse statements or blocks." 
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
