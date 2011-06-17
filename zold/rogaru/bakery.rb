# 
# implementation of Lamport's Bakery Algorythm in Ruby.
# 

module Rogaru
  class Bakery
    module Customer
      attr_accessor :entering
      attr_accessor :number
      attr_accessor :bakery
      def initialize()
        @entering = false
        @number   = nil
      end     
      
      def unlock()
        @number = nil
      end
      
      def locked?()
        return !(@number.nil?)
      end
      
      def lower_number?(other)
        return @number < other.number
      end
      
    end
    
    
    def initialize
      @customers = []      
    end
    
    def lock(customer)
      
      @customers        << customer unless @customers.include? customer
      n                 = @customers.size
      p n
      customer.entering = true
      
      all_numbers       = []      
      for j in 0...n do 
        all_numbers << customers[j].number
      end
      customer.number   = all_numbers.max
      customer.entering = false
      
      
      for j in 0...n do        
        other = @customers[j]
        while (other.entering)  
          yield :entering if block_given?
        end
        while (other.locked?) && ( other.lower_number?(customer)) 
          yield :waiting if block_given?
        end
      end
      yield :critical if block_given?
    end
    
    def unlock(customer)
      @customers.delete(customer)
      customer.unlock
    end
  end # class Bakery
end

$test_int = [ 0 ]
$bakery   = Rogaru::Bakery.new

class Foo
  include Rogaru::Bakery::Customer
  def frob
    $bakery.lock(self) { |v| p v }
    $test_int[0] = $test_int[0] + 1
    puts "#{$test_int} "
    $bakery.unlock(self)
  end
end

f = []
20.times { f << Foo.new }
f.each { |p| Thread.new { p.frob } }


