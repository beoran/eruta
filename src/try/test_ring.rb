#!/usr/bin/env ruby
#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#

BEGIN { $:.unshift(File.dirname(__FILE__)); }
# Include this script's directory in Ruby's search path.

require 'test_wrapper'

class Testring < Testwrapper

  def test_ring
    cursor_name = 'square.png'
    icon1_name  = 'icon_1.png'
    icon2_name  = 'icon_2.png'
    icon3_name  = 'icon_3.png'
    icon4_name  = 'icon_4.png'
    icon5_name  = 'icon_5.png'
    icon6_name  = 'icon_6.png'
    cursor      = Zori::Hanao.load_image('cursor', cursor_name)
    icon1       = Zori::Hanao.load_image('icon',icon1_name)
    icon2       = Zori::Hanao.load_image('icon',icon2_name)
    icon3       = Zori::Hanao.load_image('icon',icon3_name)
    icon4       = Zori::Hanao.load_image('icon',icon4_name)
    icon5       = Zori::Hanao.load_image('icon',icon5_name)
    icon6       = Zori::Hanao.load_image('icon',icon6_name)


    ring        = Zori::Ring.new(200, 200)  { |menu, item| puts item.text }
    subring     = Zori::Ring.new(200, 200)  { |menu, item| puts item.text }
    subsubring  = Zori::Ring.new(200, 200)  { |menu, item| puts item.text }

    subsubring.item('Item 1', 'Sub Sub Ring Item 1 Text', icon1) 
    subsubring.item('Item 2', 'Sub Sub Ring Item 2 Text', icon2)


    subring.item('Item 1', 'Subring Item 1 Text', icon1, subsubring) 
    subring.item('Item 2', 'Subring Item 2 Text', icon2)
    subring.item('Item 3', 'Subring Item 3 Text', icon3)
    subring.item('Item 4', 'Subring Item 4 Text', icon4)


    # ring.cursor_bitmap = cursor
    ring.item('Item 1', 'Item 1 Text' , icon1, subring) 
    ring.item('Item 2', 'Item 2 Text' , icon2)
    ring.item('Item 3', 'Item 3 Text' , icon3)
    ring.item('Item 4', 'Item 4 Text' , icon4)
    ring.item('Item 5', 'Item 5 Text' , icon5)
    ring.item('Item 6', 'Item 6 Text' , icon6)
    ring.item('Item 1', 'Item 7 Text' , icon1) 
    ring.item('Item 2', 'Item 8 Text' , icon2)
    ring.item('Item 3', 'Item 9 Text' , icon3)
    ring.item('Item 4', 'Item 10 Text', icon4)
    ring.item('Item 5', 'Item 11 Text', icon5)
    ring.item('Item 6', 'Item 12 Text', icon6)
    ring.item('Item 1', 'Item 13 Text', icon1) 
    ring.item('Item 2', 'Item 14 Text', icon2)
    ring.item('Item 3', 'Item 15 Text', icon3)
    ring.item('Item 4', 'Item 16 Text', icon4)
    ring.item('Item 5', 'Item 17 Text', icon5)
    ring.item('Item 6', 'Item 18 Text', icon6)


    @gui       << ring
    ring.focus! # Focus the ring

    until @gui.done or ring.close? do
      @screen.fill([0,0,0])
      @gui.update
      @gui.draw(@screen)
      @screen.flip
    end 
  end
end


Testring.autorun
