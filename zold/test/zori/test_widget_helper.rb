require 'test_helper'
require 'zori'

# XXX: edit the other file, not ths one!!!

def test_widget_interactively()

  @game   = Gari::Game.new
  @screen = @game.openscreen(640, 480, false)
  @queue  = Gari::Event
  @ui     = Zori.open(@game, @queue)
  frame   = Zori::Frame.new
  widget  = yield(@ui)
 
  frame  << stack
  stack  << flow1
  flow1  << widget
  
  # Layout 
  frame.h  = 200
  stack.fit_wide().fit_high()
  flow1.fit_wide
  
  flow1.layout_all
  flow1.fit_child_high_simple()
  stack.layout_all
  stack.fit_child_high_simple
  frame.layout_all
  frame.fit_child_high_simple
  
  # Set main widget
  @ui.main                 << frame
  @ui.style.font.antialias = :blended

  
  until ui.done do
    @screen.slab(0,0, @screen.w, @screen.h, Gari::Color.rgb(128,128,255))
    @ui.update
    @ui.draw
    @game.update
  end
  
end