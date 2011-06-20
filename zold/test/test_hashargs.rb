require 'test_helper'

require 'hashargs'

module ModA
  def initialize(*args)
    super(*args)
    puts "In ModA"
  end
end

module ModB
  def initialize(*args)
    super(*args)
    puts "In ModB"
  end
end

class MixClass
  include ModB
  include ModA
  
  def initialize()
    super()
  end
  
end

class TryHashargs
  include Hashargs
  
  attr_reader :x
  attr_reader :y
  attr_reader :z
  
  def initialize( hash = {} )
    hash_to_ivar(hash, x: 1, y: 2)
    @z = "you can't touch this"
  end
 
end


t = TryHashargs.new()
assert { t.x == 1 }
assert { t.y == 2 }
assert { t.z == "you can't touch this" }

t = TryHashargs.new(y: 20, x: 10, z: "should never be set")
assert { t.x == 10 }
assert { t.y == 20 }
assert { t.z == "you can't touch this" }

m = MixClass.new

