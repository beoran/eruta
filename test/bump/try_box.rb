# interactive, visual test for alline . 
#

$: << '..'
$: << '../../lib'
require 'test_helper'
require 'bump'
require 'gari'
require 'gariapp'

# Small interactive app for testing Aline. Seems to work fine now.

class BoxTestApp < Gariapp
  def initialize(wait = nil)
    super(wait)
    @drawy = 200
    @a1    = Bump::Box.new( px: 100, py: 200, w: 20, h: 30, vx: 0, vy: 0)
    @a2    = Bump::Box.new( px: 300, py: 200, w: 50, h: 70, vx: 0, vy: 0)
    @bump  = false
    @bt    = 0
    @bp    = Bump.vec(0, 0)
    @dirty = false
    @clear = false
  end
  
  def handle_event(event)
    # super
    done! if event.quit?
    if event.keydown?
      if event.keysym == :left
        @a1.vx= -1.0
      elsif event.keysym == :right
        @a1.vx= 1.0
      elsif event.keysym == :up
        @a1.vy= -1.0
      elsif event.keysym == :down
        @a1.vy= 1.0  
      elsif event.keysym == :kp6
        @a1.vx= 2.0
      elsif event.keysym == :kp4
        @a1.vx= -1.0
      elsif event.keysym == :k
        @a2.vx= -1.0
      elsif event.keysym == :l
        @a2.vx=  1.0
      elsif event.keysym == :m
        @a2.vx=  2.0
      elsif event.keysym == :j
        @a2.vx= -2.0
      elsif event.keysym == :c
        @clear = !@clear      
      elsif event.keysym == :escape
        self.done!    
      else 
        printf "#{event.keysym} "
      end
    end
    if event.keyup?
      if [:left, :right, :kp6, :kp4].member? event.keysym 
        @a1.vx= 0.0
      elsif [:up, :down].member? event.keysym 
        @a1.vy= 0.0
      elsif [:k, :l, :m, :j].member? event.keysym 
        @a2.vx= 0.0
      end
    end
    @dirty = true
  end
  
  def update_state
    @a1.step!(1.0)
    @a2.step!(1.0)
    @bump = @a1.bump_now?(@a2)
    @bt   = @a1.bump_time(@a2) || @bt
    @bp   = @a1.bump_p(@a2)    || @bp
    @bh   = @bp ? @bp + @a1.r  : @bh
    @bl   = @bp ? @bp - @a1.r  : @bl
  end
  

  
  def render(screen)
    @screen.fill(BLACK) if @clear
    draw_puts(10, 30, "Box: #{@a1.x} ; #{@a1.y} ; #{@a1.w} ; #{@a1.h}")
    draw_puts(10, 50, "Bump: #{@bump} ; #{@bt} ; #{@bp} ; #{@bh} #{@bl}")
    @screen.fillrect(0, @drawy, @screen.w, 5, BLACK)
        
    @screen.fillrect(@a1.x, @a1.y, @a1.w, @a1.h, GREEN)
    @screen.fillrect(@a2.x, @a2.y, @a2.w, @a2.h, RED)
    
#     if @bp
#       @screen.fillrect(@bp.x, @bp.y, 1, 1, YELLOW)
#     end
#     if @bh
#       @screen.fillrect(@bh.x, @bh.y, 1, 1, MAGENTA)      
#     end  
#     if @bl
#       @screen.fillrect(@bl.x, @bl.y, 1, 1, ORANGE)
#     end  
  
    # allow fps display
    super(screen)
  end
end

app = BoxTestApp.new()
app.run




