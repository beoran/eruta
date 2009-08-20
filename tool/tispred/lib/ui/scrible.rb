# 
# To change this template, choose Tools | Templates
# and open the template in the editor.
 
require 'swing'
require 'slide'


class Scrible < Slide::Frame 

  def initialize(clip = nil)
    super("Tispred - Clip Editor") 
    @canvas = Slide::Canvas.new(clip)
    @swing.getContentPane().add(@canvas.swing)    
    
    @swing.setDefaultCloseOperation(Swing::JFrame::EXIT_ON_CLOSE) 
    @swing.pack()
    # resize(800, 600) 
    show
  end

end
