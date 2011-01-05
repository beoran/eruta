require 'test_helper'
require 'test_widget_helper'

require 'gari'
require 'fimyfi'
require 'zori'

assert { Gari              }
assert { Zori::Slider      }


# 
# @game   = Gari::Game.new
# @screen = @game.openscreen(640, 480, false)
# @queue  = Gari::Event
# @ui     = Zori.open(@game, @queue)
# ui      = @ui
# assert { @ui.wide == @screen.w }
# assert { @ui.high == @screen.h } 
# uinow   = Zori::Hanao.current()
# assert { uinow.defaultfont }
# 
# 
#   widget  = Zori::Frame.new
#   
#   
#   stack   = Zori::Stack.new()
#   flow1   = Zori::Flow.new()
#   flow2   = Zori::Flow.new()
test_widget_interactively do
  slider  = Zori::Slider.new()      { |v| "Slider value changed #{v.value}" }
  slider
end
# 
#   widget << stack
#   stack  << flow1
#   stack  << flow2
#   flow2  << slider
#   
#   # Layout 
#   widget.h  = 200
#   stack.fit_wide().fit_high()
#   flow1.fit_wide
#   flow2.fit_wide
#   # flow1.fit_child_high()
#   # flow2.fit_child_high()
  

  
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

  
  until ui.done do
    @screen.slab(0,0, @screen.w, @screen.h, Gari::Color.rgb(128,128,255))
    ui.update
    ui.draw
    @game.update
  end

