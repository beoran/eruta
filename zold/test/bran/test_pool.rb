if $0 == __FILE__
  $: << '.'
  $: << '..'
  $: << '../..'
  $: << '../../lib'
end  

require 'test_helper'
require 'bran'

assert { Bran::Pool }
pool   = Bran::Pool.new
assert { pool } 

$run_ok_1 = false
$run_ok_2 = false
$run_ok_3 = false

fiber = Bran.fiber do 
  Fiber.yield
  $run_ok_1 = true
end

pool << fiber

assert { pool.size == 1 } 
assert { pool.alive? 	} 

pool.fiber do 
  Fiber.yield
  $run_ok_2 = true
  Fiber.yield  
  $run_ok_3 = true
end

assert { pool.size == 2 } 
assert { pool.alive? 	} 


pool.run_once while pool.alive?
assert { pool.size == 0 } 
assert { !pool.alive? 	} 
assert { $run_ok_1 	}
assert { $run_ok_2 }
assert { $run_ok_3 }


