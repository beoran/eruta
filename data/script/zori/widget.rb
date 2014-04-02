module Zori
  # A widget may have sub-widgets
  module Widget
    include Graphic
    
    # Position of the widget.
    attr_reader :x
    attr_reader :y
    attr_reader :z
    # Dimensions of the widget.
    attr_reader :w
    attr_reader :h 
    
    # Style of the widget
    attr_reader :style
    
    # Capabilities of the widget
    attr_reader :can
    
    # State of the widget
    attr_reader :state    
    
    def initialize(params={}, &block)
      super(params, &block)
      @action            = block
      @components        = []
      @x                 = params[:x] || 0
      @y                 = params[:y] || 0
      @w                 = params[:w] || 640
      @h                 = params[:h] || 480
      self.init_graph
      @z                 = params[:z] || 0
      @style             = Zori::Style.default.dup
      # XXX: should be copy deep of the default style
      @can               = Zori::Capability.new
      @state             = Zori::State.new
      @state.set(:active)
      # Widget is active
      # @action       = action
    end
  
    # Called when an event comes in from the Eruta engine.
    # Tries every component of this widget in order and stops if a truthy 
    # value is returned. Finally calls handle_event on self in case 
    # no component handles the event. 
    def on_event(*data)
      if @components 
        @components.each do | component |
          res = component.on_event(*data)
          return res if res
        end
      end
      res = handle_event(*data)
    end
    
    # The widget's own event handler will try to call a method named on_eventname
    def handle_event(*args)
      type   = args.shift 
      return false unless type
      method = "on_#{type}".to_sym
      # Try direct handler call.
      if self.respond_to?(method)
        return self.send(method, *args)
      else 
        return nil
      end
    end

    # Adds a component to this widget
    def <<(component)
      @components << component
    end
    
    # Checks if the coordinates are inside the main rectange of the widget.
    def inside?(x, y)
      return (x >= @x) && (x <= (@x + w)) && (y >= @y) && (y <= (@y + h))
    end
    
    # Hides the widget and all it's children
    def hide
      @components.each do |comp|
        comp.hide
      end
      hide_graph
    end
    
    # Shows the widget and all it's children
    def show
      @components.each do |comp|
        comp.show
      end
      show_graph
    end
    
    
  end
end