
require 'helper'
require 'zori'
require 'zori/ring'


describe Zori::Ring, "gui object" do
  include Helper::Common
  
  before(:all) do
    setup
    @ui    = Zori.open(@screen, @queue)
  end
  
  after(:all) do
    teardown 
  end
  
  before() do
    @ring = Zori::Ring.new()
  end
  
  after do
    @ring.close
    @ring = nil
  end
  
  it "Can be constructed" do
    @ring.should_not be_nil
  end
    
  it "Can add items to itself and display correctly" do
    subr   = Zori::Ring.new
    
    icon1  = Zori::Hanao.load_image('ui', 'icon', 'icon_1.png')
    icon2  = Zori::Hanao.load_image('ui', 'icon', 'icon_2.png')
    icon3  = Zori::Hanao.load_image('ui', 'icon', 'icon_3.png') 
    icon4  = Zori::Hanao.load_image('ui', 'icon', 'icon_4.png')
    icon5  = Zori::Hanao.load_image('ui', 'icon', 'icon_5.png')
    subr.item("Item 4", "Sub ring item 4", icon4)
    subr.item("Item 5", "Sub ring item 5", icon5)
        
    @ring.item("Sub  1", "This should open a subring", icon3, subr)
    @ring.item("Item 1", "This is the text for item 1", icon1)
    @ring.item("Item 2", "This is the text for item 2", icon2)
        
    
    @ui.main << @ring
    @ring.focus!
    
    until @ui.done || @ring.hidden?  do
      @screen.fill(Sisa::Color.new(128,128,255))
      @ui.update
      @ui.draw
      @screen.flip
    end
  end

end
