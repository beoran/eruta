# 
# To change this template, choose Tools | Templates
# and open the template in the editor.

require 'slide/action' 
require 'slide/menu' 
require 'slide/button' 

require 'slide/frame' 
require 'slide/layout' 

class Toolbox  < Slide::Frame
  
  def add_button(text)    
    button          = Slide::Button.new(text,  nil)
    bs              = "button_#{text}".to_sym
    button.action() do | act | 
        puts "Button action #{act} #{bs}"
        self.send(bs, bs) if self.respond_to?(bs)
    end
    button.resize(@button_size)
    @panel.add(button)
    @buttons << button
  end
  
  def initialize
    super("Tispred -- Toolbox")
    @layout_rows  = 8
    @layout_cols  = 2
    @button_count = @layout_rows * @layout_cols
    @button_size  = 40 
    @panel        = Slide::Panel.new()
    @layout       = Slide::Layout::Grid.new(@layout_rows, @layout_cols)
    @panel.layout = @layout
    
    self.resize(@button_size * @layout_cols, @button_size * @layout_rows)
    @buttons          = Array.new(@button_count)
    buttsym           = []
    for index in 0...@button_count do
      add_button(index.to_s)
    end
    
    self.add_menubar(self)
    @filemenu = self.add_menu('File') { |act| puts "Menu: File #{act} called." }
    @filemenu.menuitem('New')     { |act| puts "Menu: New #{act} called." }
    @filemenu.menuitem('Save')    { |act| puts "Menu: Save #{act} called." }
    @filemenu.menuitem('Save As') { |act| puts "Menu: Save As  #{act} called." }
    @filemenu.menuitem('Quit')    { |act| puts "Menu:  #{act} called." }
    self.add(@panel)
    self.show
  end  
  
  def button_1(*args)
    puts "Button 1 pressed."
  end
  
end