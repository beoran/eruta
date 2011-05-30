#!/bin/env ruby1.9
#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#

BEGIN { 
  $:.unshift(File.dirname(__FILE__));
  $:.unshift(File.join(File.dirname(__FILE__), '..')) 
}
# Include this script's directory in Ruby's search path.

require 'gari'
require 'fimyfi'
require 'nofont'
require 'zori' # GUI library
require 'tima'


module Eruta
  # Uses Eruta::Program and Eruta::Mainmode, etc to do the job.
  class Eruma < Eruta::Program
  
    def self.main
      obj = self.new
      return obj.main
    end
  
    def initialize
      super(640, 480, false)
    end
  
    def main()
      return 1 unless self.add_mode(Eruta::Editmode, :main)
      self.mode = :main
      self.run
      return 0
    end
  end
end

if __FILE__ == $0
 game = Eruta::Main.new
 # play_music()
 game.main ARGV[0]
end 

# music_thread = Thread.new do test_music() end
# main_thread  = Thread.new do main() end
# main_thread.join
# music_thread.join


