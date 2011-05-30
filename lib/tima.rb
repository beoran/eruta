# The tima module contains (tiled) maps, each of whic represent an area in the 
# Eruta game, or one of the world maps. Includes both the vsual model and the
# Chipmunk based physics.

# Needs chipmunk for physics.
require 'chipmunk'
# Needs raku for reading and writing data.
require 'raku'

module Tima
  autoload :Actor   , 'tima/actor'
  autoload :Layer   , 'tima/layer'
  autoload :Map     , 'tima/map'
  autoload :Player  , 'tima/player'
  autoload :Set     , 'tima/set'
  autoload :Thing   , 'tima/thing'
  autoload :Tile    , 'tima/tile'
  autoload :Tilelist, 'tima/tilelist'
  autoload :View    , 'tima/view'
  
  # Convenience method that returns a 2d vector.
  def self.vec(x, y)
    return CP::Vec2.new(x, y)
  end
  
  
end

