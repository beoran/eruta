# The handler module distributes events coming in though on_event
# to the named methods that correspond to on_event_name.
# IF the method exists. Otherwise returns false. 
module Zori
  module Handler
    def on_event(*args)
      type   = args.shift 
      return false unless type
      method = "on_#{type}".to_sym
      # Try direct handler call.
      if self.respond_to?(method) 
        return self.send(method, *args)
      else 
        # Try handler lookup
#         if self.respond_to?(:handler_for)
#           handler  = self.send(:handler_for, method)
#           return handler.send(method, *args)
#         end
        # Give up.
        return nil
      end
    end
  end
end
