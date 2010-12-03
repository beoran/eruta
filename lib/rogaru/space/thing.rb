module Rogaru
  class Space
    class Thing < Body
      attr_accessor :index
      attr_accessor :x
      attr_accessor :y
      attr_accessor :w
      attr_accessor :h            
      attr_accessor :z
      attr_accessor :space
      
      def camera_x
        return @x.to_i        
      end
      
      def camera_y
        return @y.to_i        
      end
      
      
      def initialize(space, x, y, z, w, h)
        @space = space;
        @x = x; @y = y; @z = z     
        @w = w; @h = h
        @color = [255,255,0]
        @markedcolor = [255,0,0]
        @marked = false   
        @index = -1
      end
      
      def mark!
        @marked=true
      end
      
      def render(screen, offset_x, offset_y)
        x = @x + offset_x 
        y = @y + offset_y
        # puts "#@x #@y #@w #@h #@color"
        color = @marked ? @markedcolor : @color 
        screen.put_rectangle(x, y, @w, @h, color)
        @marked=false          
      end
      
      # Things do nothing with dicplacement given.
      def give_displacement(dix, diy)
        return  
      end
      
      # Relative speed of motion towards other thing (if that thng can move)
      def relative_speed(other)
        othervx = (other.respond_to?(:vx) ? other.vx : 0)
        othervy = (other.respond_to?(:vy) ? other.vy : 0)
        vrelx   = othervx - @vx
        vrely   = othervy - @vy
        return vrelx, vrely
      end
      
      def nx
        @x
      end 
       
      def ny
        @y
      end   
      
      def nx=(x)
        @x = x
      end 
       
      def ny=(y)
        @y = y
      end   

      def can_be_pushed?(pusher)
        return false
      end
      
    end # class Thing
  end # class Space
end # module Rogaru
