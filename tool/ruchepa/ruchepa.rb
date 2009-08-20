#!/usr/bin/env ruby
# A simple ruby C header file parser. 
# The header should have been preprocesssed with the gnu cpp tool, using the -P -dD options



# A PEG based parser, currently unoptimized recursive descent 
# To be memoized at some later date.


class Ruchepa
end

# Immutable input list, from string input 
class Ruchepa::Input
  include Comparable  
  attr_reader :index
  
  def initialize(text, index = 0)
    @text   = text
    @index  = index
  end
  
  def to_s
    return @text[@index, @text.length]
  end
  
  # Comparison operator, based upon comparisons of self.to_s and other.to_s
  def <=>(other)
    return self.to_s <=> other.to_s
  end
  
  def finish?
    return (@index >= @text.size)
  end 
  
  def make_tail(offset)
    return Ruchepa::Input.new(@text, @index + offset)
  end
 
  
  def start_with?(str)
    strlen  = str.length
    sub     = @text[@index, strlen]
    ok      = (sub == str)
    offset  = strlen
    return ok, offset
  end
  
  def head(amount = 1)
    unless finish?
      return @text[@index, amount]
    else 
      return nil
    end  
  end
  
  def tail(offset = 1)
    unless finish?
      return make_tail(offset) 
    else
      return nil
    end  
  end
  
  def step(amount = 1)
    return head(amount), tail(amount)  
  end
  
  def match_head(head)
    ok, offset = start_with?(head)
    unless ok
      return nil, self
    end
    return step(offset)
  end
  
  
end


class Ruchepa
  class Token
    def initialize(name, type = :nonterminal, value = nil)
      @name   = name
      @type   = type
      @value  = value 
    end  
  end
end  


class Ruchepa
  class Syntaxtree
    attr_reader :children
    attr_reader :token
    
    def initialize(token, children = [])
      @token      = token
      @children   = children
      @children ||= []
    end
    
    def left_subtree
      return @children.first
    end  
      
    def right_subtree
      return @children[1, @children.size]
    end
    
    def join(new_token, *other)
      new_children     = [self] + other
      return Syntaxtree.new(new_token, new_children)
    end
        
  end # Syntaxtree
end # Ruchepa


