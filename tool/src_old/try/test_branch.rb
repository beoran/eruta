require 'time'

class FakeEvent
    def foo
      @foo = Kernel.rand(21)
    end
end

class FakeEvent1 < FakeEvent
end

class FakeEvent2 < FakeEvent
end
 
class FakeEvent3 < FakeEvent
end

class FakeEvent4 < FakeEvent
end

class FakeEvent5 < FakeEvent
end

class FakeEvent6 < FakeEvent
end


def ev1
  puts "Event 1"
end

def ev2
  puts "Event 1"
end

def ev3
  puts "Event 1"
end


$ev_lookup = { FakeEvent1 => :ev1 , FakeEvent2 => :ev2, FakeEvent3 => :ev3, 
FakeEvent4 => :ev4 , FakeEvent5 => :ev5, FakeEvent6 => :ev6 }  

def lookup_if(ev)
  klass = ev.class  
  if klass == FakeEvent1
    return :ev1
  elsif  klass == FakeEvent2
    return :ev2
  elsif  klass == FakeEvent3
    return :ev3
  elsif  klass == FakeEvent4
    return :ev4
  elsif  klass == FakeEvent5
    return :ev5
  elsif  klass == FakeEvent6
    return :ev6
  else
    return nil
  end
end

=begin
def lookup_case(ev)
  klass = ev.class  
  case klass
  when FakeEvent1:
    return :ev1
  when FakeEvent2:
    return :ev2
  when FakeEvent3:
    return :ev3
  when FakeEvent4:
    return :ev4
  when FakeEvent5:
    return :ev5
  when FakeEvent6:
    return :ev6
  else
    return nil
  end
end

def lookup_hash(ev)
  klass = ev.class
  return $ev_lookup[klass]
end

events = [ FakeEvent1.new, FakeEvent2.new, FakeEvent3.new,  
          FakeEvent4.new, FakeEvent5.new, FakeEvent6.new, ]


event= FakeEvent1.new

class EventAttr
  attr_accessor :x
  attr_accessor :y
end  

def convert_manually(ev)
  return { :x => ev.x , :y => ev.y }
end

def convert_auto(ev, map)
  result = {}
  for k in map do 
    v = ev.send(k.to_sym)
    result[k] = v
  end  
  return result
end
=end

class Milli
  attr_reader :n
  
  def initialize(i, mul = 1000)
    @n = (i * mul).to_i
  end
  
  def + (o)
    return Milli.new(@n + o.n, 1)
  end

  def - (o)
    return Milli.new(@n - o.n, 1)
  end

  def * (o)
    return Milli.new((@n * o.n) / 1000, 1)
  end

  def / (o)
    return Milli.new((@n * o.n) / 1000, 1)
  end

  def to_i
    return (@n / 1000)
  end 

end

def calc_float(x, vx, time)
    x  += vx * time 
    sx  = x.to_i
    return sx  
end

def calc_int(x, vx, time)
    x  += (vx * time) / 1000 
    sx  = x / 1000
    return sx  
end

def calc_rat(x, vx, time)
    x  += (vx * time) 
    sx  = x.to_i
    return sx  
end

ARVAL_X = 0
ARVAL_Y = 1
ARVAL_Z = 2

def new_arval(x, y, z)
    return [x, y, z]
end

def new_wrapped_arval(x, y, z)
    arr = [x, y, z]
    class << arr # meta-class
      def x
        self[0]    
      end     

      def y
        self[1]    
      end
           
      def z
        self[2]    
      end     

      def x=(o)
          self[0] = o
      end

      def y=(o)
          self[1] = o
      end
       
      def z=(o)
          self[2] = o
      end
    end
    return arr    
end


def calc_arval
    av              = new_arval(3,2,0)
    av[ARVAL_Z]     = av[ARVAL_X] + av[ARVAL_Y]
    return av[ARVAL_Z]
end

class WrapStruct
    attr_accessor :x
    attr_accessor :y
    attr_accessor :z
    def initialize(x, y, z)
        @x = x ; @y = y; @z = z
    end
end


