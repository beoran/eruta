# encoding: UTF-8
# Zori: 
#
# Yet another  GUI library for Ruby/SDL ( though Sisa  )
# beoran@gmail.com, 2009
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
# parent widget and it's contents and/or child widgets  
#
# When automatically arranging widgets, the margin of a widget determines how 
# closely that widget may be packed to other widgets that are on the same level.
# 
# The work in Zori is divided between Hanao and Widget. The Hanao class 
# handles everything that depends on and/or may influence several widgets at 
# once, such as event dispatching but also setting the focus, determining which 
# widget is being hovered, or dragged, etc. The latter fuctions change the state 
# of several widgets, so they are handled on the level of the system.
# The Widget class and it's child classes handle the individual state and
# actions of the various widgets individually.
#


module Zori
  DEBUG   = true

  autoload :Button      , 'zori/button'
  autoload :Checkbox    , 'zori/checkbox'
  autoload :Dialog      , 'zori/dialog'
  autoload :Flow        , 'zori/flow'
  autoload :Flowlayout  , 'zori/flowlayout'
  autoload :Frame       , 'zori/frame'
  autoload :Input       , 'zori/input'
  autoload :Label       , 'zori/label'
  autoload :Mainwidget  , 'zori/mainwidget'
  autoload :Menu        , 'zori/menu'
  autoload :Menubar     , 'zori/menu'
  autoload :Menuitem    , 'zori/menuitem'
  autoload :Progress    , 'zori/progress'
  autoload :Radiobutton , 'zori/radiobutton'
  autoload :Ring        , 'zori/ring'
  autoload :Sizemixin   , 'zori/sizemixin'
  autoload :Stack       , 'zori/stack'
  autoload :Stacklayout , 'zori/stacklayout'
  autoload :Style       , 'zori/style'
  autoload :Submenu     , 'zori/menu'
  autoload :Hanao       , 'zori/hanao'
  autoload :Text        , 'zori/text'
  autoload :Widget      , 'zori/widget'
  
  def self.open(screen, queue) 
    return Zori::Hanao.new(screen, queue)
  end
  


  
end



if $0 == __FILE__
  require 'sisa'
  p Zori::Style
  p Sisa::Joystick.active?
  Sisa::Joystick.open_all() 
  p Sisa::Joystick.active?
  screen = Sisa::Screen.make(640, 480, false);
  queue  = Sisa::Event.queue
  ui     = Zori.open(screen, queue)
  p ui.wide
  p ui.high
  ui     = Zori::Hanao.current()
  
  p ui.wide
  p ui.high
  p ui.style
  widget  = Zori::Frame.new
   
  for n in Sisa::Key.constants do
    next unless n =~ /\AMOD_/
    p n
    p Sisa::Key.const_get(n)
  end 
  
  # Set up a test menu
  menu        = Zori::Menubar.new()
  # menu.toplevel= true
  # showfile    = Zori::Menuitem.new("File")
  # showedit    = Zori::Menuitem.new("Edit")     
  filesave    = Zori::Menuitem.new("Save")  { puts "File -> Save!"  }
  fileopen    = Zori::Menuitem.new("Open")  { puts "File -> Open!"  }
  editcopy    = Zori::Menuitem.new("Copy")  { puts "Edit -> Copy!"  }
  editpaste   = Zori::Menuitem.new("Paste") { puts "Edit -> Paste!" }
  filemenu    = Zori::Submenuitem.new("File")
  editmenu    = Zori::Submenuitem.new("Edit")
  
  filemenu    << fileopen
  filemenu    << filesave
  editmenu    << editcopy
  editmenu    << editpaste
  # showfile    << filemenu
  # showedit    << editmenu
  # menu        << showfile
  # menu        << showedit
  menu        << filemenu
  menu        << editmenu
 
  menu.layout_children
  filemenu.layout_children
  
  
  
  stack   = Zori::Stack.new()
  flow1   = Zori::Flow.new()
  flow2   = Zori::Flow.new()
  inbox   = Zori::Input.new()       { |w| puts "Input value: #{w.value}"        }
  inbox2  = Zori::Input.new()       { |w| puts "Input 2 value: #{w.value}"      }
  progress= Zori::Progress.new()    { puts "Progress full!!!"                   }
  checkb  = Zori::Checkbox.new()    { |v| puts "Checkbox clicked! #{v.value}"   }
  radiob  = Zori::Radiobutton.new() { |v| puts "Radiobutton clicked! #{v.value}"}
  radiob.value = 1
  label   = Zori::Label.new("Ünicöde supported! Why don't you wrap this very long line over several lines?\nI'm a label\nthat spans over multiple lines.")
  label2  = Zori::Label.new("And I'm a very wide label.") 
  # "that should really not fit at all on a single line, so I added some more text to it to make really sure it would overflow.")
  # label.w = 200
      
  button1 = Zori::Button.new("Left!") {   
    widget.style.alignment = :left
    label.style.alignment  = :left
    widget.layout_all     
    res = Zori::Dialog.yesno('What will it be, yes or no?')
    p res  
  }
  
  button2 = Zori::Button.new("Middle!") {   
    widget.style.alignment = :middle
    label.style.alignment  = :middle
    widget.layout_all
    inbox.hide
  }
  
  
  button3 = Zori::Button.new("Right!") { |w|
    w.parent.style.alignment = :right
    label.style.alignment    = :right
    w.parent.layout_all
    inbox.show
  } 
    
  widget << stack
  
  stack  << menu
  stack  << flow1
  stack  << flow2
  
  flow1  << button1
  flow1  << button2
  flow1  << button3
  flow1  << inbox2
  flow2  << label2
  flow2  << progress
  flow2  << label 
  flow2  << checkb
  flow2  << radiob
  flow2  << inbox
  
  # Layout 
  widget.h  = 200
  stack.fit_wide().fit_high()
  flow1.fit_wide
  flow2.fit_wide
  # flow1.fit_child_high()
  # flow2.fit_child_high()
  
  menu.fit_wide()
  menu.layout_all
  menu.fit_child_high_simple()
  filemenu.layout_all
  filemenu.fit_child_high_simple()
  filemenu.fit_child_wide_simple()
  editmenu.layout_all
  editmenu.fit_child_high_simple()
  editmenu.fit_child_wide_simple()
  
  flow1.layout_all
  flow1.fit_child_high_simple()
  flow2.layout_all
  flow2.fit_child_high_simple()
  stack.layout_all
  stack.fit_child_high_simple
  widget.layout_all
  widget.fit_child_high_simple
  
  # Set main widget
  ui.main                 << widget
  ui.style.font.antialias = :blended
  # Zori::Dialog.textsplash('This will disappear in a few seconds')
  # answer = Zori::Dialog.ask('What do you say?')
  # p answer
  
  until ui.done do
    progress.advance
    screen.fill(Sisa::Color.new(128,128,255))
    ui.update
    ui.draw
    screen.flip
  end
   
   
  
end
