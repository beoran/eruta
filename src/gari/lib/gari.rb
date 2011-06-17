# Ruby extensions to the gari library

# Autoload doesn't work with c esxtensions, so load the ruby extensions manually
require 'gari.so'
require 'gari/color'
require 'gari/handler'
require 'gari/key'
require 'gari/mod'
require 'gari/event'
require 'gari/font'
require 'gari/image'


module Gari
  def self.vec(x, y)
    return Gari::Vector.new(x, y)
  end
  
  def self.rgb(r, g, b)
    return Gari::Color.rgb(r, g, b)
  end
end










