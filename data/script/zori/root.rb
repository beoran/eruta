module Zori
  # The root widget is the single toplevel widget that holds all 
  # other sub-widgets. 
  class Root
    include Widget
    
    # Non-mouse cursor graph
    attr_reader :cursor_graph
    # Non-mouse cursor image
    attr_reader :cursor_image
    # Mouse cursor graph
    attr_reader :mouse_graph
    # Mouse cursor image
    attr_reader :mouse_image
    
    # Non-mouse cursor currently "focusing" this widget. 
    attr_reader :cursor_widget
  
    # Default font for the UI.
    attr_reader :font
    
    # Loads the common data for the Root widget.
    def load_common_data
      # @font           = Store.load_ttf_font('/font/Tuffy.ttf', 16) 
      @font           = Store.load_ttf_font('/font/unifont.ttf', 16) 
      
      
      @mouse_image    = Store.load_bitmap('/image/gin/fountain-pen_32.png')
      @mouse_image.average_to_alpha(255,255,255)
      @mouse_graph    = Graph.make_image(200, 200, @mouse_image.id)
      @mouse_graph.z  = 9999
      @mouse_graph.image_flags = Eruta::FLIP_HORIZONTAL | Eruta::FLIP_VERTICAL
      
      @cursor_image   = Store.load_bitmap('/image/gin/curled-leaf_32.png')
      @cursor_image.average_to_alpha(255,255,255)    
      @cursor_graph           = Graph.make_image(100,100, @cursor_image.id)
      @cursor_graph.z         = 10000
      # @cursor_graph.speed     = [ rand(10) - 5, rand(10) - 5]
      
      Eruta.show_mouse_cursor = false
    end
    
    def initialize(params={}, &block)
      load_common_data
      @pages         = {}
      @active        = nil
      @cursor_widget = nil
    end
        
    # Unregisters a page
    def register(page)
      @pages ||= {}
      @pages[page.name] = page
    end
    
    # Registers a page
    def unregister(page)
      @pages ||= {}
      @pages[page.name] = nil
    end
    
    # Returns the page for the given name. 
    def for_name(name) 
      @pages ||= {}
      return @pages[name.to_sym]
    end
    
    # Returns the currently active page or nil for none.
    def active
      @active
    end    
    
    # Returns the currently active page id or nil for none.
    def active_id
      return nil unless @active
      return @active.name
    end
    
    # Activates the named page, and passes the data if any.
    # Returns true on success or false if no such page is registered.
    def go(name, data = {})      
      page = self.for_name(name)
      return false unless page
      @active.on_leave(name) if @active
      @active = page
      @active.on_enter(data)
      return true
    end
    
    # Sends the event to the active page
    def on_event(*args)
      # move the mouse cursor around in all cases
      if args[0] == :mouse_axes
        x = args[2]
        y = args[3]
        @mouse_graph.position = [x, y] if @mouse_graph
      end  
      return nil unless @active
      return @active.on_event(*args)
    end

  end
end