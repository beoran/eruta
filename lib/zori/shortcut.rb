#
#
# Shortcut is a mix in module that helps with implementing 
# keyboard shotcuts. Don't forget to call shortcut_key(key)
# in your on_key function. d 
# 

module Zori
  module Shortcut
    
    # defines a shortcut
    def shortcut(key, method=nil, *args, &block)  
      @shortcuts ||= {}
      unless block
        block = lambda do | *largs| 
          self.send(method, *largs)
        end
      end
      @shortcut[key] = [ block, args ] 
    end
    
    # Call this in your on_key function. 
    # It will return true if the shortcut was handled, and 
    # false if it wasn't.
    def key_shortcut(key)
      short = @shortcut[key]
      return false unless short
      to_call, args = *short
      to_call.call(self, *args)
      return true
    end
  
  end
end