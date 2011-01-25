# interactive test for bump
if $0 == __FILE__
  $: << '../lib'
end

require 'gari'
require 'bump'
require 'fimyfi'

class TryBump
  include Gari::Handler
  
  # Loads a font
  def self.load_font(name, size)
    fname = Fimyfi.join(Fimyfi.font_dir, name)
    font  = Gari::Font.new(fname, size)
    warn("Could not open font #{name} in #{fname}") if(!font)
    return font      
  end

  def initialize      
    @game   = Gari::Game.new
    @queue  = Gari::Event
    super(@queue)
    @screen = @game.openscreen(640, 480, false)
    @font   = self.class.load_font('dejavuserif.ttf', 12)
    @done   = false 
    @box1   = Bump::Box.new(x: 100, y: 100, w: 50, h: 80, m: 100000.0)
    @box2   = Bump::Box.new(x: 200, y: 200, w: 30, h: 60)
    @cr     = Gari::Color.rgb(255, 0  , 0  )
    @cc     = Gari::Color.rgb(0  , 255, 255)
    @space  = Bump::Space.new(100)
    @space.add_mobile(@box1)
    @space.add_mobile(@box2)    
  end
  
  def on_keydown(ev)
    case ev.keysym
    when :left 
      @box2.v = Bump.vec(-2, 0)
    when :right 
      @box2.v = Bump.vec(2, 0)
    when :up 
      @box2.v = Bump.vec(0, -2)
    when :down 
      @box2.v = Bump.vec(0, 2)
    when :escape
      done!
    else 
      p ev.keysym
    end  
  end
  
  def on_keyup(ev)
    case ev.keysym
    when :left 
      @box2.v = Bump.vec(0, 0)
    when :right 
      @box2.v = Bump.vec(0, 0)
    when :up 
      @box2.v = Bump.vec(0, 0)
    when :down 
      @box2.v = Bump.vec(0,  0)
    else 
      p ev.keysym
    end  
  end
  
  def draw_box(box, color)
    @screen.slab(box.x, box.y, box.w, box.h, color)
  end
  
  def draw_boxes
    draw_box(@box1, @cr)
    draw_box(@box2, @cc)
  end
  
  def draw_text(x, y, text)
    @screen.text(x, y, text, @font, Gari.rgb(255,255,255), Gari.rgb(0,0,0))
  end  

  
  
  def draw_fps
    draw_text(20, 20, @game.fps)
  end

  def draw_coll
    vr = @box1.vrel(@box2)
    dp1= @box1.dp
    dp2= @box2.dp
    ov = @box1.overlap_vector(@box2)
    cn = @box1.collide_now?(@box2)
    draw_text(20, 40, "Coll: #{@coll.map{|e| e ? e.round(3) : e}} #{@how}")
    draw_text(20, 60, "Vrel 1-2:#{vr.x} #{vr.y} 
    #{ov.x} #{ov.y} #{dp1.x} #{dp1.y}")
    draw_text(20, 80, "Now?:#{cn}")
  end
  
  def draw
    @screen.slab(0, 0, @screen.w, @screen.h, Gari::Color.rgb(128,128,255))
    draw_boxes
    draw_fps
    draw_coll  
  end
 
  def update
    handle_events
    @game.nextframe
    dt = 50.0 / @game.fps
    @space.prepare(dt)
    @space.update(dt)
    @coll = * @box1.collide_t(@box2)  
  end
  
  def run
    @game.startfps
    until @done do
      self.update
      @game.update
      self.draw
    end
  end
end

tb = TryBump.new
tb.run