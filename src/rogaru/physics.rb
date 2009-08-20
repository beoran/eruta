# 
# To change this template, choose Tools | Templates
# and open the template in the editor.

require 'chipmunk'

module Rogaru
  module Physics
    VECTOR_ZERO = CP::Vec2.new(0.0, 0.0)
    INFINITY    = 1.0 / 0.0
    STATIC_BODY = CP::Body.new(INFINITY, INFINITY)
    autoload :World, 'rogaru/physics/world'
    autoload :Myworld, 'rogaru/physics/myworld'
  end
end




