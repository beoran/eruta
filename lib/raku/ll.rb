module Raku
  class LL
    class Term
      attr_reader :name
      def initialize(name)
        @name   = name.to_sym 
      end
    end
    
    class Or < Term
      attr_reader :list
      def initialize(name, *list)
        super(name)
        @list = list
      end  
      
      def inspect
        return @list.join('|')
      end  
    end 
    
    class And < Term
      attr_reader :list
      def initialize(name, *list)
        super(name)
        @list = list
      end  
      
      def inspect
        return @list.join('&')
      end
    end 
    
    attr_reader :rules
    
    def initialize
      @rules     = {}
    end
    
    def add_rule(rule)
      @rules[rule.name] = rule
      return rule
    end
    
    def term(name)
      add_rule(Term.new(name))
    end
    
    def choose(name, *list)
      add_rule(Or.new(name, *list))
    end
    
    def list(name, *list)
      add_rule(And.new(name, *list))
    end
    
    def is_term?(rule)
      rule.downcase == rule
    end
    
    def first_set_for(nonterm)
      rule = @rules[nonterm]
      return [ nonterm ] if !rule
      if rule.is_a? And 
        return first_set_for(rule.list.first)
      end
      if rule.is_a? Or
        result = [] 
        for sub in rule.list
          aid = first_set_for(sub)
          result += aid
        end
        return result  
      end
      return [ nonterm ]
    end
    
        
# Initialize every Fo(Ai) with the empty set
# if there is a rule of the form Aj → wAiw' , then
#    * if the terminal a is in Fi(w' ), then add a to Fo(Ai)
#    * if ε is in Fi(w' ), then add Fo(Aj) to Fo(Ai)
# repeat step 2 until all Fo sets stay the same.

    def follow_set_for(nonterm)
      set = []
      @rules.each do |name, rule|
        if rule.is_a? And
          ind = rule.list.index(nonterm)
          if ind
            rest   = rule.list[ind + 1]
            if rest
              follow = nil
              follow = rest.first if rest.respond_to? :first
              follow = rest
              aid  = first_set_for(follow)
              if(aid.member? :empty)
                set += follow_set_for(name)
              end
              aid -= [:empty]
              set += aid
            else
              set += follow_set_for(name)
            end
          end
        end
      end
      return set
    end
    
    def all_terminals()
      result = []
      @rules.each do |name, rule|
        if rule.is_a?(And) || rule.is_a?(Or)
          result += rule.list.select { |e| @rules[e].nil? } 
          # add all terminals in every rule
        else
          result << rule
        end
      end
      result -= [:empty]
      return result.sort.uniq
    end  

    def all_nonterminals()
      result = []
      @rules.each do |name, rule|
        if rule.is_a?(And) || rule.is_a?(Or)
          result += rule.list.reject { |e| @rules[e].nil? } 
          # add all terminals in every rule
        else
          result << rule
        end
      end
      result -= [:empty]
      return result.sort.uniq
    end  
    
    
    def make_parsing_table
      table = {} 
      @rules.each do |name, rule|
        first = first_set_for(name)
        first.each do | terminal |
          next if terminal == :empty
          table[name] ||= {}
          table[name][terminal] = rule
        end
        if first.member? :empty
          follow = follow_set_for(name)
          follow.each do | terminal |
            next if terminal == :empty
            table[name] ||= {}
            table[name][terminal] = rule
          end
        end
      end
      return table  
    end 
    
    def parse(start = :program, *tokens)
      result = []
      @table = make_parsing_table
      @stack = [ :'$' , start ]
      token  = tokens.shift
      last   = nil
      node   = nil
      top    = @stack.pop
      while token do        
        p token.kind, top
        raise "Parse error: Parse stack empty." unless top
        # Match, so store the result.
        if token.kind == top
          node   = Node.new(token.kind, token.value, last)
          result << last
          last   = node
          top    = @stack.pop
          p @stack
        else
          ruleaid = @table[top]
          unless ruleaid
            raise "Parse error: Unexpected #{token.inspect} for #{top}."
          end
          rule = ruleaid[token.kind]
          raise "Parse error: Unexpected #{token.inspect}. (#{top})" unless rule
           
          if rule.is_a?(Or) || rule.is_a?(And) 
            # push rule's list on the stack
            rule.list.to_a.reverse.each { |e| @stack.push(e) } 
          else
            @stack.push(rule)
          end 
          p @stack
        end
        token  = tokens.shift
      end
      return result
    end
  
    
  end 
end