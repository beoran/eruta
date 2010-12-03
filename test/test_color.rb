#!env ruby1.9
require 'test_helper'

col  = Gari::Color.parse_hex("def2").to_a
assert {  res [221, 238, 255, 34]
    
    
    def test_initialize
      color1 = Sisa::Color.new(221,238,255,34)
      color2 = Sisa::Color.new("def2")
      color3 = Sisa::Color.new(221,238,255,34)
      assert_equal color1, color3
      assert_equal color2, color3
    end    
  end
end  




