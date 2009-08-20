# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 


module Slide
  module Mouse
    class Adapter
      include Awt::MouseMotionListener 
      include Awt::MouseListener 
      
      def initialize()
        @click    = nil
        @press    = nil
        @release  = nil
        @drag     = nil
        @motion   = nil
        @last_down= nil 
      end
      
      # Translates a swing Mouseevent into a set of simple values.
      def translate(mouse_event)
        sb = mouse_event.getButton()  
        b  = :none
        case sb 
          when Awt::MouseEvent::BUTTON1
            b = :button_1
          when Awt::MouseEvent::BUTTON2
            b = :button_2
          when Awt::MouseEvent::BUTTON3
            b = :button_3
          else   
            b = :button_none
        end
        x = mouse_event.getX()
        y = mouse_event.getX()
        return  b, x, y
      end  
      
      def mouseDragged(mouse_event)       
        b, x, y = translate(mouse_event)
        b = @last_down[0] if @last_down
        if @drag
          @drag.call(b, x, y)
        end
      end  
    
      def mousePressed(mouse_event)
        b, x, y = translate(mouse_event)
        @last_down= [b, x, y]
        if @press
          @press.call(b, x, y)
        end	        
      end
    
      def mouseReleased(mouse_event) 	
        b, x, y = translate(mouse_event)
        @last_down= nil
        if @release
          @release.call(b, x, y)
        end  
      end
    
      def mouseMoved(mouse_event)      
        b, x, y = translate(mouse_event)
        if @motion
          @motion.call(b, x, y)
        end        
      end
    
      def mouseClicked(mouse_event)
        b, x, y = translate(mouse_event)
        if @click
          @click.call(b, x, y)
        end        
      end  
    
      def mouseEntered(mouse_event)
        b, x, y = translate(mouse_event)
        if @entered
          @entered.call(b, x, y)
        end        
      end

      def mouseExited(mouse_event) 
        b, x, y = translate(mouse_event)
        if @exited
          @exited.call(b, x, y)
        end        
      end  

      # Install a handler block that will be called with button, x, and y 
      # when the mouse is clicked on the component.
      def click(&block)
        @click = block
      end
      
      # Install a handler block that will be called with button, x, and y 
      # when the mouse is pressed over the component.      
      def press(&block)
        @press = block
      end
      
      # Install a handler block that will be called with button, x, and y 
      # when the mouse is released over the component.      
      def release(&block)
        @release = block
      end
      
      # Install a handler block that will be called with button, x, and y 
      # when the mouse is moving over the component.      
      def motion(&block)
        @motion = block
      end
      
      # Install a handler block that will be called with button, x, and y 
      # when the mouse is being dragged (moved with button held down) over 
      # the component.      
      def drag(&block)
        @drag = block
      end
      
    end
    
      
  end
end
