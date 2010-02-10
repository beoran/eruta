# encoding: utf-8
#
# 
# A ring shaped user interface
# XXX: it's still quite broken, because it was ported from an older GUI system, 
# and it doesn't quite fit the new system yet. 

module Zori

  # A widget to model a ring-style menu
  class Ring < Widget 
  # ???  Should perhaps not inherit from Widget, because of it's wide layout
  # TODO Animate the ring menu so it opens and closes with a "swoop" and
  # rotates steplessly
  
    # One item on a ring menu
    class Item  < Zori::Widget
      attr_accessor :name     # Short name
      attr_accessor :text     # Long description
      attr_accessor :icon     # Icon
      attr_accessor :subring  # Subring to be opened on activating this item, of any
      attr_accessor :x        # X position to draw the icon
      attr_accessor :y        # Y position to draw the icon
      attr_accessor :display  # Our display
      
      def initialize(name, text, icon = nil, subr = nil, &block)
        super(&block)
        @x            = nil
        @y            = nil
        @name         = name
        @icon         = icon
        @text         = text
        self.subring  = subr
        @can_focus    = false 
      end
      
      def subring=(subr)
        if subr
          self     << subr 
          @subring  = subr
          @subring.hide
          @subring.disable
        end  
      end
      
      def draw(display)
        if @icon
          display.put_bitmap(@x ,@y, @icon)    
        else
          display.put_rectangle(@x, @y, 40, 40, [255,0,0])
        end
      end
    end  
    
    # Center position of the ring menu
    attr_accessor :cx
    attr_accessor :cy
    attr_accessor :radius         # Radius of the ring menu
    attr_accessor :target_icon    # Icon used for targeting
    attr_reader   :items          # Items in this menu 
    attr_reader   :active_item    # Current active item
    attr_reader   :action         # Action block to perform on selection
    attr_accessor :cursor_bitmap  # Cursor bitmap to draw.
    attr_accessor :parentring     # Parent ring menu to return to, if any
    
    # Sets up the ring menu. 
    # The coordinates +cx+ and +cy+ are on-screen coordinates for the center 
    # of the menu. The +block+ is called when an item in menu is selected.    
    def initialize(cx = nil, cy = nil, &block)
      super(&block)
      cx          ||= @hanao.w / 2
      cy          ||= @hanao.h / 2
      @radius       = 50 # Small radius for ring with less items
      @items        = []
      @active_item  = nil
      @cx           = cx
      @cy           = cy
      @before       = self
      @after        = self
      @target_item  = nil
      @action       = block
      @cursor       = 0 
      @motion       = :none 
      @status       = Zori::Dialog::Status.new("")
      self         << @status
      @cursor_bitmap= Zori::Hanao.load_image('ui', 'cursor', 'square.png')      
      @subring      = nil # Currently active sub-ring, if any      
      @parentring   = nil # Currently active parent ring, if any
    end
    
    # Adds a new item to this ring menu
    # It has a +name+, a +text+ to display, and and optional 
    # +icon+ and a +subring+ attached to it
    # Returns the item added
    def item(name, text, icon = nil, subring = nil)
      item          = Item.new(name,text,icon,subring)
      item.x        = @cx
      item.y        = @cy
      @items       << item
      @active_item  = item
      @cursor       = @items.size - 1
      self         << item
      update()
      return item
    end
    
    # Draws this menu
    def update()
      return if @items.size < 1
      angle     =  Math::PI  
      # -2 * (Math::PI / 6)
      # - (Math::PI / 2)
      aring     = (2 * Math::PI) / @items.size
      size      = @items.size
      for index in (0..size)
        alpha   = angle + (aring * (-index + @cursor))
        item    = @items[index]
        next unless item
        item.x  = @cx + (@radius * Math.sin(alpha))
        item.y  = @cy + (@radius * Math.cos(alpha))
      end
      @active_item = @items[@cursor]
      @status.text = @active_item.text if @status && @active_item
      @radius = ((size * 40) / 10) + 50
      @radius = 400 if @radius < 40
      #adjust radius to amount of items in the menu, clipping to 400
    end
    
    # Draws the children
    def draw_children(target)
      for child in self.ordered_children do
        child.draw_all(target)
      end
    end
    
    # Draws the cursor
    def draw_cursor(display)    
      if @cursor_bitmap
        xx = @cx  
        yy = @cy - @radius
        display.put_bitmap(xx, yy, @cursor_bitmap)
      else 
        display.put_line(@cx, @cy - radius, 40, - radius, [255,255,0] )
        display.put_line(@cx, @cy - radius + 40 , 40, - radius + 40, [255,255,0] )
      end  
    end
    
    # Draws this ring menu to the display
    def draw(display)
      @status.draw(display) if @status
      return if @subring
      draw_cursor(display)
    end
    
    def draw_after_children(target)
      super(target)
      draw_cursor(target)
    end
    
    protected
    # Helper function to make the menu roll right
    def roll_right
      @cursor      += 1
      @motion       = :right
      @cursor       = 0 if @cursor >= @items.size
    end
    
    # Helper function to make the menu roll left
    def roll_left
      @cursor      -= 1
      @motion       = :left
      @cursor       = @items.size - 1 if @cursor < 0
    end    
    
    public 
    
    # Shows self and the items, but hides the subrings
    def show_ring
      self.show
      for item in @items
        next unless item.subring
        item.subring.hide  
      end
    end
   
    # Call on @parentring when it returns from a subring.
    def return_from_subring
      # @subring.hide
      @subring  = nil
      # self.show
      self.show_ring()
      self.focus! # Get the keyboard/joystick focus
    end
    
    # Enters the subring from the given parent ring
    def enter_subring(parent)
      @parentring = parent
      self.show_ring
      self.focus! # Get the keyboard and joystick focus.
    end
    
    # Recursively closes this ring and all parent and side rings
    def close_all
      self.close
      @parentring.close_all if @parentring and !@parentring.close?
      # Prevent eternal loops by not closing if already closed
    end
    
    # Recursively hidesthis ring and all parent and side rings
    def hide_all
      self.hide
      @parentring.hide_all if @parentring and !@parentring.hidden?
      # Prevent eternal loops by not closing if already closed
    end
    
    
    # Handle key presses
    def on_key(key, modifier, string)
      case key
        when Sisa::Key::RIGHT
          roll_right
        when Sisa::Key::LEFT
          roll_left
        when Sisa::Key::RETURN, Sisa::Key::KP_ENTER
          if @active_item && @active_item.subring
            @subring            = @active_item.subring
            @subring.enter_subring(self)
            # Enter the submenu if any
            # No need to hide ourselves since we forward drawing to subring also
          else
            @action.call(self, @active_item) if @action
            self.hide_all()            
            # Don't forget to hide ourself and the parent ring(s)
          end
          # Or call the given action block if enter is pressed.
          # In this case, we canclose the ring menu
        when Sisa::Key::ESCAPE, Sisa::Key::KP_PLUS
          # self.hide
          # Hide this menu, but show the parent again if any
          p self
          if @parentring
            @parentring.return_from_subring
          else
            self.hide
          end
        else
          # Do nothing
      end 
      update
    end
    
    
    
  end # Ring

end # Zori




