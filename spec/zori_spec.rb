require 'helper'
require 'zori'


describe Zori, "gui library" do
  include Helper::Common
  
  before(:all) do
    setup() 
  end
  
  after(:all) do
    teardown
  end


  it "should be constructable" do
    # p Sisa::Joystick.methods.sort - Object.methods
    ui     = Zori.open(@screen, @queue)
    ui.should_not be_nil
  end


end



