# module that keeps track of the state of the game
# Just so no global variables are needed...
module State
  class << self
    # Thing the actor is currently talking with.
    attr_accessor :talk_with

    # Dialog box
    attr_accessor :talk_box 

    # GUI state
    attr_accessor :gui

    # Keyboard double keystroke prevention filter
    attr_accessor :last_key_time
    
    # Keyboard double keystroke prevention filter
    attr_accessor :last_key

    # Startup time of the engine
    attr_accessor :time_start

    
  end
end





