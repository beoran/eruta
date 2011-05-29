# The tima module contains (tiled) maps, each of whic represent an area in the 
# Eruta game, or one of the world maps. Includes both the vsual model and the
# Chipmunk based physics.

require 'chipmunk'

module Tima
  autoload :Actor   , 'tima/actor'
  autoload :Layer   , 'tila/layer'
  autoload :Map     , 'tima/map'
  autoload :Player  , 'tima/player'
  autoload :Set     , 'tima/set'
  autoload :Thing   , 'tima/thing'
  autoload :View    , 'tila/view'
  
  # Convenience method that returns a 2d vector.
  def self.vec(x, y)
    return CP::Vec2.new(x, y)
  end
  
  
end

