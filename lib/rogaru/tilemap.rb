#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#
#
#

require 'rexml/document'
require 'rogaru/image'

module Rogaru
  module Tilemap
    autoload :Camera , 'rogaru/tilemap/camera'
    autoload :Entity , 'rogaru/tilemap/entity'
    autoload :Layer  , 'rogaru/tilemap/layer'  
    autoload :Map    , 'rogaru/tilemap/map'
    autoload :Tile   , 'rogaru/tilemap/tile'
    autoload :Tileset, 'rogaru/tilemap/tileset'  
  end 
end  

puts "Loaded Tilemap"

