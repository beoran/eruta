# PMF: poor man's fibers, using exceptions


class PMF
  class Busy < Exception
    attr_reader :todo
    
    def initialize(&block)
      super()
      @todo = block
    end
  end
  
  class Done < Exception
    attr_reader :result
    def initialize(result)
      super()
      @result = result
    end
  end
  
  def initialize(&block)
    @block    = block
    @to_redo  = nil
    @has_run  = false 
  end
  
  def run    
    result        = self
    begin
      if @to_redo 
        result    = @to_redo.call
      elsif @block        
        result    = @block.call
        # if we get here, the block ran to completion. 
        @to_redo  = nil # Nothing to redo anymore
        @block    = nil   # empty block so we know we're done.
      else 
        return nil  
      end  
      rescue PMF::Busy => ex
        @to_redo  = ex.todo
        # retry # don't redo what needs to be redone.
      rescue PMF::Done => ex
        result = ex.result
        @to_redo = nil # Nothing to redo anymore
        @block   = nil # empty block se we know we're done.
    end
    return result
  end
  
  def self.busy(&block)
    raise PMF::Busy.new(&block)
  end
  
  def self.done(*result)
    raise PMF::Done.new(result)
  end
  
  

end

fib = PMF.new do
  PMF.busy do
    puts "I'm busy now"
    PMF.busy do
      puts "I'm still busy now"
      PMF.done("Not busy anymore!")
    end  
  end
end

p PMF

r = fib.run
p r
r = fib.run
p r
r = fib.run
p r
r = fib.run
p r



