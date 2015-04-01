module Zori
  # The root widget is the single toplevel widget that holds all 
  # other sub-widgets. 
  class Root
    include Widget

    # There are two cursors, a mouse cursor and a keyboard/joystick one.
    
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
      @font           = Font.load(:font_gui, '/font/unifont.ttf', 16) 
      
      @mouse_image    = Bitmap.load(:mouse_cursor,
                                    '/image/gin/fountain-pen_32.png')
      @mouse_image.average_to_alpha(255,255,255)
      @mouse_graph    = Graph.make_image(200, 200, @mouse_image.id)
      @mouse_graph.z  = 9999
      @mouse_graph.image_flags = Eruta::FLIP_HORIZONTAL | Eruta::FLIP_VERTICAL
      
      @cursor_image   = Bitmap.load(:cursor, '/image/gin/curled-leaf_32.png')
      @cursor_image.average_to_alpha(255,255,255)    
      @cursor_graph   = Graph.make_image(100,100, @cursor_image.id)
      @cursor_graph.z = 10000
      @cursor_graph.image_flags = Eruta::FLIP_HORIZONTAL
      # @cursor_graph.speed     = [ rand(10) - 5, rand(10) - 5]
      
      Eruta.show_mouse_cursor = false
    end
    
    def initialize(params={}, &block)
      load_common_data
      @pages         = {}
      @active        = nil
      @cursor_widget = nil
    end

    # Creates a select mark graph for any widget
    def create_select_mark
      select_mark             = Graph.make_image(100, 100, @cursor_image.id)
      select_mark.z           = 9998
      select_mark.image_flags = Eruta::FLIP_HORIZONTAL
      return select_mark
    end
        
    # Unregisters a page
    def register(page)
      @pages ||= {}
      @pages[page.name.to_sym] = page
    end
    
    # Registers a page
    def unregister(page)
      @pages ||= {}
      @pages[page.name.to_sym] = nil
    end

    # Returns the page for the given name. 
    def for_name(name) 
      @pages ||= {}
      return @pages[name.to_sym]
    end

    # Looks up a registered page
    def [](name)
      return for_name(name)
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

    # Moves the keyboard cursor to a given position
    def move_cursor(x, y)
      @cursor_graph.position = x, y
    end

    # Sets the given widget as the one widget that is marked by the keyboard
    # cursor.
    def mark_widget(widget)
      return false unless widget.mark
      @cursor_widget.unmark if @cursor_widget
      @cursor_widget = widget
      move_cursor(widget.x + widget.w, widget.y)
      return true
    end

    # Sets the given widget as the one that is hovered by the mouse cursor.
    def hover_widget(widget)
      return false unless widget.hover
      @mouse_widget.unmark if @mouse_widget
      @mouse_widget = widget
      widget.mark
      move_cursor(widget.x + widget.w, widget.y)
      return true
    end


  end
end
