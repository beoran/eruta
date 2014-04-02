

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
    include Zori::Widget
    
    
    attr_reader :name

    # Creates a new UI page with the given name.
    def initialize(params={}, &block)
      super(params, &block)
      @name     = params[:name].to_sym
    end
    
    # Called when this page is activated.
    def on_enter(data = {})
      @state.set(:active)
      self.show
      on_event(:enter)
    end
    
    # Called when this page is deactivated.
    def on_leave(name=nil)
      @state.unset(:active)
      self.hide
      on_event(:leave, name)
    end
    
    # Override this in pages to do special case handling
    def handle_event(*data)
      return false
    end
 
    # Adds a button to the page 
    def make_button(x, y, w, h, heading, &block)
      button = Zori::Button.new(:x => x, :y => y, :w => w, :h => h, :heading => heading, &block)
      self << button
      return button
    end
    
    
    
  end
end

