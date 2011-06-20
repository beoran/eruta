
require 'test_helper'
require 'atto'

assert "Atto is defined"                        do Atto                      end 
assert "Atto::Test is defined"                  do Atto::Test                end
assert "Atto::Test#describe_tests is defined"   do Atto::Test.describe_tests end
assert "Failure can be detected"                do false                     end

puts Atto::Test.describe_tests



