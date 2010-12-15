# encoding: UTF-8
require 'test_helper'
p $:

require 'gari'
require 'fimyfi'
require 'zori'

assert { Gari              }
assert { Zori::Button      }
assert { Zori::Checkbox    }
assert { Zori::Dialog      }
assert { Zori::Flow        }
assert { Zori::Flowlayout  }
assert { Zori::Frame       }
assert { Zori::Input       }
assert { Zori::Label       }
assert { Zori::Mainwidget  }
assert { Zori::Menu        }
assert { Zori::Menubar     }
assert { Zori::Menuitem    }
assert { Zori::Progress    }
assert { Zori::Radiobutton }
assert { Zori::Ring        }
assert { Zori::Sizemixin   }
assert { Zori::Stack       }
assert { Zori::Stacklayout }
assert { Zori::Style       }
assert { Zori::Submenu     }
assert { Zori::Hanao       }
assert { Zori::Text        }
assert { Zori::Widget      }
assert { Zori::Shortcut    }
# assert { Zori::Mapeditor   }



@game   = Gari::Game.new
@screen = @game.openscreen(640, 480, false)
@queue  = Gari::Event
@ui     = Zori.open(@game, @queue)
ui      = @ui
assert { @ui.wide == @screen.w }
assert { @ui.high == @screen.h } 
uinow   = Zori::Hanao.current()
assert { uinow.defaultfont }


widget  = Zori::Frame.new
# Set up a test menu
menu        = Zori::Menubar.new
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
   
   res = Zori::Dialog.yesno('What will it be, yes or no?')
   p res  

  
  until ui.done do
    progress.advance
    @screen.slab(0,0, @screen.w, @screen.h, Gari::Color.rgb(128,128,255))
    ui.update
    ui.draw
    @game.update
  end



