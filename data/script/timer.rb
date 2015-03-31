
# The Timer module allows timers to be set
module Timer
  
  # Timer class models a normal timer
  class Timer
    # Name of the timer, stored as a symbol
    attr_reader :name
    # Start time in ticks since Eruta engine start
    attr_reader :start
    # End time in ticks 
    attr_reader :stop
    # Delay of timer
    attr_reader :delay
    
    def initialize(name, delay, &to_call)
      @name    = name.to_sym
      @to_call = to_call
      @delay   = delay
      @start   = Eruta.time
      @stop    = @start + delay
      @done    = false
    end
    
    def update()
      now      = Eruta.time
      if now >= @stop
        @done = @to_call.call
        if !@done
          @start   = Eruta.time
          @stop    = @start + @delay
        end
      end
      return @done
    end
    
    def done?
      return @done
    end
  end
  
  
  # Adds a new timer
  # If the callback terurns true, the timer will nor be 
  # called again. If false it wll be called again
  def add_timer(name, delay, &callback)
    @timers ||= {}
    timer = Timer::Timer.new(name, delay, &callback)
    @timers[timer.name] = timer
    return timer
  end
  
  # Gets a timer by name
  def get(name)
    @timers ||= {}
    return @timers[name.to_sym]
  end

  # Updates all timers
  def update
    done_timers = []
    @timers.each do | timer |
      if timer.update
        done_timers << timer
      end
    end
    
    done_timers.each do | timer |
      @timers.delete(timer)
    end
  end
end  
