# 
# Eruta is copyright 2008-2011 beoran@rubyforge.org.
#

require 'time'
require 'fiber'

# Bran is a module that helps implementing the Reactor pattern 
# using Fibers, to limit inversion of control issues.
# 
# It allows to wait for a condition without blocking, and has a way to pool 
# fibers together.
#
# 
module Bran
  # A Pool is a pool of Bran fibers.
  autoload :Pool, 'bran/pool'

  # Waits for a given condition block to return non-false or non-nil 
  # in a nonblocking way, or for a given timeout to expire.
  #
  # @param [#to_f, nil] timeout
  #   If timeout is not nil, then it is used as a timeout value.
  #
  # @param args 
  #  The arguments args are passed on to the block yielded to.
  #
  # @yield The method will yield args several times until the block 
  #   yielded to returns true or until timeout. After every call that 
  #   returns false or nil,  Fiber.yield will be called.
  #
  # @return [false, Object] 
  #   Returns false on timeout, otherwise the return value of the block.
  #
  def wait_for(timeout = nil, *args)
    start = Time.now
    fiber = Fiber.current 
    res   = nil
    # Loop until the condition block returns true
    until res
      if timeout
        delta = Time.now - start
        if delta >= timeout.to_f
          return false
        end
      end
      # Check the block.
      res = yield(*args)
      return res if res
      # Yield fiber so cooperative multitasking can happen.
      Fiber.yield(:fibrea_would_block)
    end
    return res
  end
  
  # Sleeps for delay seconds, in a non-blocking way.
  #
  # @param [#to_f] timeout
  #   Amount of seconds to delay. May be a fraction of a second.
  #
  # @return [Fiber] current fiber. 
  def sleep_for(delay = 1.0)
    start = Time.now
    fiber = Fiber.current 
    delay = delay.to_f
    # Loop until the condition block returns true
    begin
      delta = Time.now - start
      # Yield fiber so cooperative multitasking can happen.
      Fiber.yield(:fibrea_would_block)
    end until delta >= delay
  end
  
  # Repeats a given block in a nonblocking way, 
  # until it's last statement is non-false or non-nil, 
  # 
  # @param args 
  #   The arguments args are passed on to the block yielded to.
  #
  # @yield The method will yield args several times until the block 
  #   yielded to returns true. After every call that returns false or nil,  
  #   Fiber.yield will be called.
  #
  # @return [Object] 
  #   Returns the return value of the block.
  #  
  def repeat(*args)
    res = yield 
    until res
      # Check the block.
      res = yield(*args)
      return res if res
      # Yield fiber so cooperative multitasking can happen.
      Fiber.yield(:fibrea_would_block)
    end     
  end
  
  # Creates a Fiber that will immediately call Fiber.yield, and then  
  # yield to the block given. Useful for cooperative threading using fibers.
  #
  # @param args 
  #  The arguments args are passed on to the block yielded to.
  #
  # @yield The method will yield args to the block given.
  #
  # @return [Fiber] 
  #   Returns a new fiber that is ready to be run with Fiber#resume.
  #  
  def self.fiber(*args)
    fiber = Fiber.new do 
      Fiber.yield
      yield(*args)
    end    
    return fiber
  end
  
  # Allow calls as class methods too.
  extend self
  
end