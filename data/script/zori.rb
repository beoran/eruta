# encoding: UTF-8
# Zori Eruta Edition:
#
# Zori is a GUI library for mruby and the Eruta engine. 
# beoran@gmail.com, 2009, 2010
#
# Zori (japanese footwear) follows the Shoes philosophy in that it sees 
# width as largely static, and height as dynamic and scrollable because 
# of the mouse wheel and pageup/pagedown keys.
#
# Zori is oriented towards relatively simple UIs. 
# Therefore, there is only one top-level widget, the Root widget. 
# This widget, in turn contains several Pages, of which only
# one can be active at the same time. 
# The default page is used for the in game HUD, NPC or PC dialog and 
# status display. Other pages are used for the various menus.
# Every page can contain can contain any amount of child widgets. 
# Widgets in turn may or may not contain sub-widgets.
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
# The work in Zori is divided between Root, Page, and Widget. The Root class,
# of which the root widget is the only instance, handles everything that 
# depends on and/or may influence several widgets at once, such as event 
# dispatching but also setting the focus, determining which widget is 
# being hovered, or dragged, etc. The latter functions 
# change the state of several widgets, so they are handled on the level of 
# the Hanao class.
# The Widget module and the widget classes that include it the individual 
# state and actions of the various widgets individually.
#
# Widget state:
# A widget can be considered a state machine with the following states:
# * hidden  : Invisible (implies inactive)
# * inactive: Will ignore input
# * active  : Ready to receive input 
# * marked  : Currently "hovered" by keyboard cursor. Only one widget can be
#             marked.
# * selected: Previously "pressed" on and still relevantly "selected". 
# * hovered: Currently "hovered" by mouse cursor. Only one widget can be hovered
#            at most. 
# * focused : This widget and it's children receive keyjoy input excusively
#             some simpler widgets such as buttons cannot be focused since they
#             perform their action immediately when "clicked". Only
#             one widget can be focused a the same time. 
# * triggered:Widgets that can't be focused become triggered in stead while
#             they perform their action.
# * dragged : The widget is being dragged. 
#
# The all states and mutually exclusive except for the "marked" and "hovered"
# state which are different since one widget may be e.g. focused and marked
# and hovered at the same time. Yet different is the selected flag,
# which keeps track of previously marked items such as the marked parent menu,
# or for exammple, when swapping two items in an item list, etc. 
# Therefore marked and hovered are separate flag from the rest of the states.  
#
# Allowed transitions:
# * hide    : Set to hidden state.
# * show    : Set to active state from hidden state.
# * disable : Set to inactive state from active state or higher.
# * enable  : Set to active state from inactive state.
# * mark    : Set to marked state.
# * focus   : Set to focused state.
# * trigger : Set to triggered state.
#  
# The state is orthogonal with the value of the widget. A pushbutton or
# checkbox will have a boolean value, a slider a numerical one,
# and a text boxt a string value.
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

#  script 'zori/rect.rb'
#  script 'zori/handler.rb'
#  script 'zori/element.rb'

  
  script 'zori/style.rb'
  script 'zori/state.rb'
  script 'zori/capability.rb'
  script 'zori/graphic.rb'  
  script 'zori/widget.rb'
  script 'zori/draggable.rb'
  script 'zori/button.rb'
  script 'zori/longtext.rb'
  script 'zori/menu.rb'  
  script 'zori/page.rb'
  script 'zori/root.rb'

#   script 'zori/mouse.rb'
#   script 'zori/joystick.rb' 
#   script 'zori/keyboard.rb' 
  
#   script 'zori/control.rb'

#   script 'zori/hanao.rb'
  
#   script 'zori/sizemixin.rb'
#   script 'zori/layoutmixin.rb'
#   script 'zori/flowlayout.rb'
#   script 'zori/stacklayout.rb'
#   script 'zori/fixedlayout.rb'
#   script 'zori/widget.rb'
# 
#   script 'zori/button.rb'
#   script 'zori/checkbox.rb'
#   script 'zori/flow.rb'
#   script 'zori/frame.rb'
#   script 'zori/input.rb'
#   script 'zori/label.rb'
#   script 'zori/mainwidget.rb'
#   script 'zori/menu.rb'
#   script 'zori/menu.rb'
#   script 'zori/menuitem.rb'
#   script 'zori/progress.rb'
#   script 'zori/radiobutton.rb'
#   script 'zori/ring.rb'
# 
#   script 'zori/slider.rb'
#   script 'zori/stack.rb'
# 
#   script 'zori/style.rb'
#   script 'zori/menu.rb'
#   script 'zori/text.rb'
#   script 'zori/shortcut.rb'
#   # script 'zori/mapeditor.rb'
#   script 'zori/mouse.rb'
#   script 'zori/keyboard.rb'
#   script 'zori/joystick.rb'
# 
#   script 'zori/dialog.rb'
#   script 'zori/console.rb'
  
  
  # Non-mouse cursor graph
  def self.cursor_graph
    self.root.cursor_graph
  end
  
  # Non-mouse cursor image
  def self.cursor_image
    self.root.cursor_image
  end

  # Mouse cursor graph
  def self.mouse_graph
    self.root.mouse_graph
  end
  
  # Mouse cursor image
  def self.mouse_image
    self.root.mouse_image
  end
  
  # Default font for the UI.
  def self.font
    self.root.font
  end

  # Closes the UI subsystem.
  def self.close
    @root = nil
  end

  # Initializes the UI subsystem
  def self.open()
    @root = Zori::Root.new()
    return @root
  end
  
  # Returns the root, that is the active top-level controller of the UI
  def self.root
    unless @root
      self.open
    end
    return @root
  end
  
  # Makes a new UI page 
  def self.make_page(name)
    page = Zori::Page.new(:name => name)
    yield page if block_given?
    @root.register(page)
    return page
  end
  
  # Activate a given UI page.
  def self.go(name)
    self.root.go(name)
  end

  # Looks up a registered page by page name
  def [](page_name)
    return self.root[page_name]
  end

  # Send an event to the UI subsystem. This may NOT be named on_poll
  # or we get infinite recursion due to inheritance from Object.
  def self.on_event(*args)
    return self.root.on_event(*args)
  end
end




