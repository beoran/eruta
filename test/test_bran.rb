if $0 == __FILE__
  $: << '.'
end  

require 'test_helper'
require 'bran'

class MayBlock
  extend Bran
  include Bran
  attr_reader :state
  
  
  def initialize()
    @state = nil
  end
  
  def may_block
    puts "Before state ok1 #{@state}."
    assert { @state == nil } 
    self.class.wait_for { self.state == :ok1 }
    assert { @state == :ok1 }
    puts "State is now ok1 #{@state}."
    wait_for { self.state == :ok2 }
    assert { @state == :ok2 }
    puts "State is now ok2 #{@state}."
    timeout = 1.1
    before  = Time.now    
    wait_for(timeout) { self.state == :no_way }
    after = Time.now
    # No way is never set, but we should get here thanks to the timeout.
    assert { @state == :ok2 }
    # Check if the timeout was more or less correct (up to 1 %)  
    assert { (after - before) < timeout + 0.01 }
    assert { (after - before) > timeout - 0.01 }
    # Sleeping 
    before  = Time.now    
    sleep_for(timeout)
    after = Time.now
    # Check if the sleep time was more or less correct (up to 1 %)     
    assert { (after - before) < timeout + 0.01 }
    assert { (after - before) > timeout - 0.01 }
  end
  
  
  def on_event(msg)
    puts "Event: #{msg}"
    @state = msg
  end
  
  def on_fiber_event(msg)
    self.on_event(msg)
    Fiber.yield
  end

end


assert { Bran }
assert { Bran::Pool }
pool   = Bran::Pool.new
assert { pool } 

mb = MayBlock.new
assert { mb } 
assert { mb.state.nil? }

pool.fiber do 
  mb.may_block
end

pool.fiber do 
  mb.on_fiber_event(:foo) 
  mb.on_fiber_event(:foo)
  mb.on_fiber_event(:foo)
  mb.on_fiber_event(:ok1)
  mb.on_fiber_event(:foo)
  mb.on_fiber_event(:ok2)
end


pool.run_once while pool.alive?



















