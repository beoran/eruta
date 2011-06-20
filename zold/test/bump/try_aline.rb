# interactive, visual test for alline . 
#

$: << '..'
$: << '../../lib'
require 'test_helper'
require 'bump'
require 'gari'
require 'gariapp'

# Small interactive app for testing Aline. Seems to work fine now.

class AlineTestApp < Gariapp
  def initialize(wait = nil)
    super(wait)
    @a1    = Bump::Aline.new(100, 20, 0)
    @a2    = Bump::Aline.new(300, 50, 0)
    @bump  = false
    @bt    = 0
    @bc    = 0
    @dirty = false
    @drawy = 200
    @clear = false
  end
  
  def handle_event(event)
    # super
    done! if event.quit?
    if event.keydown?
      if event.keysym == :left
        @a1.v= -1.0
      elsif event.keysym == :right
        @a1.v= 1.0
      elsif event.keysym == :kp6
        @a1.v= 2.0
      elsif event.keysym == :kp4
        @a1.v= -1.0
      elsif event.keysym == :k
        @a2.v= -1.0
      elsif event.keysym == :l
        @a2.v=  1.0
      elsif event.keysym == :m
        @a2.v=  2.0
      elsif event.keysym == :j
        @a2.v= -2.0
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
        @a1.v= 0.0
      end
      if [:k, :l, :m, :j].member? event.keysym 
        @a2.v= 0.0
      end        
    end
    @dirty = true
  end
  
  def update_state
    @a1.step!(1.0)
    @a2.step!(1.0)
    @bump = @a1.bump_now?(@a2)
    @bt   = @a1.bump_time(@a2) || @bt
    @bc   = @a1.bump_c(@a2)    || @bc
    @bh   = @bc ? @bc + @a1.r  : @bh
    @bl   = @bc ? @bc - @a1.r  : @bl
  end
  

  
  def render(screen)
    @screen.fill(BLACK) if @clear
    draw_puts(10, 30, "Line: #{@a1.hi} ; #{@a2.lo}")
    draw_puts(10, 50, "Bump: #{@bump} ; #{@bt} ; #{@bc} ; #{@bh} #{@bl}")
    @screen.fillrect(0, @drawy, @screen.w, 5, BLACK)
    @screen.line(@a1.lo, @drawy, @a1.r2, 0, GREEN)
    @screen.line(@a2.lo, @drawy, @a2.r2, 0, RED)
    if @bc
      @screen.line(@bc, @drawy - 10, 0, 20, YELLOW)
    end
    if @bh
      @screen.line(@bh, @drawy - 10, 0, 20, MAGENTA)
    end  
    if @bl
      @screen.line(@bl, @drawy - 10, 0, 20, ORANGE)
    end  
  
    # allow fps display
    super(screen)
  end
end

app = AlineTestApp.new()
app.run




