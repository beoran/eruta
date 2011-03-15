#
# Bump, a module for swept collision detection.
#
#
require 'gari'

module Bump
  def self.vec(x, y)
    Vector.vec(x, y)
  end
  
  def self.vec0
    Vector.zero
  end
 
  autoload :Thing,  'bump/thing'
  autoload :Mobile, 'bump/mobile' 
  autoload :Box,    'bump/box'
  autoload :Lookup, 'bump/lookup'
  autoload :Space , 'bump/space'
  autoload :Vector, 'bump/vector'

end