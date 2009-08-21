#!env ruby1.9
$: << '../src'

require 'minitest/autorun'
require 'sdl'
require 'sisa'

module Test 
  class Color < MiniTest::Unit::TestCase
    def test_parsehex
      res = Sisa::Color.parse_hex("def")
      p res
      pass 
    end
  end
end  




