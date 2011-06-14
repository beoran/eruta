#!/usr/local/bin/ruby
#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#

BEGIN {
  # Include this script's directory  and one up in Ruby's search path.
  $:.unshift(File.dirname(__FILE__))
  $:.unshift(File.join(File.dirname(__FILE__), '..'))
}

require 'test_wrapper' 

class TestBracelet < Testwrapper

def test_bracelet
  cursor_name = Settings.cursor_dir + 'square.png'
  icon1_name  = Settings.icon_dir   + 'icon_1.png'
  icon2_name  = Settings.icon_dir   + 'icon_2.png'
  icon3_name  = Settings.icon_dir   + 'icon_3.png'
  icon4_name  = Settings.icon_dir   + 'icon_4.png'
  icon5_name  = Settings.icon_dir   + 'icon_5.png'
  icon6_name  = Settings.icon_dir   + 'icon_6.png'
  cursor      = Image.load_alpha(cursor_name)
  icon1       = Image.load_alpha(icon1_name)
  icon2       = Image.load_alpha(icon2_name)
  icon3       = Image.load_alpha(icon3_name)
  icon4       = Image.load_alpha(icon4_name)
  icon5       = Image.load_alpha(icon5_name)
  icon6       = Image.load_alpha(icon6_name)
  
  
  bracelet    = Bracelet.new(200, 200) 
  
  ring        = bracelet.ring()  { |menu, item| puts item.text }
  sidering    = bracelet.ring()  { |menu, item| puts item.text ; puts "From side ring" }
  subring     = Ring.new(200, 200)  { |menu, item| puts item.text }
  subsubring  = Ring.new(200, 200)  { |menu, item| puts item.text }
  
  sidering.item('Item 1', 'Sidering Item 1 Text', icon1, subsubring) 
  sidering.item('Item 2', 'Sidering Item 2 Text', icon2)
  sidering.item('Item 3', 'Sidering Item 3 Text', icon3)
  sidering.item('Item 4', 'Sidering Item 4 Text', icon4)
  
  
  subsubring.item('Item 1', 'Sub Sub Ring Item 1 Text', icon1) 
  subsubring.item('Item 2', 'Sub Sub Ring Item 2 Text', icon2)
  
  
  subring.item('Item 1', 'Subring Item 1 Text', icon1, subsubring) 
  subring.item('Item 2', 'Subring Item 2 Text', icon2)
  subring.item('Item 3', 'Subring Item 3 Text', icon3)
  subring.item('Item 4', 'Subring Item 4 Text', icon4)
  
  
  # ring.cursor_bitmap = cursor
  ring.item('Item 1', 'Item 1 Text', icon1, subring) 
  ring.item('Item 2', 'Item 2 Text', icon2)
  ring.item('Item 3', 'Item 3 Text', icon3)
  ring.item('Item 4', 'Item 4 Text', icon4)
  ring.item('Item 5', 'Item 5 Text', icon5)
  ring.item('Item 6', 'Item 6 Text', icon6)
  ring.item('Item 1', 'Item 7 Text', icon1) 
  ring.item('Item 2', 'Item 8 Text', icon2)
  ring.item('Item 3', 'Item 9 Text', icon3)
  ring.item('Item 4', 'Item 10 Text', icon4)
  ring.item('Item 5', 'Item 11 Text', icon5)
  ring.item('Item 6', 'Item 12 Text', icon6)
  ring.item('Item 1', 'Item 13 Text', icon1) 
  ring.item('Item 2', 'Item 14 Text', icon2)
  ring.item('Item 3', 'Item 15 Text', icon3)
  ring.item('Item 4', 'Item 16 Text', icon4)
  ring.item('Item 5', 'Item 17 Text', icon5)
  ring.item('Item 6', 'Item 18 Text', icon6)
  
  
  @gui       << bracelet
  @gui.focus_widget = bracelet # Focus the bracelet
  
  until @gui.done or bracelet.close? do
          @screen.fill([0,0,0])
          @gui.update
          @gui.draw
          @gui.flip
  end 
  
  
  Rubygame.quit()

end

end # Class

TestBracelet.autorun
