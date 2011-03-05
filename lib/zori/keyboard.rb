module Zori
  # Class that models the Keyboard state
    class Keyboard
      attr_reader :keys, :mods
      def initialize
        @keys = {}
      end
      
      def press(scancode, mod, text)
        # Reuse Keyinfo object, to lessen memory use
        unless @keys[scancode]  
          info            = Keyinfo.new()
          @keys[scancode] = info
        end
        @keys[scancode].pressed = Time.now
        @keys[scancode].sym     = scancode
        @keys[scancode].mod     = mod
        @keys[scancode].text    = text
        @keys[scancode].repeated= nil  
      end
      
      # Returns the keyinfo for the given scancode, or nil if not pressed yet.
      def state(scancode)
        return @keys[scancode]
      end
      
      # Returns a Time when the key was last pressed
      def pressed?(scancode) 
        s = self.state(scancode)
        return false unless state
        return state.pressed
      end
      
      # Registers a key release 
      def release(scancode)
        return unless @keys[scancode]
        @keys[scancode].pressed = nil 
        @keys[scancode].repeated= nil
      end
      
      # Returns keys that need to be repeated
      def to_repeat()
        result = []
        now   = Time.now
        for k, value in @keys do
          if value.pressed 
            if ! value.repeated && ((now - value.pressed) > Hanao::REPEAT_TIME) 
            # First key repeat 
              result << value
            elsif value.repeated && ((now - value.repeated) > Hanao::REPEAT_SPEED)
            # Consecutive key repeats
              result << value
            end  
          end    
        end
        return result
      end
      
    end
end # module Zori