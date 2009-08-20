# 
# Tile and Sprite Editor, written in Jruby for portability, and power of the GUI. 
 
require 'awt'
require 'swing'
require 'model/tispred'
require 'ui/mditest'
require 'ui/scrible'
require 'model/indexmap'
require 'ui/toolbox'
require 'ui/paledit'


palette = Palette.new
palette << Rgb.new(255,0  ,0  )
palette << Rgb.new(0  ,255,0  )
palette << Rgb.new(0  ,0  ,255)
paledit = Paledit.new
imap    = Indexmap.new(40, 40, palette)
imap.fill(1)
app     = Tispred.new 
toolbox = Toolbox.new
ui2     = Scrible.new(imap)
ui2.next_to(toolbox)





