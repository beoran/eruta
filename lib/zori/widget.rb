require 'forwardable'


module Zori
  class Rect
    attr_accessor :x
    attr_accessor :y
    attr_accessor :h
    attr_accessor :w
    def initialize(x, y, h, w)
      @x, @y, @h, @w = x , y, h, w
    end 
  end

  # A widget is any potentially visible object in the user iterface
  class Widget
    extend  Forwardable # We forward size and position requests to bounds rect    
    # include Stacklayout # stack layout
    include Sizemixin       
    # Size mixin, separated out because this file is becoming too big. 
    include Flowlayout  # Flow layout
     
     
    attr_reader   :style
    attr_reader   :bounds
    attr_accessor :z
    attr_accessor :order
    attr_accessor :parent
    attr_reader   :hanao
    attr_accessor :target
    
    Z_BACK        = 0
    Z_NORMAL      = 1
    Z_FRONT       = 2
    
  
    def initialize(target = nil, &action)
      @autolayout   = false
      @action       = action
      @layer        = 10
      @hanao        = Zori::Hanao.current
      @style        = @hanao.style.deep_copy
                      # Deep copy of the system style
      @colors       = @style.colors # Alias to the colors 
      @can_focus    = true          # Can be focused
      @focus        = false         # Currently has focus
      @closed       = false         # Widget was closed, ie, can be disposed of?
      @hidden       = false         # Widget is hidden?
      @active       = true          # Widget is active, ie, interacts with the system 
      @children     = []            # Child widgets in logical (insertion) order
      @drawchildren = []            # Children, in drawing order 
      @eventchildren= []            # Children, in event interaction order  
      @bounds       = Zori::Rect.new(0, 0, @hanao.w, @hanao.h)
      @z            = Z_NORMAL      # Useful for z-ordering 
      @order        = 0             # Logical order of this widget in it's parent
      @parent       = nil
      @can_drag     = false
      @debug        = Zori::DEBUG
      @iobalance    = 0     # Hover debugging
      @ignore       = []    # Events this widget is not interested in
      @mouse_down   = false # Is the mouse down on this widget or not?
      @mouse_over   = false # Is the mouse over this widget or not?
      @target       = @hanao.screen  # currently active drawing target
    end
    
    # Returns true if the widget can be dragged, false if not    
    def can_drag?
      return @can_drag
    end
    
    # Returns true if his widget is not interested in the given event name
    def ignore?(event)
      return true unless self.respond_to?(event)
      return @ignore.member?(event)
    end
    
    # Widgets have x, y, h, w. All are absolute on-screen coordinates. 
    def_delegators :@bounds, :x, :y, :w, :h, :w=, :h=
    def_delegator  :@bounds, :w, :wide
    def_delegator  :@bounds, :h, :high
    def_delegator  :@bounds, :w, :width
    def_delegator  :@bounds, :h, :height
    
    # :x=, :y= 
    
    # Sets new x position of widget. Automatically moves children as well.
    # Also calles update on self an every child
    def x=(new_x)
      delta_x       = new_x - self.x  # determine relative motion
      @bounds.x     = new_x           # set real position
      for child in self.children do
        # Move all children over the same delta as self.
        child.x     = child.x + delta_x # move children      
      end
      self.update_all                   # update self and children
    end
    
    # Sets new y position of widget. Automatically moves children as well.
    def y=(new_y)
      delta_y       = new_y - self.y  # determine relative motion
      @bounds.y     = new_y # set real position      
      for child in self.children do
        # Move all children over the same delta as self.
        child.y     = child.y + delta_y 
      end
      self.update_all                 # update self and children
    end
   

    # Returns this widget's x, y, w, and h in that order in an array
    def dimensions()
      return [self.x, self.y, self.w, self.h]
    end
    
    # Returns the colors that this widget's style uses
    def colors()
      return @style.colors
    end
    
    # Returns the padding for this widget
    def padding()
      return @style.padding
    end
    
    # Returns the marrgin for this widget
    def margin
      return @style.margin
    end
    
    # Returns the font that this widget's style is using.
    def font()
      return @style.font 
    end
    
    # Performs the given action with the right arguments.
    def perform_action(act)
      if act
        if act.arity == 0
          act.call()
        else
          act.call(self)
        end
      end
    end
    
    # Performs our own defined action 
    def act()
      perform_action(@action)
    end
    
    ##
    ## Focus control
    ##
    
    # Returns true if the widget can accept focus, false if not 
    # By default checks if the wiget is active or not
    def can_focus?
      return @can_focus && self.active? 
    end

    # Sets the focus to this widget.
    def focus! 
      @hanao.focus(self)       
    end
        
    # Releases the focus on this widget
    def unfocus!
      @hanao.unfocus(self)
    end
    
    # Checks if this widgets currently has and wants focus or not.
    # If focused, it responds to joystick and keyboard events, if not, 
    # it ignores these events.
    def focus?
      return @hanao.focus?(self) 
      # @focus && self.active?
    end
    
    # Focuses the next sibling of self . Only possible is self.parent is set
    def focus_next!()
      return false unless self.parent
      self.parent.unfocus! # No focus on any of the parent's widgets.
      widget = self.parent.next_sibling(self)
      widget.focus!    
    end
    
    # Focuses the previous sibling of self . Only possible is self.parent is set
    def focus_previous!()
      return false unless self.parent
      self.parent.unfocus! # No focus on any of the parent's widgets.
      widget = self.parent.previous_sibling(self)
      widget.focus!    
    end
    
    ##
    ## Containment
    ##
    
    # Checks if the given x and y coordinates are inside this widget
    def inside?(x , y)
      return  (x >= self.x && x <= (self.x + self.w) && 
              y >= self.y && y <= (self.y + self.h))
    end
    
    # Checks if the given widget overlaps wholy or partially with the 
    # other widget
    def overlap?(widget)
      self.inside?(widget.x, widget.y) || widget.inside?(self.x, self.y) 
    end
    
    # Returns true if this widget is topmost, false if it isn't
    def topmost?
      overlap = @hanao.overlapping(self)
      return true if overlap.empty?            
      for widget in overlap do
        return false if widget.z > self.z
      end
      return true
    end

    
    # Checks if the given widget is completely contained in self
    # That means that if drawn widget will be drawn inside self without
    # crossing it's borders. 
    def contains?(widget)
      return false unless point_inside?(widget.x, widget.y)
      return false unless point_inside?(widget.x + widget.w , widget.y + widget.h)
      # for the widget tot overlap, both it's top left and bottom right corners 
      # must be inside this widget. 
      return true
    end
    
    # Return self or child under these coordiates, or nil if none is under them.
    # If the parameter active_only is true, 
    # will return true only if we're active only to hide ourself in case we're 
    # inactive.   
    def under?(x, y, active_only = true)
      for widget in self.event_children do
        result = widget.under?(x, y, active_only)
        return result if result
        # Iterate depth first, and find the fitting widget. 
      end
      # If we get here, x and y are not inside the children 
      return nil unless self.inside?(x, y)
      # If not inside ourself, return nil.
      # If we get here, it's in self.
      if active_only
        return nil unless self.active? # return nil if we're not active
      end
      return self
    end
    
    # Return all of self and all children under these coordiates, or and empty
    # array  if none is under them. Returns only active children.  
    def all_under?(x, y)
      result = []
      for widget in self.event_children do
        aid     = widget.all_under?(x, y)    
        result += aid
        # Iterate depth first, and find the widgets that are also under this one. 
      end
      # Only read self to results if we're active and the mouse really
      # is inside self. This automatically "bubbles up" to the parent,
      # so this test is only needed here and not whilst iterating.
      if self.active? && self.inside?(x, y) 
        result << self
      end
      # The result contains the first child first and self last, 
      # so it /should/ be self-sorting
      return result
    end
    
    ## 
    ## Child Widgets    
    ##
    
    # Sorts children's draw and event order again if t, for example, 
    # their Z value has changed
    def sort_children()
      @drawchildren   = @children.sort
      # The ones with highest Z are drawn last
      @eventchildren = @drawchildren.reverse
      # The ones with highest Z are sent events first
    end
 
     
    # Adds a child to this widget
    def add_child(child)
      @children     << child
      child.parent   = self
      child.z        = Z_NORMAL  
      child.order    = @children.size - 1
      # Logical order is current position in array.
      # Sort children in drawing order and event order on insertion, 
      # so we don't have to sort each time we draw
      self.sort_children()
      if @autolayout 
      # Does't work very well at the moment. I think the problem is that 
      # the insertion order influences layout, which isn't good.
        child.fit_wide()
        self.layout_all 
        self.fit_child_high_simple
        self.fit_child_wide_simple
        # finally layout the children if possible
      end    
      return child
    end
    
    
    # Adds a child to this widget
    alias :<< :add_child
    
    # Puts the child widget +widget+ in front of of all other child widgets
    def child_to_front(widget)       
      return false unless @children.member? widget
      # Lower all other children first.
      for child in self.children
        child.z = Z_NORMAL if child.z > Z_NORMAL 
      end

      widget.z  = Z_FRONT # On top of self
      # change z layer
      self.sort_children      
      return true 
    end  
    
    # Puts the child widget +widget+ behind all other child widgets
    def child_to_back(widget)
      return false 
      return false unless @children.member? widget
      # Raise all other children first.
      for child in self.children
        child.z = Z_NORMAL if child.z < Z_NORMAL 
      end
      widget.z  = Z_BACK  # Below self
      # Change z layer
      self.sort_children
      return true 
    end  
    
      
    # Puts self in front of of all other widgets
    # Recursively calls parent->to_front +recurse+ times 
    # if +recurse+ is greater than 0 
    def to_front(recurse = 1)
      return false unless self.parent
      res  = self.parent.child_to_front(self)
      return self.parent.to_front(recurse - 1) if recurse > 0
      return res
    end
    
    # Puts self behind all other widgets
    # Recursively calls parent->to_back +recurse+ times 
    # if +recurse+ is greater than 0  
    def to_back(recurse = 0)
      return false unless self.parent
      res  = self.parent.child_to_back(self)
      return self.parent.to_back(recurse -1)  if recurse > 0
      return res
    end
      
    # Puts self at same level as all sibling widgets by resetting 
    # self.z to Z_NORMAL  
    def to_between()
      self.z = Z_NORMAL
    end
    
    # Recursively removes all closed child widgets from self
    def cleanup_closed
      @children.reject { |c| c.closed? } 
      @drawchildren  = @children.sort
      @children.each   { |c| c.cleanup_closed } 
    end
    
    # Calls Klass.new with args and adds this new child to this widget 
    def new_child(klass, *args)
      return add_child(klass.new(*args))
    end  
    
    # Returns the children of this widget
    def children
      return @children
    end
    
    # Returns the next sibling of this widget, or the first widget in the children if not found 
    def next_sibling(widget)
      sibling = nil
      windex  = self.children.index(widget) # Find the widget's index
      return self.children.first unless windex
      sibling = self.children[windex + 1]   # Next is one index further
      return self.children.first unless sibling
      return sibling
    end
    
    # Returns the previous sibling of this widget, or the first widget in the children if not found 
    def previous_sibling(widget)
      sibling = nil
      windex  = self.children.index(widget) # Find the widget's index
      return self.children.first unless windex
      sibling = self.children[windex - 1]   # Previous is one index back 
      return self.children.first unless sibling
      return sibling
    end
    
    # Returns whether or not self has widget as a child recusively
    def has_child?(widget)
      return false unless widget
      for child in self.children
        return true if child == widget
        return true if child.has_child?(widget)
      end
      return false
    end
    
    # Sortability 
    def <=>(other)
      zeq = (self.z <=> other.z)
      return zeq if zeq != 0  
      return self.order <=> other.order
    end
    
    # Childeren in drawing order
    def ordered_children
      return @drawchildren
    end
    
    # Children in event handling order
    def event_children
      return @eventchildren
    end
    
    # Calls the block, first with self, and then with all children traversed
    # depth-first. Stops if any of the blocks returns :stop
    def self_and_each_child(&block)
      for child in self.event_children
        result = child.self_and_each_child(&block)
        return result if result == :stop
      end
      result = block.call(self)
      return result
    end
    
    ##
    ## Visibility and Activity
    ##    
    # Returns whether or not the widget was closed 
    def closed?
      return @closed
    end
    
    alias :close? :closed?
    
    #
    ## Low level Visibility and activity setters
    #
    # Enables this widget only, and does nothing else
    def enable!
      @active = true 
    end
    
    # Disables this widget only, and does nothing else.
    def disable!
      @active = false
    end
    
    # Hides this widget only, and does nothing else.
    def hide!
      @hidden = true
    end
    
    # Unhides this widget only, and does nothing else.
    def unhide!
      @hidden = false
    end
    
    # Closes the widget, and notifies the parent to clean up.
    # This hides the widget first.
    def close
      self.hide
      @closed = true      
      if self.parent
        self.parent.cleanup_closed 
      end
    end
    
    # Returns whether or not the widget is available for interaction. 
    # By default, a hidden widget will also be not active 
    def active?
      return @active && !(self.hidden?)
    end
    
    
    # Activates the widget and all it's children without showing it. 
    # This may cause strange results 
    # for certain widgets 
    def enable()
      self.enable!
      self.children.each { |child| child.enable }
    end
    
    # Sets the widget to non active, though it may stay visible
    # Automatically releases the focus on self and children
    def disable()
      self.disable!
      self.children.each { |child| child.disable }      
      self.unfocus!
    end
    
    # Returns if the widget is hidden or otherwise invisible 
    def hidden?
      return @hidden || self.closed?
    end
    
    # Hides the widget. This also deactivates the widget by 
    # calling disable on it. If +recursive+ is true as it is by default,
    # all children are also hidden.
    def hide(recursive = true)
      self.hide!
      self.disable!
      self.children.each { |child| child.hide }
    end
    
    # Unhides the widget, but does not activate it.
    # If +recursive+ is true as it is by default,
    # all children are also unhidden.    
    def unhide(recursive = true)
      if recursive
        self.children.each { |child| child.unhide }
      end  
      self.unhide!
    end
    
    # Unhides the widget and it's children, and activates it
    def show()
      self.unhide()
      self.enable()
    end
    
    # Shows this widget if hidden and hides it when shown
    def toggle_hidden
      hidden? ? show : hide
    end
    
    
    ### Drawing         ###
    
    # You may also want to override this, in case your widget needs a background.
    # Look for self.style.draw_background_image(target, self) 
    def draw_background(target)
    end
    
    # You may also want to override this, in case your widget needs a border.
    # Normally called by draw_after_children
    # Try self.put_border(target)
    def draw_border(target)
    end
    
    # Override this, as it draws the widget itself.
    # This will be called before draw_children, so the children normally 
    # draw over the parent widget 
    def draw(target)
    end
    
    # You may override this if you need to draw something after your child 
    # widgets are done drawing
    def draw_after_children(target = nil)
      draw_border(target)
    end
    
    
    # Draws the children. Ususally you don't need to override this.
    def draw_children(target = nil)
      for child in self.ordered_children do
        child.draw_all(target)
      end
    end
    
    def total_z
      result  = self.parent ? self.parent.total_z : 0
      result += self.z
      return result  
    end
    
    # Calls draw and then draw_children. 
    # Usually, you don't need to override this.
    # It also takes care of not drawing the widget if it's hidden or closed.
    def draw_all(target=nil)
      return if self.hidden?
      target ||= @self.target
      draw_background(target)
      draw(target)
      draw_children(target)
      draw_after_children(target)
      # Debugging message
      # style.colors.text
      # @debugcolor ||= Sisa::Color.random_dark
      # style.font.draw(target, self.x, self.y, "#{self.z}", @debugcolor, style.colors.background)
    end
    
    # Drawing helper functions
    
    # Draws a text with the Widget's default font and colors at the given 
    # position on the given bitmap.
    def put_text(target, x, y, text)
      target.text(x, y, text, self.font, 
                  self.colors.text, self.colors.background)  
    end
    
    
    # Draws a normal background around ourself 
    def put_background(target)
      target.slab.put_border(*self.dimensions, self.colors.background)
    end
    
    # Draws a normal border around ourself 
    def put_border(target)
      @hanao.put_border(target, self.colors, *self.dimensions)
    end
    
    # Draws an inset border around ourself
    def put_inset(target)
      @hanao.put_inset(target, self.colors, *self.dimensions)
    end
    
    # Draws an outset border around ourself
    def put_outset(target)
      @hanao.put_outset(target, self.colors, *self.dimensions)
    end
    
    # Debugging helper
    def puts(*args)
      Kernel.puts(*args) if @debug
    end
    
    # Called after layout or value change. Updates self and all children
    def update_all
      update
      for child in children do
        child.update()
      end
    end
    
    # Override this, not update_all()
    def update
    end
    
    # Lays out this object's children. Does nothing by default.
    # Is implemented by the selected layout mixin
    # def layout_children
    # end
    
    # Layout
    def layout_all
      for child in self.children  do
        child.layout_children
      end      
      self.layout_children
    end
    
    # Simple height fitter that simply looks at the last (presumably bottom-most)
    # widget. Only works after the children are properly layout-ed
    def fit_child_high_simple()
      newh    =  self.children.last.out_bottom + self.padding
      self.h  = newh
      return self
    end
    
    # Simple width fitter. Only works after the children are properly layout-ed
    # and if the widget is left-aligned
    def fit_child_wide_simple()
      maxw    = 0
      self.children.each { |child| maxw = child.out_right if  child.out_right > maxw } 
      neww    = maxw + self.padding
      self.w  = neww
      return self
    end


    
    # Shrinks or grows self so it snugly fits inside the given widget's width. 
    # If widget is nil, fits itself to widget.parent. If that is also 
    # not set, does nothing. This method doesn't move the widget around.
    # Always returns self for method chaining.
    def fit_wide(widget=nil) 
      widget ||= self.parent     
      return self unless widget
      self.w   = widget.w - ( widget.padding * 2) - (self.margin * 2)
      return self
    end
    
    # Shrinks or grows self so it snugly fits inside the given widget's height. 
    # If widget is nil, fits itself to widget.parent. If that is also 
    # not set, does nothing. This method doesn't move the widget around.
    # Always returns self for method chaining.
    def fit_high(widget=nil) 
      widget ||= self.parent     
      return self unless widget
      self.h   = widget.h - ( widget.padding * 2) - (self.margin * 2)
      return self
    end
    
    def on_active()
      puts "Active"
    end
    
    # Called if a key is pressed when this widget has the focus
    def on_key_down(code, mods, text)
      return false unless self.focus?
      self.on_key(code, mods, text)
    end
    
    # Called if a key is released when this widget has the focus
    def on_key_up(code, mods, text)
      return false unless self.focus?
    end
    
    # Called if a complete key press is and releae is received. 
    # The sequence of events emitted on a key press shall be:
    # on_key_down, on_key (on_key, repeated many times), on_key_up
    def on_key(sym, mod, text = nil)
      # puts "Key: #{sym}, #{mod}, #{text}"
    end
    
    # Called if the mouse is pressed somewhere
    def on_mouse_down(x, y, button) 
      @mouse_down = true     
    end
    
    # Called if the mouse is released somewhere 
    def on_mouse_up(x, y, button)
      @mouse_down = false      
    end
    
    # Called when a mouse click is recieved. 
    # The sequence of events emitted on a mouse click shall be:
    # on_mouse_down, on_mouse_up, on_click 
    def on_click(x, y, button)      
      puts "Click: #{x}, #{y}, #{button} #{self.inspect}"
      self.focus! if @can_focus
    end
    

    # Called when the mouse moves over this widget
    def on_mouse_move(x, y)
    end
    
    # Called when the mouse moves into this widget 
    def on_mouse_in(x, y, from = nil)
    end
    
    # Called when the mouse moves out of this widget
    def on_mouse_out(x, y, from = nil)
      @mouse_down = false 
      # We're not pressed if the mouse leaves us.
    end
    
    
    # Called when the scroll wheel of the mouse is rolled over this widget 
    def on_scroll(direction)
      # puts "Scroll: #{direction}"
    end
    
    # Called after this widget has recieved the focus
    def on_focus(from_widget = nil)
      # puts "#{self}: gained focus from #{from_widget}"
    end
    
    # Called after this widget has lost focus. Note that you cannnot normally 
    # stop focus from being lost.
    def on_focus_lost(to_widget = nil)
      # puts "#{self}: lost focus to #{to_widget}"
    end 
    
    # Default on_mouse_drag that already works.
    def on_mouse_drag(from_x, from_y, to_x, to_y)
      return unless self.can_drag? # Only drag if allowed.
      dx     = to_x - from_x
      dy     = to_y - from_y
      self.x += dx 
      self.y += dy
    end
    
    # Inspect
    def inspect
      return "<#{self.class.name}: #{self.object_id} #@iobalance>"
    end
    
  end # class Widget
end # class Zori