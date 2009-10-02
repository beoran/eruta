require 'narray'


module Rogaru
  # A module that helps drawing particle effects
  module Stardust
    
    
    DUST_ALIVE= 0
    DUST_X    = 1
    DUST_Y    = 2
    DUST_VX   = 3
    DUST_VY   = 4
    DUST_TIME = 5
    # A particle is represented, for speed, as an NArray
    def self.make_particle(x, y, vx, vy, time, *others)
      size          = others.size + 6
      arr           = NArray.float(size)
      arr[DUST_X]   = x
      arr[DUST_Y]   = y
      arr[DUST_VX]  = vx
      arr[DUST_VY]  = vy
      arr[DUST_TIME]= time
      index         = DUST_TIME + 1
      for other in others do
        arr[index]  = other
        index += 1  
      end  
      # arr = [true, x, y, vx, vy, time, *others]
      class << arr
        def alive? 
          self[DUST_ALIVE] == 1.0
        end
        def alive=(val)
          self[DUST_ALIVE] = val ? 1.0 : 0.0
        end
      end
      return arr
    end
    
    # A stream is a set of particles that follow the same rules.
    class Stream
    
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
          vx    = (rand(100) / 10.0) - 5 
          vy    = (rand(100) / 10.0) - 5 
          # Speeds set randomly for this kind of stream          
          time  = (rand(@timemax) / 1.5)
          # Randomly shorten their lifetime
          return Stardust.make_particle(0.0, 0.0, vx, vy, time)
      end
      
      # Compensation for map scrolling, etc
      attr_accessor :vx_fudge
      # Compensation for map scrolling, etc
      attr_accessor :vy_fudge
      attr_accessor :hide
      
      def initialize(color = [255,255,255 ], amount = 100, max = 200)
        @screenw= 640
        @screenh= 480
        @color  = color
        @amount = amount
        @timemax= max
        @vx_fudge = 0 # Compensation for map scrolling, etc
        @vy_fudge = 0 # Compensation for map scrolling, etc
        @particles = []
        amount.times do 
          @particles << init_particle
        end 
        @hide      = false
      end  
      
      # Updates the movement of a single particle. 
      # Override this in child classes.
      # Should return whether or not the particle is still alive.
      def update_particle(particle, time)        
          if particle[DUST_TIME] && particle[DUST_TIME]  > @timemax
            return false
          end     
          return true     
      end
      
      # Advances time for a particle
      # You may or may not have to override this.
      # it is clalled automagically BEFORE update_particle
      # if particle[DUST_TIME] is not nill
      def time_particle(particle, time)
        if particle[DUST_TIME] # allow for eternal paricles
          particle[DUST_TIME] += time 
        end  
      end
      
      
      # Standard motion for a particle. 
      # You may or may not have to override this.
      # it is clalled automagically after update_particle
      # It also automagically applies the fudge speeds
      def move_particle(particle, time)
        particle[DUST_X]    += particle[DUST_VX] -+ @vx_fudge
        particle[DUST_Y]    += particle[DUST_VY]  + @vy_fudge
      end
      
      
      def update(time = 1.0)
        @particles.reject! { |p| ! p.alive?}
        # remove all killed particles
        # one step too late, but it's not important, to be accurate 
        # here, just to be fast.
        @particles.each do | particle |           
          time_particle(particle, time)
          alive = update_particle(particle, time)
          particle.alive = true
          move_particle(particle, time)
        end
      end
      
      # Draws a single particle. 
      # Override this in child classes.
      # The X and Y coordinats are automagically corrected,
      # so you should use them as such.
      def render_particle(x, y, surface, particle, ci) 
        surface.draw_circle(x, y, 2,  ci, true, 0.5)
      end
      
      def render(surface, x, y)
        return if @hide # dosn't display of hidden
        ci = surface.map_color(@color)
        # surface.auto_lock_off
        surface.lock
        @particles.each do | particle | 
            xx = x + particle[DUST_X]
            yy = y + particle[DUST_Y]
            render_particle(xx, yy, surface, particle, ci)
        end
        surface.unlock
        # surface.auto_lock_on
      end 
      
      def alive?
        @particles.size > 0
      end
        
      
    end
    
    class Snow < Stream
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
        x     = (rand(@screenw))
        y     = (rand(@screenh))
        vx    = 0.0
        vy    = 1
        # Speeds set randomly for this kind of stream          
        time  = rand(100)
        # Randomly shorten their lifetime
        return Stardust.make_particle(x, y, vx, vy, time)
      end
      
      def update_particle(particle, time)
        if particle[DUST_Y] > @screenh
          particle[DUST_Y]    = rand(@screenh * 9 / 10 )
          particle[DUST_X]    = rand(@screenw)
        elsif particle[DUST_TIME] > @timemax
          particle[DUST_X]    = rand(@screenw)
          particle[DUST_Y]    = rand(@screenh * 9 / 10)
          particle[DUST_TIME] = rand(100)
        end  
        if (particle[DUST_TIME] % 5) == 0
          particle[DUST_VX] = (rand(3) - 1)
        else
          particle[DUST_VX] = 0
        end  
        return true
      end
    end
    
    class Rain < Stream
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
        x     = (rand(@screenw))
        y     = (rand(@screenh))
        vx    = 0.0
        vy    = 1
        # Speeds set randomly for this kind of stream          
        time  = rand(100)
        # Randomly shorten their lifetime
        return Stardust.make_particle(x, y, vx, vy, time)
      end
      
      def update_particle(particle, time)
        if particle[DUST_X] > @screenw
          particle[DUST_X] %= @screenw
        elsif particle[DUST_X] < 0
          particle[DUST_X] = - (particle[DUST_X] % @screenw)
        end
        
        
          
        if particle[DUST_Y] > @screenh
          particle[DUST_Y]    = rand(@screenh * 9 / 10 )
          particle[DUST_X]    = rand(@screenw)
        elsif particle[DUST_TIME] > @timemax
          particle[DUST_X]    = rand(@screenw)
          particle[DUST_Y]    = rand(@screenh * 9 / 10)
          particle[DUST_TIME] = rand(100)
        end  
        return true
      end
      
      # Draws a single particle. 
      # Override this in child classes.
      def render_particle(x, y, surface, particle, ci) 
        x2 = x
        y2 = y + rand(5) + 2
        l  = 7 + rand(3)
        w  = 1 #  + rand(2) 
        # surface.draw_line(x, y, x2, y2, ci)
        surface.draw_rect(x, y, w, l, ci, true)
      end
    end
    
    # A do-nothing effect, useful as a placeholder
    # or to "swap out" with an effect to turn it on and off
    class None < Stream
      # Do nothing in stead of update
      def update(time=1.0)
      end
      # Do nothing in stead of render
      def render(surface, x, y)
      end
    end  
    
    class Blood < Stream
      BLOOD_G = DUST_TIME + 1
      def initialize(color = [255, 0, 0], amount = 15, max = 100)
        super
      end
      
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
        x     = 0.0
        y     = 0.0
        vx    = (rand(1001)- 500) / 1000.0  
        vy    = 0
        g     = 0.00000001
        # Speeds set randomly for this kind of stream          
        time  = 0 # rand(100)
        # Randomly shorten their lifetime
        return Stardust.make_particle(x, y, vx, vy, time, g)
      end
      
      def update_particle(particle, time)
        time = particle[DUST_TIME]
        if time < @timemax * 0.1
            particle[DUST_Y] -= (rand(100) + 1) / 25.0 
        elsif time  < @timemax * 0.2
          particle[DUST_Y] += (rand(100) + 1) / 25.0 
          particle[DUST_VX] /= 2   
        else
          particle[DUST_VY]  = 0
          particle[DUST_VX]  = 0   
        end
        # particle[BLOOD_G]
        # particle[DUST_VY] = 2 if particle[DUST_VY] > -2
        if time > @timemax
          return false
        end     
        return true     
      end  
     
      # Draws a single particle. 
      # Override this in child classes.
      def render_particle(x, y, surface, particle, ci) 
        surface.draw_circle(x, y, 2,  ci, true, 0.5)
      end
    end


    class Slash < Stream
      SLASH_SX = DUST_TIME + 1
      SLASH_SY = DUST_TIME + 2
      SLASH_W  = DUST_TIME + 3
      def initialize(color = [255, 0, 0], amount = 1, max = 30)
        super
      end
      
      # Sets up an individual particle. Overide this in child clases.
      def init_particle()
        x     = 0.0
        y     = 0.0
        vx    = 0.5
        vy    = 1
        g     = 0.00000001
        # Speeds set randomly for this kind of stream          
        time  = 0 # rand(100)
        # Randomly shorten their lifetime
        return Stardust.make_particle(x, y, vx, vy, time, 0.0, 0.0, 1)
      end
      
      def update_particle(particle, time)
        time = particle[DUST_TIME]
        # particle[BLOOD_G]
        # particle[DUST_VY] = 2 if particle[DUST_VY] > -2
        if time > @timemax
          return false
        end     
        return true     
      end  
     
      # Draws a single particle. 
      # Override this in child classes.
      def render_particle(x, y, surface, particle, ci) 
        dx = x - particle[DUST_X]
        dy = y - particle[DUST_Y]
        sx = dx + particle[SLASH_SX]
        sy = dy + particle[SLASH_SY]
        sw = particle[SLASH_W]
        surface.draw_line(x, y, sx, sy, ci, true)
        surface.draw_line(x, y+sw, sx, sy+sw, ci, true)
        surface.draw_line(x, y-sw, sx, sy-sw, ci, true)
      end
    end

    
  end
end
