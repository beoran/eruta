# encoding: UTF-8
# Zori Eruta Edition:
#
# Zori is a GUI library for mruby and the Eruta engine 
# beoran@gmail.com, 2009, 2010
#
# Zori (japanese footwear) follows the Shoes philosophy in that it sees width 
# as largely static, and height as dynamic and scrollable because of the mouse
# wheel and pageup/pagedown keys
#
# There is only one top-level widget, however, every widget can contain 
# any amout of child widgets. Widgets that are children of the same parent 
# widget are said to be on the same level.
#
#
# The box model used in Zori is as follows:
#  ......................................
#  .margin                              .
#  .  +-------------border-----------+  .
#  .  | padding                      |  .
#  .  |  ..........................  |  .
#  .  |  .                        .  |  .
#  .  |  . contents               .  |  .
#  .  |  .                        .  |  .
#  .  |  ..........................  |  .
#  .  | padding                      |  .
#  .  +------------------------------+  .
#  .margin                              .
#  ......................................
#
# The padding determines the minimal distance between the border or the 
# parent widget and it's contents and/or child widgets.
#
# The border's thickness is only relevant for visual effects. It does not change  
# the layout. The border is effectively "inside" the padding of the widget.
#
# The margin of a widget determines how closely that widget may be packed 
# to it's sibling widgets.
#
# The work in Zori is divided between Hanao and Widget. The Hanao class
# handles everything that depends on and/or may influence several widgets at
# once, such as event dispatching but also setting the focus, determining which
# widget is being hovered, or dragged, etc. The latter fuctions change the state
# of several widgets, so they are handled on the level of the Hanao class.
# The Widget class and it's child classes handle the individual state and
# actions of the various widgets individually.
#


module Zori
  DEBUG   = true

  # We define structs for stuff that's too simple to need classes yet.
  # Structure for mouse button info
  MouseButtonInfo = Struct.new(:button, :pressed, :released)
  # Structure for Click Info
  ClickInfo       = Struct.new(:button, :widget, :when)
  # Structure for Drag Info
  DragInfo        = Struct.new(:button, :widget, :when)
  # Class that models the state of a single key
  KeyInfo         = Struct.new(:pressed, :sym, :mod, :text, :repeated)

  script 'zori/rect.rb'
  script 'zori/state.rb'
  script 'zori/capability.rb'
  script 'zori/handler.rb'
  script 'zori/graphic.rb'
  script 'zori/element.rb'
  script 'zori/control.rb'
  
  script 'zori/page.rb'
  
  script 'zori/hanao.rb'
  
  
  
  
  
  script 'zori/sizemixin.rb'
  script 'zori/layoutmixin.rb'
  script 'zori/flowlayout.rb'
  script 'zori/stacklayout.rb'
  script 'zori/fixedlayout.rb'
  script 'zori/widget.rb'

  
  script 'zori/button.rb'
  script 'zori/checkbox.rb'


  script 'zori/flow.rb'

  script 'zori/frame.rb'
  script 'zori/input.rb'
  script 'zori/label.rb'
  script 'zori/mainwidget.rb'
  script 'zori/menu.rb'
  script 'zori/menu.rb'
  script 'zori/menuitem.rb'
  script 'zori/progress.rb'
  script 'zori/radiobutton.rb'
  script 'zori/ring.rb'

  script 'zori/slider.rb'
  script 'zori/stack.rb'

  script 'zori/style.rb'
  script 'zori/menu.rb'
  script 'zori/text.rb'
  script 'zori/shortcut.rb'
  # script 'zori/mapeditor.rb'
  script 'zori/mouse.rb'
  script 'zori/keyboard.rb'
  script 'zori/joystick.rb'

  script 'zori/dialog.rb'
  script 'zori/console.rb'
  
  
  # Non-mouse cursor graph
  def self.cursor_graph
    @cursor_graph
  end
  
  # Non-mouse cursor image
  def self.cursor_image
    @cursor_image
  end

  # Mouse cursor graph
  def self.mouse_graph
    @mouse_graph
  end
  
  # Mouse cursor image
  def self.mouse_image
    @mouse_image
  end
  
  def self.font
    @font
  end

  
  def self.load_common_data
    @font           = Store.load_ttf_font('/font/Tuffy.ttf', 16)
    
    @mouse_image    = Store.load_bitmap('/image/gin/fountain-pen_32.png')
    @mouse_image.average_to_alpha(255,255,255)
    @mouse_graph    = Graph.make_image(200, 200, @mouse_image.id)
    @mouse_graph.z  = 9999
    @mouse_graph.image_flags  = Eruta::FLIP_HORIZONTAL | Eruta::FLIP_VERTICAL
    
    @cursor_image   = Store.load_bitmap('/image/gin/curled-leaf_32.png')
    @cursor_image.average_to_alpha(255,255,255)
    
    
    @cursor_graph   = Graph.make_image(100,100, @cursor_image.id)
    @cursor_graph.z = 10000
    @cursor_graph.speed = [ rand(10) - 5, rand(10) - 5]
    p @cursor_graph
    Eruta.show_mouse_cursor=false
  end
  

  # Closes the UI subsystem.
  def self.close
    @hanao = nil
  end

  # Initializes the UI subsystem
  def self.open()
    self.load_common_data
    @hanao = Zori::Hanao.new()
    p @hanao
    return @hanao
  end
  
  # Returns the active top-level controller of the UI
  def self.active
    p @hanao
    return @hanao
  end
  
  # Makes a new UI page 
  def self.make_page(name)
    page = Zori::Page.new(name)
    yield page if block_given?
    return page
  end

  # send an event to the UI subsystem. this may NOT be named on_poll
  # or we get infinite recursion due to inheritance from Object.
  def self.on_event(*args)
    if args[0] == :mouse_axes
      x = args[2]
      y = args[3]
      @mouse_graph.position = [x, y] if @mouse_graph
    end
    
    return Zori::Page.on_event(*args)
#     if @hanao
#       @hanao.on_event(*args)
#     else
#       puts("UI not opened!: #{args}")
#     end
  end

end




