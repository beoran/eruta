#
#
# Neotoma: a Packrat Parser and Parser Generator 
#
require 'singleton'
require 'strscan'


module Neotoma

  # A result from parsing. It's a node that has a tree-like structure
  class Result
    attr_reader :children
    attr_reader :parent
    attr_reader :value
    attr_reader :name  
  
    def initialize(name, value = nil, parent = nil)
      @name     = name 
      @value    = value
      @parent   = parent
      @children = []
    end
    
    # Returns true if this result node has no children
    def leaf?
      return @children.size < 1
    end
    
    # Returns true if this is a branch result node that has children
    def branch?
      return !self.leaf?
    end
    
    # Adds a child to this result if it is not nil nor false
    def add_child(child)
      return nil   unless child
      @children << child
      return       child
    end
    
    # Creates a new child for this result  
    def new_child(name, value, klass = Neotoma::Result)
      child     = klass.new(name, value, self)
      return      add_child(child) 
    end
    
    # Walks the result tree recursively, depth-first
    def walk(&block) 
      for child in @children do
        child.walk(&block)
      end
      return block.call(self) 
    end
    
    
  end 

  # The scanner wraps the input string
  class Scanner < StringScanner
    attr_reader :checkpoints
  
    def initialize(string)
      super(string)
      @checkpoints = [] # Stack of checkpoints
    end
   
    # Commits the current position as a checkpoint. Returns self.
    def commit()
      @checkpoints << self.pos
      return self
    end
       
    # Rolls back to the previous checkpoint. returns self. 
    # Raises an exception if the checkpoit stack is empty. 
    def rollback()
      oldpos    = @checkpoints.pop
      raise "Checkpoint stack underflow in scanner!" unless oldpos
      self.pos  = oldpos
      return self    
    end
    
    # Scans for the pattern, and commits the position if the sacn was sucessful.
    # Returns the result of the scan
    def scan_commit(pattern)
      result = self.scan(pattern)
      commit() if result
      return result
    end 
   
  end 

  # A general parser rule 
  class Rule
    attr_reader :name
    def initialize(name)
      @name = name
    end
    
    # Cached parse of the input for this rule
    def parse(scanner, cache = {})
      cache_key           = "#{self.object_id}:#{scanner.pos}"
      cached, cached_pos  = cache[cache_key]
      if cached
        scanner.pos       = cached_pos 
        return cache
      else    
        result            = parse_real(scanner)
        cache[cache_key]  = [ result, scanner.pos ]
        return result
      end  
    end
    
    # Parses the input for real
    def parse_real(input)
      return nil
    end
    
    # converts a literal to a literal rule, but leaves a rule a rule
    def self.to_rule(other_rule)
      return other_rule if other_rule.is_a? Rule
      other_rule = Literal.new("Literal: #{other_rule}", other_rule)
    end
    
    def | (other_rule)
      other_rule = self.class.to_rule(other_rule)
      return Alternate.new("#{self.name}|#{other.name}", self, other_rule)       
    end
    
    def & (other_rule)
      other_rule = self.class.to_rule(other_rule)
      return Sequence.new("#{self.name}|#{other.name}", self, other_rule)       
    end

  end
  
  # A literal (regexp, string or number) rule
  class Literal < Rule 
    def initialize(name, literal) 
      super(name)
      if literal.is_a Regexp
        @literal = literal
      else
        @literal = Regexp.new(literal.to_s)
      end  
    end 
    
    def parse_real(scanner)
      found = scanner.scan_commit(@literal)
      return found
    end
    
  end
   
  # Sequence of alternatives
  class Sequence  < Rule
    def initialize(name, *rules)
      super(name)
      @rules = rules 
    end
    
    def parse_real(scanner)
      for rule in @rules do
        subres = rule.parse(scanner)
        unless subres
          scanner.pos = scanner
        end
      end  
      found = scanner.scan_commit(@literal)
      return found
    end
    
        
  end
  
  # Choice between two or more alternatives
  class Alternate < Rule
    def initialize(name, *rules)
      super(name)
      @rules = rules 
    end
    
    def parse_real(scanner)
      for rule in @rules do
        subres = rule.parse(scanner)
        if subres
          scanner.scan_commit(@literal)          
          return subres
        end
      end  
      return nil
    end
    
  end
  
  # Empty match
  class Empty  < Rule
    include Singleton
    def initialize(name)
      super('(empty)')
    end
  end
  
  # Negative or positive lookahead
  class Lookahead < Rule
    def initialize(name, rule, positive = true)
      super(name)
      @rule       = rule
      @positive   = positive
    end
  end
  
  # End of parser input 
  class Endstream < Rule
    include Singleton 
    def initialize(name)
      super('(end of stream)')
    end
  end
  
  # Repetition (*, +, {n, m})
  class Repetition < Rule
    def initialize(name, rule, minimum = 0, maximum = nil)
      super(name)
      @rule       = rule
      @minimum    = minimum
      @maximum    = maximum
    end
  end
 
  class Parser
    def rule(name)
    end
  
  
  end 
 
end



if $0 == __FILE__
  
s = %q{
foo bar baz
fum do 
  foo bar baz
  foo bar baz
end
}

ss = Neotoma::Scanner.new(s)
p ss.scan_commit(/\W/)
p ss.scan_commit(Regexp.new('foo'))
p ss.scan_commit(Regexp.new('foo'))
p ss.pos 
p ss.checkpoints
ss.rollback
ss.rollback
p ss.pos
p ss.scan_commit(Regexp.new('foo'))
 
 
end
