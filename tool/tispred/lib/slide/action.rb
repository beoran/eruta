# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'awt'

module Slide  
  module Action
    # Adapter for swing action events
    class Adapter
    include Awt::ActionListener
    
      def initialize(name = :action)
        @name   = name
        @action = nil
      end

      def action(&block)
        @action = block     
      end

      def actionPerformed(event)
        if @action
          @action.call(@name)
        end
      end
      
    end
    
  end    
end
