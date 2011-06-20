# interactive, visual test for alline . 
#

$: << '../../lib'
$: << '..'
require 'test_helper'
require 'bump'
require 'gari'
require 'gariapp'

# Small interactive app for testing Boxes.
@a    = Bump::Box.new(px: 200, py: 200, w: 20, h: 30, vx: 0, vy: 0)

class BoxTestApp < Gariapp
  def initialize(wait = nil)
    super(wait)
    @drawy = 200
    @a     = []      
    @a    << Bump::Box.new(px: 200, py: 200, w: 20, h: 30, vx: 0, vy: 0)
    @a    << Bump::Box.new(px: 300, py: 200, w: 50, h: 70, vx: 0, vy: 0)
    @a    << Bump::Box.new(px: 400, py: 200, w: 40, h: 60, vx: 0, vy: 0)
    @a1    = @a[0] 
    @a2    = @a[1]
    @a3    = @a[2]
    @col   = [ GREEN, RED, BLUE ]
    @bump  = false
    @bt    = 0
    @bp    = Bump.vec(0, 0)
    @dirty = false
    @clear = true
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
        @a1.vx= 1.0
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
      elsif event.keysym == :n
        @a2.vy=  1.0
      elsif event.keysym == :i
        @a2.vy= -1.0        
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
      elsif [:i, :n, :j].member? event.keysym 
        @a2.vy= 0.0  
      end
    end
    @dirty = true
  end
  
  def update_state
    @a2.bump_with(@a3, 1.0)
    @a1.bump_with(@a3, 1.0)    
    @a1.bump_with(@a2, 1.0)
    
    
    
   
    # @a2.bump_with(@a1, 1.0)
    
    
    # @a3.bump_with(@a1, 1.0)
    # @a3.bump_with(@a2, 1.0)
    
    @a.each { |b| b.step!(1.0) } 
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
    draw_puts(10, 70, "Bump 2: #{@a1.dp}")
    # @screen.fillrect(0, @drawy, @screen.w, 5, BLACK)
    for i in (0...@a.size)     
      @screen.fillrect(@a[i].x, @a[i].y, @a[i].w, @a[i].h, @col[i])
    end
    
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




