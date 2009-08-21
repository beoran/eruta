#!env ruby1.9
require 'test_common'

module Test 
  class Color < Common
    def test_parse_hex
      res = Sisa::Color.parse_hex("def2")
      assert_equal res, [221, 238, 255, 34]
    end
    
    def test_initialize
      color1 = Sisa::Color.new(221,238,255,34)
      color2 = Sisa::Color.new("def2")
      color3 = Sisa::Color.new(221,238,255,34)
      assert_equal color1, color3
      assert_equal color2, color3
    end    
  end
end  