class Ruchepa
  # Parsing Expression Grammar Expressions, here dubbed "Rules"
  # All rules are immutable, to allieviate the need for making deep copies
  class Rule
    attr_reader   :name
    attr_accessor :action
  
    class Empty < Rule
      # always returns true, and an empty result, with the input unchanged
      def parse(input)                
        success([], input)
      end
    end
    
    # A terminal (string) 
    class Terminal < Rule
      attr_reader :terminal
      
      def initialize(name, action, terminal)
        super(name, action)
        @terminal = terminal        
      end
      
      def parse(input)
        head, tail = input.match_head(@terminal)
        if head
          return success( [ head ], tail)
        else
          return failed(tail)
        end
      end
      
    end
      
    class Sequence < Rule
      attr_reader :sequence
      def initialize(name, action, *sequence)
        super(name, action)
        @sequence = sequence
      end
      
      def parse(input)
        ok            = true 
        results       = []
        input_aid     = input
        # Try all rules in sequence and see if they are sucessful
        for rule in @sequence do
          ok, result, rest = rule.parse(input_aid)
          unless ok 
            return failed(input)
          end
          results         << result
          input_aid        = rest  
        end  
        return success(results, input_aid) # Sucess if we get here
      end       
    end
    
    class Choice < Rule
      attr_reader :choice
      def initialize(name, action, *choice)
        super(name, action)
        @choice = choice
      end
      # Parses input for an ordered choice       
      def parse(input)
        ok            = true 
        results       = []
        input_aid     = input
        # Try all rules in order and return the first one that is sucessful
        for rule in @choice do
          ok, result, rest = rule.parse(input)
          if ok 
            return success( [ result ], rest ) # Sucess if we get here            
          end
        end  
          return failed(input) # none of the choices was sucessful
      end
    end
    
    class Repeat < Rule
      attr_reader :subrule
      def initialize(name, action, subrule)
        super(name, action)
        @subrule = subrule
      end
      # Rule can be repeated any number of times (hence always a sucess) 
      def parse(input)
        ok            = true 
        results       = []
        input_aid     = input
        # Parse several times with the same subrule
        while ok                 
          ok, result, rest  = @subrule.parse(input_aid)
          if ok 
            input_aid        = rest
            results          << result 
          end  
        end 
        return success( results, input_aid ) # Sucess if we get here             
      end
    end
    
    class Not < Rule
      attr_reader :subrule
      def initialize(name, action, subrule)
        super(name, action)
        @subrule = subrule
      end
      
      # Inverse result of subrule, but return the input unchanged
      # As the not rule is a prefix expression (a negative lookahead)
      def parse(input)
        ok, result, rest = @subrule.parse(input)
        if ok 
          return failed(input)
        else
          return success(result, input)
        end          
      end
    end
    
    def self.make(*parts, &block)
      if parts.nil? || parts.empty?
        return empty(block)
      end
      first       = parts.shift
      first_rule  = to_rule(first) 
      return first_rule.sequence(*parts, &block)       
    end
    
    def self.any(*parts, &block)
      if parts.nil? || parts.empty?
        return empty(block)
      end
      first       = parts.shift
      result_aid  = to_rule(first, &block) 
      result      = result_aid.alternate(*parts, &block)
      p result
      return result
    end
    
    def initialize(name, action)
      @action = action 
      @name   = name.to_s            
    end
    
    # Tries to parse the input. Returns whether or not the parse was OK, 
    # the resulting parse tree, and the remaining input
    def parse(input)
      return failed(input) # Not implemented in top level rule, so fail      
    end
        
    # For a failed return from the parse
    def failed(input)
      return false, nil, input
    end
    
    # For a sucessful return
    def success(result, input)
      return true, result, input
    end
    
    # Generates a rule for a terminal token
    def self.terminal(tok, &block)
      return Terminal.new(tok, block, tok.to_s)
    end
    
    # Generates a rule for an empty token
    def self.empty(&block)
      return Empty.new('(nil)', block)
    end
    
    
    def self.to_rule(other)
      return other if other.is_a? Ruchepa::Rule
      if other.is_a? Range
        othera = other.to_a
        return any(*othera)  
      end  
      return terminal(other.to_s)
    end    
    
    
    def not_predicate(&block)
      act = block || @action
      return Not.new("(!#@name)", act,  self)
    end
    
    def repeat(&block)
      act = block || @action
      return Repeat.new("(#@name*)", act, self)
    end
    
    
    def sequence(*others, &block )
      newname = "#@name" 
      newseq  = [ self ] 
      for aid in others do 
        aidrule = self.class.to_rule(aid)
        newname += (' + ' + aidrule.name)
        newseq  << aidrule
      end            
      return Sequence.new(newname, block, *newseq)
    end
    
    def alternate(*others, &block)
      newname = "#@name" 
      newcho  = [ self ]
      for aid in others do 
        aidrule  = self.class.to_rule(aid)
        newname += (' / ' + aidrule.name)
        newcho  << aidrule
      end
      result = Choice.new(newname, block, *newcho)
      puts result.to_s
      return result
    end
    
    # Operator overloading
    def /(other, &block)
      self.alternate(other, block)
    end
    def +(other, &block)
      self.sequence(other, block)
    end
    
    def to_s
      return "#{self.class.to_s}: #@name"
    end
    
      
  end
  
  
  
  
end  
  



class Ruchepa
  class Parser
    
    
    
  end # Parser
end # Ruchepa





if $0 == __FILE__
  # Built-in testing :)
  require 'ruchepa_test.rb'
end  
