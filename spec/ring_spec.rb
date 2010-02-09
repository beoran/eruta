
require 'helper'
require 'zori'


describe Zori::Ring, "gui object" do
  include Helper::Common
  
  before(:all) do
    setup
    @ui    = Zori.open(@screen, @queue)
  end
  
  after(:all) do
    teardown    
  end
  
   it "Can be contructed" do
     ring = Zori::Ring.new(@zori, {:foo => :bar})
     ring.should_not be_nil
   end

end
