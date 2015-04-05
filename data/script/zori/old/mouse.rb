module Zori
#This class models the mouse state and mouse constants
  class Mouse
    LEFT_BUTTON   = 1
    MIDDLE_BUTTON = 2
    RIGHT_BUTTON  = 3
    SCROLL_UP     = 4
    SCROLL_DOWN   = 5
    
    # Returns if a button is in reality a scroll or not. 
    # Needed due to an SDL artifact which translates scrolls to 
    # moude button presses and releases.
    # If it is a scroll, returns -1 for a scroll up, and +1 for a scroll down.
    # otherwise returns nil
    def self.is_scroll?(button)
      case button 
      when SCROLL_UP
	return -1
      when SCROLL_DOWN
	return +1
      else
	return nil
      end   
    end     
  
    attr_accessor :x, :y, :under, :clicked
    
    def initialize(xin = 0, yin = 0)
      @x       = xin
      @y       = yin
      @under   = []
      @clicked = nil
      @pressed = {}
      @dragged = {}
      @visible = true
      @cursor  = Hanao.load_image('cursor', 'mouse.png') rescue nil;
      # Hanao.screen.showcursor = false if @cursor
    end
    
    def move(xin, yin) 
      @x = xin
      @y = yin
    end
    
    def press(button, widget)
      # @pressed[button] ||= MouseButtonInfo.new()
      @pressed[button] = ClickInfo.new(button, widget, Time.now) 
    end
    
    def clicked?(widget)
      return @clicked == widget
    end
    
    def release(button)
      @pressed.delete(button);
      @clicked = nil
      @dragged = {}
    end
    
    def pressed?(button=nil)
      return (!@pressed.empty?) unless button
      return @pressed[button]
    end
    
    # Draws the mouse cursor to the screen
    def draw(screen)
      color = Gari::Color::White
      color = Gari::Color::Green if pressed?()
      color = Gari::Color::Red   if pressed?(2)
      if @cursor
	screen.blit(@x, @y, @cursor)
      else   
	screen.disk(@x, @y, 1, color)
      end  
#     Uncomment these lines below to mark clicked/hovered widgets for debugging
#         if @clicked
#            screen.disk(@clicked.x, @clicked.y, 10, Gari::Color::Red)
#         end
#         if @under && (!under.empty?)
#           for under in @under do
#             screen.disk(under.x, under.y, 7, Gari::Color::Green)
#           end 
#         end

      
    end    
  end  
end # mozule Zori