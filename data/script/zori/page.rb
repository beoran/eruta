

# The zori UI consists of several "pages" or screens of UI.
# Only one page can be active at the same time, and the size fo a page is 
# that of the screen.
# The page named :default is normally active and is supposed to be used 
# for the in-engine UI and HUD elements, dialogs, etc.
# Other pages are for screens such as the start screen, save screen,
# main menu, inventory, etc.
# 
module Zori
  class Page
    attr_reader :name
    attr_reader :widgets
    attr_reader :state
    attr_reader :can

    # Creates a new UI page with the given name.
    def initialize(name)
      @can      = Capability.new
      @state    = State.new
      @name     = name.to_sym
      @widgets  = []
      self.class.register(page)
    end
    
    # Unregisters a page
    def self.register(page)
      @pages ||= {}
      @pages[page.id] = page
    end
    
    # Registers a page
    def self.unregister(page)
      @pages ||= {}
      @pages[page.id] = nil
    end
    
    # Returns the page for the given name. 
    def self.for_name(name) 
      @pages ||= {}
      return @pages[name.to_sym]
    end
    
    # Returns the currently active page or nil for none.
    def self.active
      @active
    end
    
    
    # Returns the currently active page id or nil for none.
    def self.active_id
      return nil unless @active
      return @active.id
    end
    
    # Activates the named page, and passes the data if any.
    # Returns true on success or false if no such page is registered.
    def self.go(name, data = {})
      page = self.for_name(name)
      return false unless page
      @active.on_leave(name) if @active
      @active = page
      @active.on_enter(data)
      return true
    end

    # Called when this page is activated.
    def on_enter(data = {})
      @state.set(:active)
      on_event(:enter)
    end
    
    # Called when this page is deactivated.
    def on_leave(name=nil)
      @state.unset(:active)
      on_event(:leave, name)
    end
    
    # Override this in pages to do special case handling
    def on_page_event(*data)
      return false
    end
    
    # Called when an event comes in from the Eruta engine.
    # Tries every widget in order and stops if a truthy value is returned.
    # Finally calls on_page_event. If that returns non-truthy, the return nil 
    # so the event can be handled by the normal non-UI controls.
    def on_event(*data)
      @widgets.each do | widget |
        res = widget.on_event(*data)
        return res if res
      end
      res = on_page_event(fata)
    end

    # Called when an event comes in from the Eruta engine.
    # Sends event to active page.
    def self.on_event(*data)
      @active.on_event(*data) if @active
    end

    # Adds a widget to this page.
    def <<(widget)
      @widgets << widget
    end
    
    
    
  end
end

