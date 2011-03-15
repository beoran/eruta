module Bump
  # A 2D planar mathematical vector. This class is immutable.  
  class Vector
    attr_reader :x
    attr_reader :y
    
    def initialize(x, y)
      @x, @y = x.to_f, y.to_f
    end 
    
    # Checks if a vector is zero. 
    # @Returns [true, false] True if zero, false if not.
    def zero?
      return @x == 0.0 && @y == 0.0
    end
      
    # Returns a zero vector.
    def self.zero
      return self.new(0.0, 0.0)
    end
    
    # Returns a zero vector for ease of use.
    def zero
      return self.zero
    end
    
    
    # Constructs a vector with the given components, for ease of use
    def vec(x, y)
      return self.class.new(x, y)
    end
      
    # Compares two vectors for strict equality.
    def ==(other)
      return @x == other.x && @y == other.y
    end 
      
    # Compares two vectors and sees if their components are within delta of each 
    # other.      
    def equal_delta(other, delta = 0.001)
      return true   if self.equal(other)
      delta = delta.abs
      return false  unless other.x.abs.between?(@x.abs, @x.abs + delta)
      return other.y.abs.between?(@y.abs, @y.abs + delta)
    end
    
    def -@
      return vec(-@x, -@y)
    end
    
    # Vector addition.      
    def +(other)
      return vec(@x + other.x, @y + other.y)
    end
    
    # Vector substraction.     
    def -(other)
      return vec(@x - other.x, @y - other.y)
    end
    
    
    # Scalar Vector multiplication. 
    def *(factor)
      return vec(@x * factor, @y * factor)
    end
    
    # Scalar Vector division 
    def / (factor)
      return vec(@x / factor, @y / factor)
      #/
    end
    
    # Vector dot product. A scalar result.    
    def dot(other)
      return self.x * other.x + self.y * other.y    
    end

    # Cross product magnitude. The cross product of 2D x,y vectors is
    # a 3D vector with a pure z component, so this function returns only the     
    # magnitude of that z component.  
    def cross(other)
      return self.x * other.y - self.y * other.x    
    end
    
    
    # Perpendicular vector, rotated by 90 degrees, anticlockwise.
    def perp
      return vec(-@y, @x);
    end
    # Perpendicular vector, rotated by -90 degrees, clockwise.
    def nperp
      return vec(@y, -@x);
    end
    
    # Returns the dot poduct of the vector and itself. 
    def dotself
      return @x * @x + @y * @y
    end
       
    # Returns the squared length of the vector. Same as dotself, really. 
    # Useful for comparing lengths when speed is of importance.
    alias :lensq :dotself
    
    # Returns the length (or magnitude) of the vector.
    def length 
      return Math.sqrt(self.dotself)
    end
    
    # Returns a vector that is the projection of self onto other.
    def project(other)
      return other.mul(self.dot(other) / other.dotself)
    end
      
    # Rotates self by other. */
    def rotate(other)
      return vec(self.x*other.x - self.y*other.y, 
                 self.x*other.y + self.y*other.x);
    end
 
    # Inverse rotation.
    def unrotate(other)
      return vec(self.x*other.x + self.y*other.y, 
                 self.x*other.y - self.y*other.x);
    end
    
    # Linear interpolation on a line between between self and other. 
    # Returns a vector that points to a point on the line between self and other.
    def lerp(other, tx)
      self * (1.0 - tx) + (other * tx)
    end    
    
    # Returns self normalized. Will raise an exception in case length of vector 
    # is 0.
    def normalize_unsafe! 
      len = self.length  
      return vec(self.x / len, self.y / len);
    end

    # Returns self normalized. Will retuen a zero vector in case the length 
    # of the vector is 0.
    def normalize
      return zero if @x == 0.0 && @y == 0.0
      return normalize_unsafe! 
    end
    
    # If the length of the vector is greater than max, this function 
    # returns a vector with the same direction as self, but with the length 
    # of that vector limited to max. Otherwise, it returns self.  
    def max(m) 
      len  = self.length
      return self if (len <= m) 
      return self.normalize * m
    end  
      
    # Returns the distance between self and other. 
    def distance(other) 
      return (self - other).len
    end  

    # Returns the squared distance between self and other
    def distsq(other)
      return (self- other).distsq
    end  

    # Returns true if the distance between self and other is less than or 
    # equal to d.
    def near?(other, d)
      return self.distsq(other) <= (d*d)
    end
    
    # Retuns a unit vector that makes the given angle a with the X axis.
    def self.for_angle(a)
      return self.new(Math.cos(a), Math.sin(a))
    end  

    # Returns the angle of the vector with the X axis in clockwise 
    # direction in radians
    def angle
      return Math.atan2(self.y, self.x);
    end
    
    # Returns a vector with components self.x, 0
    def vector_x
      return vec(@x, 0.0)
    end
    
    # returns the overlap of the x component of self, of other, where  
    # self and other are centers of line pieces with width rself and rother 
    # respectivly. Returned as a vector with only an x component with the 
    # magnitude of overlap
    def overlap_x(other, rself, rother) 
      xself   = self.vector_x
      xother  = other.vector_x
      return zero unless xself.near(xother, rself)
       
    end
      


  end # Class vector
end



