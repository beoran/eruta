# encoding: utf-8
require 'test_helper.rb'
require 'bump'


assert { Bump }
assert { Bump::Lookup }
l = Bump::Lookup.new
assert { l }

