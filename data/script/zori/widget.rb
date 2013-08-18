
module Zori
  # A widget is any potentially visible object in the user iterface
  class Widget
    # include Stacklayout # stack layout
    include Sizemixin
    # Size mixin, separated out because this file is becoming too big. 
    include Layoutmixin
    # Layout mixin, separated out because this file is becoming too big. 
    include Flowlayout  # Flow layout
    
    
    attr_reader   :can
    attr_reader   :state
    attr_reader   :style
    attr_reader   :bounds
    attr_accessor :z
    attr_accessor :order
    attr_accessor :parent
    attr_accessor :target
    attr_reader   :graphs
    attr_reader   :children
        
    Z_BACK        = 1000
    Z_NORMAL      = 2000
    Z_FRONT       = 3000
    
  
    def initialize(target = nil, &action)
      @graphs       = []
      @action       = action
      @z            = Z_NORMAL
      @style        = Zori::Style.default.dup
      @can          = Zori::Capability.new
      @state        = Zori::State.new
      @state.set(:active)
      # Widget is active
      # XXX: should be copy deep of the default style
      @children     = []    # Child widgets in logical (insertion) order
      @graph        = []    # Scene graph elements to draw self.
      @bounds       = Zori::Rect.new(0, 0, 640, 480)
      @parent       = nil
    end
    
 
    # Widgets have x, y, h, w. All are absolute on-screen coordinates. 
    def x
      @bounds.x
    end

    def y
      @bounds.y
    end

    def w
      @bounds.w
    end

    def h
      @bounds.h
    end

    def w=(v)
      @bounds.w = v
    end

    def h=(v)
      @bounds.h = v
    end

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
    
    
    # Performs the given action with the right arguments.
    def perform_action(act, *args)
      if act
        if act.arity == 0
          act.call()
        else
          act.call(self, *args)
        end
      end
    end
    
    # Performs our own defined action 
    def act(*args)
      perform_action(@action, *args)
    end
    
    def active?
      self.state.is?(:active)
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
      return self.parent.to_back(recurse - 1) if recurse > 0
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
    
    # Inspect
    def inspect
      return "<#{self.class.name}: #{self.object_id} #@iobalance>"
    end
    
  end # class Widget
end # class Zori