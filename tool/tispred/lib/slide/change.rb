#  Wrapper for ChangeListeners


require 'swing'
require 'slide'

module Slide  
  module Change
    # Adapter for swing action events
    class Adapter
    include Swing::ChangeListener
    
      def initialize()
        @action = nil
      end

      def change(&block)
        @action = block     
      end

      def stateChanged(event)
        if @action
          source = event.getSource()
          @action.call(source)
        end
      end
      
    end
    
    # A module that helps in handling objects, to be included in a class
    # that also included Slide::Wrapper   
    module Handler    
      # Enables change handling
      def enable_change(block = nil)
        return @change if @change 
        @change   = Slide::Change::Adapter.new()
        if block
          @change.action do |source| block.call(source) end
        end
        @swing.addChangeListener(@change)
        return @change
      end

      # Gets the change handler
      def change(&block)
        return enable_action(name, block) 
      end
    end
    
    # A module that helps in handling objects, to be included in a class
    # that also included Slide::Wrapper   
    module ColorModelHandler    
      # Enables change handling
      def enable_change(block = nil)
        return @change if @change 
        @change   = Slide::Change::Adapter.new()
        if block
          @change.action do |source| block.call(source) end
        end
        @swing.getSelectionModel().addChangeListener(@change)
        return @change
      end

      # Gets the change handler
      def change(&block)
        return enable_action(name, block) 
      end
    end
    
  end    
end

