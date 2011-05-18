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
      @rules = {}
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
          result += aid
        end
        return result  
      end
      return [ nonterm ]
    end
  
    
  end 
end