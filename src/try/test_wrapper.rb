
BEGIN { 
  $:.unshift(File.dirname(__FILE__)); 
  $:.unshift(File.join(File.dirname(__FILE__), '..')); 
  # Include this script's directory in Ruby's 
  # search path as well as parent.
}

require 'settings.rb'
require 'sisa'

require 'zori'
# include Zori


class Testwrapper

  def initialize
    @queue       = Sisa::Event.queue
    @screen      = Sisa::Screen.make(640, 480, false)
    @joysticks   = Sisa::Joystick.open_all
    @gui         = Zori.open(@screen, @queue)
  end
  
  def run
    tests = self.methods.select{ |m| m =~ /\Atest_/ }
    for method in tests
      self.send(method)
    end
    # Call all methods starting with test_    
  end
  
  def self.autorun
    instance = self.new
    instance.run
  end

end