class WrapStruct2
    def initialize(x, y, z)
        @x = x ; @y = y; @z = z
    end
    def x 
        @x
    end
    def x= (o)
        @x = o
    end
    def y 
        @y
    end
    def y= (o)
        @y = o
    end 
    def z 
        @z
    end
    def z= (o)
        @z = o
    end           
end

WRAP_STRUCT = Struct.new(:x, :y, :z)

def calc_wrapped_arval
    av              = WrapStruct.new(3,2,0)
    av.z            = av.x + av.y
    return av.z
end

def calc_struct_arval
    av              = WrapStruct2.new(3,2,0)
    av.z            = av.x + av.y
    return av.z
end

def calc_hash_arval
    av              = { :x => 3, :y => 2, :z => 0 }
    av[:z]          = av[:x] + av[:y]    
    return av[:z]
end


class Task
  attr_accessor :priority
  attr_accessor :fiber
  attr_accessor :block

  def initialize(prior = 0 , &block)
    @priority = prior
    @block    = block
    @fiber    = Fiber.new do
      status, result = self.run_block
    end
  end
  
  def run_block
  
  end

end

class Scheduler
end


=begin

res = 0
x  = 32000 
vx = 1.5
t  = 0.7
start = Time.now
1000000.times do
  res = calc_arval
end
stop = Time.now
puts "#{stop - start}s"
puts res

x  = 32000000 
vx = 1500
t  = 700
res = 0
start = Time.now
1000000.times do
  res = calc_wrapped_arval
end
stop = Time.now
puts "#{stop - start}s"
puts res


start = Time.now
1000000.times do
  res = calc_struct_arval
end
stop = Time.now
puts "#{stop - start}s"



start = Time.now
1000000.times do
  res = calc_hash_arval
end
stop = Time.now
puts "#{stop - start}s"

exit(0)

x  = 32000000000 
vx = 1500000
t  = 700000
res = 0
start = Time.now
10000000.times do
  res = calc_int(x, vx, t)
end
stop = Time.now
puts "#{stop - start}s"
puts res




x   = Milli.new(32000) 
vx  = Milli.new(1.5)
t   = Milli.new(0.7)
res = 0
start = Time.now
10000000.times do
  res = calc_rat(x, vx, t)
end
stop = Time.now
puts "#{stop - start}s"
puts res



ea = EventAttr.new
ea.x = 10
ea.y = 20
map = [:x, :y]

start = Time.now
1000000.times do
  res = convert_manually(ea)
end
stop = Time.now
puts stop - start


start = Time.now
1000000.times do
  res = convert_auto(ea, map)
end
stop = Time.now
puts stop - start



start = Time.now
1000000.times do
  event.foo()
end
stop = Time.now
puts stop - start


start = Time.now
1000000.times do
  event.send(:foo)
end
stop = Time.now
puts stop - start


start = Time.now
1000000.times do
  event   = events[rand(3)]
  to_call = lookup_if(event)
end
stop = Time.now
puts stop - start

start = Time.now
1000000.times do
  event   = events[rand(3)]
  to_call = lookup_case(event)
end
stop = Time.now
puts stop - start

start = Time.now
1000000.times do
  event   = events[rand(3)]
  to_call = lookup_hash(event)
end
stop = Time.now
puts stop - start

# Conclusions : hashes are fast if many event types
# send is only marginally slower than a direct function call
# Integer calcuation is about 3x faster than float calculation, 
# but it will be a bit harder to manage, since we need to 
# multiply and divide with a given fraction. Wrappers are too slow in the case of numbers.
# Also the disadvantage of using Fixnum is that ee may end up in Bignum wich 
# is 8 times slower 

# Speed of structures to hold data:
# Array is the fastest, then, about 1.6 times slower is an object from a class using attr_accessor
# A manually crafted classw ith manually created accessors is about 2.0 times slower than an array
# Struct's and Hash are about equally slow: about 3.0 times slower than arrays.
# All the above was tested on ruby1.9. On ruby1.8, everything runs much slower (about 9 to 10 times),
# hand-made objects are 2 times slower than arrays, but acessors are jyst as fast and hashes are actually 
# slightly faster than arrays! Arrays are about 1.1 times slower there.

=end

  
