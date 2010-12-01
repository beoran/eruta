#
#
#

$: << '.'
require 'libgari.so'
game    = Gari::Game.new
screen  = game.openscreen(640, 480, false)
yellow  = Gari::Color.rgb(255, 255, 0)
im1     = Gari::Image.new(16 ,  16, 16, 0)
im1.slab(0,0,16,16, yellow)
sh1     = Gari::Sheet.new(im1)
p sh1.image
screen.blit(20, 20, im1)
im1     = nil
GC.start
p sh1.image
screen.blitsheet(50, 50, sh1)
 
game.update
sleep(3)







