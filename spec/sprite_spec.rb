# Tests screen related functions
require 'helper'
require 'sprite'

=begin
describe Sprite do 
  include Helper::Common
  
  before(:all) do
    setup()
  end
  
  before do
    @sprites          = Rogaru::Sprite::List.load
    @sprite           = @sprites[:fake]
    @sprite.visible   = true 
    @sprite.x         = 10
    @sprite.y         = 20
    @sprite.z         = 1
    @sprite.pose      = :stand
    @sprite.direction = :south
  end
  
  after do
    @sprites = nil
    @sprite  = nil
  end
  
  after(:all) do
    teardown()
  end
  
  it "Should have frames" do
    part =  @sprite.drawing[0]
    frame = part.pose.frames.first
    frame.w.should > 0
    frame.h.should > 0       
  end
  

  it "Should be drawable" do
    @sprite.visible.should    == true 
    @sprite.x.should          == 10
    @sprite.y.should          == 20
    @sprite.z.should          == 1  
    @sprite.pose.should       == :stand
    @sprite.direction.should  == :south
    part                      =  @sprite.drawing[0]
    frames, time, fps = draw_wait do
      wipe_screen
      for layer in 0..3 do 
        @sprites.draw_layer(@screen, layer)
      end
    end
    should_draw(20,50)
  end
end  
=end