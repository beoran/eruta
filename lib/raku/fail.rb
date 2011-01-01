module Raku
# A Fail is used for anything that fails.
   module Fail
     def fail!(msg = "")
       @fail_message = msg
       @fail_failed  = true
       return self
     end

     def fail?()
       @fail_failed ||= false
       return @fail_failed
     end

     def fail_message()
       return @fail_message
     end
   end # module Fail
end # module Raku   