
# The Timer module allows timers to be set
module Timer
  
  # Timer class models a normal timer
  class Timer
    # Name of the timer, stored as a symbol
    attr_reader :name
    # Start time in ticks since Eruta engine start
    attr_reader :start
    # Delay of timer
    attr_reader :delay
    # Total time timer was running
    attr_reader :total
    # Current cycle start
    attr_reader :cycle_start
    # Current cycle stop
    attr_reader :cycle_stop
        
    # Initializes the timer.
    def initialize(name, delay, &to_call)
      @name         = name.to_sym
      @to_call      = to_call
      @delay        = delay
      @start        = Eruta.time
      @cycle_start  = @start
      @cycle_stop   = @cycle_start + delay
      @total        = 0
      @done         = false
    end
    
    # Updates the timer.
    def update()
      now      = Eruta.time
      @total   = now - @start
      if now  >= @cycle_stop
        @done  = @to_call.call(self)
        if !@done
          @cycle_start  = Eruta.time
          @cycle_stop   = @cycle_start + @delay
        end
      end
      return @done
    end
    
    # Checks if timer is done.
    def done?
      return @done
    end
    
    # Forces timer to done.
    def done!
      @done = true
    end
  end
  
  
  # Makes a new timer
  # If the callback terurns true, the timer will not be 
  # called again. If false it wll be called again.
  def self.make(name, delay, &callback)
    @timers ||= {}    
    timer = ::Timer::Timer.new(name, delay, &callback)
    @timers[timer.name] = timer
    p @timers
    return timer
  end
  
  # Gets a timer by name
  def self.get(name)
    @timers ||= {}
    return @timers[name.to_sym]
  end

  # Updates all timers
  def self.update
    @timers ||= {}
    done_timers = []
    @timers.each do | name, timer |
      if timer.update
        done_timers << timer
      end
    end
    
    p @timers unless @timers.empty?
    
    done_timers.each do | timer |
      @timers.delete(timer.name)
    end
  end
end  
