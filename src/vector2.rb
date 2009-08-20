
class Vector2

	# Constructs a new Vector2 object with the given x and y components.
	def initialize(x, y)
		@x = x
		@y = y
	end
	
	# Read only attributes. The class is immutable.
	attr_reader :x
	attr_reader :y

    # Alternate constructor; same as Vector2.new.
	def self.[] (x,y)
		return self.new(x,y)
	end

	# Constructor en masse. Creates a new Vector2 object for each [x,y] Array given, 
	# and returns them in a new Array in the order they were given. 
	def self.ify(*args)
		# Do it in the naive way, because it's faster.
		result = []
		args.each  do | pair |
			result.push self.new(pair[0], pair[1])
		end
		return result
	end
	
	# Return a copy with the x and y components multiplied by the given scalar (a float or integer).
	def *(scalar)
		return self.class.new(@x * scalar, @y * scalar)
	end

  	# Return the vector sum with the other vector.
    def +( other )
		return self.class.new(@x + other.x, @y + other.y)
	end

    #  Return the vector difference with the other vector.
    def -( other )
		return self.class.new(@x - other.x, @y - other.y)
	end
	# Return a copy with the x and y components negated.
    def -@
		return self.class.new(-@x, -@y)
	end

	# Return a copy which has the same angle, but a magnitude of 1.0.
	def unit
		m = magnitude()
		return self.class.new( @x / m, @y / m)
	end

    # Return the vector dot product (aka inner product) with the other vector. 
    def dot( other )
		return (@x * other.x) + (@y * other.y)
   	end

	# Returns the unit dot product. Like dot, but using the unit vectors of self and other. 
	def udot(other)
		return self.unit.dot(other.unit)
	end

	# Returns the magnitude of the receiver.
    def magnitude
		return (@x * @x + @y * @y) ** 0.5
	end

	# Returns the magnitude squared of the receiver.
    def magnitude_squared
		return (@x * @x + @y * @y)
	end

	# Return the difference in angles (radians) between this Ftor and +other+.
	def angle_with(other)
		Math.acos( self.udot(other) )
	end

	# Return the angle from the positive X axis, in radians.
	def angle
		return Math.atan2(@y, @x)
	end
	
	# Return a copy which has been rotated by pi/2 radians. 
    def perp
		return self.class.new(-@y, @x)
	end
    
	# Return a new vector which is the receiver projected onto the other vector.
    def projected_onto( other )
		return other * other.dot(self) * ( 1.0 / other.magnitude_squared )
	end

  	# Convert to Array: [x,y]
    def to_ary
		return [@x, @y]
	end
    
	# Convert to Point2: Point2
	def to_p
		return Point2.new(@x, @y)
	end

  	# Convert to Vector2 (return self)
   	def to_v
		return self
	end

	#  Create a new instance of Transform passing in the description, 
	# apply it to the this vector, and return the result.
	# XXX: implement this correctly
	def transformed( description )
		transform = Transform.new(description)
	end
	
end

