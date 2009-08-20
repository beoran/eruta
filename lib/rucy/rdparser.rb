
class Program
  class Node
    attr_reader :rule
    attr_reader :word
    def initialize(rule, word)
      
    end    
  end
end


class Parserule 
  def initialize(name, optional=false, multiple=false)
    @name     = name
    @listparts= []
    @orparts  = [] 
    @optional = optional
    @multiple = multiple
  end 
    
  def add_listpart(part)
    @listparts << part
  end
  
  def add_orpart(part)
    @orparts  << part
  end
  
  def match_orparts(words)
    @orparts.each do | part |
      return true if part.match(words)
    end
    return false
  end
  
  def match_listparts(words)
    index = 0
    @listparts.each do | part |
      word = words[index]
      return false unless part.match(word)
      index += 1
    end
    return true
  end
  
  def match_token(words)
    word = words[0]
    
  end
  
  def match(words)
    return match_orparts(words) if(@orparts)
    return match_listparts(words) if(@listparts)
    
  
  end    
  
end



class Parser
  class Rule
    def initialize(name, parser)
      @name      = name.to_sym
      @parser    = parser
    end
    def match(tok)
      return false
    end
  end

  class TerminalRule < Rule
    def initialize(tok, parser)
      super tok.name, parser 
      @token    = tok
    end
    
    def match(word)
      return (word.token == @token)
    end 
  end
  
  
  class SimpleRule < Rule
    def initialize(name, parser)
      super name, parser 
      @parts    = []
    end 
     
    def add_part(part)
      @parts << part
    end
    
    def match(words)
      @parts.each do |part|
        return false unless part.match(words)
      end
      return true
    end    
  end
  
  class CompoundRule < Rule
    def initialize(name, parser)
      super name, parser 
      @subrules    = []
    end
  end
  

  attr_accessor :lexer
  def initialize(lexer)
    @start_rule = Rule.new(:start, self)
    @now_rule   = @start_rule  
  end



end













class RDParser
   attr_accessor  :pos
   attr_reader    :rules

   def initialize(&block)
     @lex_tokens    = []
     @named_tokens  = []
     @rules = {}
     @start = nil
     instance_eval(&block)
   end

   def parse(string)
     @tokens = []
     until string.empty?
       raise "unable to lex '#{string}" unless @lex_tokens.any? do |tok|
         match = tok.pattern.match(string)
         if match
           @tokens << tok.block.call(match.to_s) if tok.block
           string = match.post_match
           true
         else
           false
         end
       end
     end
     @pos = 0
     @max_pos = 0
     @expected = []
     result = @start.parse
     if @pos != @tokens.size
       raise "Parse error. expected: '#{@expected.join(', ')}', found '#{@tokens[@max_pos]}', #{result.inspect}"
     end
     return result
   end

   def next_token
     @pos += 1
     return @tokens[@pos - 1]
   end

   def expect(tok)
     t = next_token
     if @pos - 1 > @max_pos
       @max_pos = @pos - 1
       @expected = []
     end
     return t if tok === t
     @expected << tok if @max_pos == @pos - 1 && !@expected.include?(tok)
     return nil
   end

   private

   LexToken = Struct.new(:name, :pattern, :block)

   def token(name, pattern, &block)
     @lex_tokens << LexToken.new(name, Regexp.new('\\A' + pattern.source), block)
     
   end

   def start(name, &block)
     rule(name, &block)
     @start = @rules[name]
   end

   def rule(name)
     @current_rule = Rule.new(name, self)
     @rules[name] = @current_rule
     yield
     @current_rule = nil
   end

   def match(*pattern, &block)
     @current_rule.add_match(pattern, block)
   end

   class Rule
     Match = Struct.new :pattern, :block

     def initialize(name, parser)
       @name = name
       @parser = parser
       @matches = []
       @lrmatches = []
     end

     def add_match(pattern, block)
       match = Match.new(pattern, block)
       if pattern[0] == @name
         pattern.shift
         @lrmatches << match
       else
         @matches << match
       end
     end

     def parse
       match_result = try_matches(@matches)
       return nil unless match_result
       loop do
         result = try_matches(@lrmatches, match_result)
         return match_result unless result
         match_result = result
       end
     end

     private

     def try_matches(matches, pre_result = nil)
       match_result = nil
       start = @parser.pos
       matches.each do |match|
         r = pre_result ? [pre_result] : []
         match.pattern.each do |token|
           if @parser.rules[token]
             r << @parser.rules[token].parse
             unless r.last
               r = nil
               break
             end
           else
             nt = @parser.expect(token)
             if nt
               r << nt
             else
               r = nil
               break
             end
           end
         end
         if r
           if match.block
             match_result = match.block.call(*r)
           else
             match_result = r[0]
           end
           break
         else
           @parser.pos = start
         end
       end
       return match_result
     end
   end
end



