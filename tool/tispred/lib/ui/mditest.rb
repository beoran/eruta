# 
# Main window of the Tispred aplication
 
require 'swing'

class Mditest < Swing::JFrame
  
  class Testframe < Swing::JInternalFrame
    def initialize() 
    x_offset = 30 
    y_offset = 30
    super("Test of internal frame",
          true, #resizable
          true, #closable
          true, #maximizable
          true) #iconifiable
    setSize(100,100)
    # Set the window's location.
    setLocation(x_offset, y_offset)
    end      
  end
  
  def initialize
    super("Tispred -- Tile and Sprite Editor")
    setSize(500,500)
    @desktop = Swing::JDesktopPane.new()
    self.add(@desktop)
    frame_create
    self.setExtendedState(Swing::JFrame::MAXIMIZED_BOTH)
    setVisible(true)
  end
  
  protected 
  
  def frame_create() 
    frame = Testframe.new
    frame.setVisible(true)
    @desktop.add(frame)
    begin
        frame.setSelected(true)
    rescue java.beans.PropertyVetoException => e
      puts "Not allowed."
    end  
  end
  
end
