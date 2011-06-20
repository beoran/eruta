require 'stringio'
require 'autowatchr'

Autowatchr.new(self) do |config|
  config.ruby     = 'ruby'
  config.lib_dir  = 'lib'
  config.test_dir = 'test'
end



