# 
# Eruta is copyright 2008-2011 beoran@rubyforge.org.
#


module Bran
  # A Pool is used to schedule Fibers to run in parallel.
  class Pool
  
    # Initializes a fiber pool.
    def initialize(*args)
      @fibers = [] 
    end
    
    # Adds a fiber to the active fiber list of this pool
    # @param [Fiber] fiber to add to pool
    def <<(fiber)
      @fibers << fiber 
    end
    
   # Returns the amount of active fibers.
    def size
      return @fibers.size
    end
    
    # Returns true if the pool has any active members, false if not
    def alive?
      return @fibers.size > 0
    end
    
    # Run every active fiber once in a round-robin fashion.
    # Returns the results the runs of active fibers as a hash
    # and an array of those fibers those that have ended.
    # The fibers that have ended are removed from the Pool's active list.
    def run_once
      ended = []
      result = {}
      return result, ended if @fibers.size < 1
      @fibers.each do |fiber|
        if fiber.alive?
          res = fiber.resume(fiber)
          result[fiber] = res
        else
          ended << fiber
        end  
      end
      # Remove fibers that have ended running. 
      @fibers = @fibers - ended
      return result, ended
    end
    
    # Calls Bran.fiber with the given arguments and adds it to this Pool.
    def fiber(*args, &block) 
      fiber = Bran.fiber(*args, &block)
      self << fiber  
      return fiber
    end
  
  
  end
end  