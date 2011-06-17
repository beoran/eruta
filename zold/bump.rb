#
# Bump, a module for swept collision detection.
#
#
require 'gari'

module Bump
  def self.vec(x, y)
    Vector.new(x, y)
  end
  
  def self.vec0
    Vector.zero
  end
 
  # New idea:
  # Floats can model everything, however, they make the complications 
  # more complex. Would it help to define speed in terms of frames?
  # The logic could then run at say, 32 Hz. 
  # A speed of 1 is then a motion of 32 pixels per second. 0.5 is updated only 
  # once every 16 frames, 0.24 every 8 frames, etc.
  # Like that, the state of the game only has to be updated a few times
  # per second and not every frame, and also not for every object.
 
 
  autoload :Aline,  'bump/aline'
  autoload :Thing,  'bump/thing'
  autoload :Mobile, 'bump/mobile' 
  autoload :Box,    'bump/box'
  autoload :Lookup, 'bump/lookup'
  autoload :Space , 'bump/space'
  autoload :Vector, 'bump/vector'

end

