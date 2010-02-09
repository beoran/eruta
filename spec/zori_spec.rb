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
  
  before do 
    @ui = Zori.open(@screen, @queue)    
  end


  it "should be constructable" do
    # p Sisa::Joystick.methods.sort - Object.methods
    @ui.should_not be_nil    
  end
  
  it "should have the same size as the screen" do
    @ui.w.should == @screen.w
    @ui.h.should == @screen.h
  end
  
  it "should be possible to construct widgets" do
    frame = Zori::Frame.new
    frame.w.should == @screen.w
    frame.h.should == @screen.h    
  end
  
  
  
  


end



