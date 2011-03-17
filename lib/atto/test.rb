module Atto
  # Tiny module for test driven deveopment. Use with include Atto::Test
  module Test
    extend self
    @@failures, @@dots, @@messages = [], [], []
    
    # Returns the file and line of the calling context
    def parse_stack(stack=caller)
      return *(stack.first.match(/(.*):(\d+)/)[1..2])
    end
    
    # Formats a message that reporst on any test failiures
    def format_failure(message, stack = caller)
      # file, line = 
      return "(%s:%0.3d) %s" % [*parse_stack(stack), message]
    end
     
    # Formats an exception backtrace to a string. 
    def format_exception(raised, msg = "")
      return msg + raised.to_s + "\n" + raised.backtrace.join("\n")
    end
    
    # Creates a failiure for the given raised and result state
    # Returns nil if no failure
    def make_failure(res, raised, msg, stack)
      return nil if res
      if raised
        return format_failure(msg || format_exception(raised, "Exception!\n"), stack)
      end       
      return format_failure(msg || "Assertion failed!", stack)
    end
    
    # Makes a dot for the given raised and result state
   def make_dot(res, raised)
     return res ? '.' : ( raised ? 'E' : 'F')
   end
     
   # Calls a block safely with a timeout
   def run_block(delay = 10, &block)
      res, raised = nil, nil
      begin
        Timeout.timeout(delay) do res = block.call ; end
      rescue 
        raised = $!
      end
      return res, raised 
   end  
    
    # Test assertion. 
    def assert(msg=nil, file=nil, line=nil, delay=10, stack=caller, &block)
      res, raised = run_block(delay, &block)
      @@dots     << make_dot(res, raised)
      failure     = make_failure(res, raised, msg, stack)
      @@failures << failure if failure
      if msg         
        @@messages << ( failure ?  msg + " (failed) " : msg)   
      end   
    end

    def self.results #:nodoc:
      aid = @@dots.join + "\n" + @@failures.join("\n") + "\n"
      col = @@failures.empty? ? :green : :red
      aid << Atto::Ansi.color_string(col, "=" * 78)
    end
    
    def self.describe_tests
      @@messages.join(".\n") + ".\n"
    end
  
    at_exit { puts results unless results.strip.empty?; exit @@failures.empty? }
  end
end  
