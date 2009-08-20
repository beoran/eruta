# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'slide'

class Paledit < Slide::Frame
  include Slide::Wrapper
  
  def add_button(text, button_action)    
    button          = Slide::Button.new(text,  nil)
    button.action(button_action) do | act | 
        puts "Button action #{act}"
        self.send(act, act) if self.respond_to?(act)
    end
    @editpanel.add(button)
    @editpanel_buttons << button
  end
  
  def add_color_button(r, g, b, a)    
    index           = @palette.size
    button          = Slide::Button.new("#{index} #{r} #{g} #{b} #{a}",  nil)
    button.background_rgba(r, g, b, a)
    button_action   = "color_button_#{index}".to_sym
    button.action(button_action) do | act | 
        puts "Button action #{act}"
        @palette.action(index)
    end
    @palette.add_rgba(r, g, b, a)
    button.resize(@button_size)
    @colorpanel.add(button)
    @color_buttons << button
  end

  
  def initialize(palette)
    super("Tispred - Palette")
    @palette          = palette
    @colorwide        = 12
    @colorhigh        = 24
    @button_size      = 20
    
    @mainpanel        = Slide::Panel.new()
    @mainlayout       = Slide::Layout::Grid.new(1, 2) 
    @mainpanel.layout = @mainlayout
    
    @editpanel        = Slide::Panel.new()
    @editlayout       = Slide::Layout::Grid.new(5, 1)     
    @editpanel.layout = @editlayout
    
    
    
    @colorpanel       = Slide::Panel.new()
    @colorlayout      = Slide::Layout::Grid.new(@colorwide, @colorhigh)     
    @colorpanel.layout= @colorlayout
    
    
    @mainpanel.add      @editpanel
    
    
    
    
    
    self.add mainpanel
    
    # @chooser = Slide::Colorchooser.new
    # self.add @chooser
    # @swing.pack
    self.show
  end
  
end
