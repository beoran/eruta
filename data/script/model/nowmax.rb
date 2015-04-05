
script 'model/nowvalue'

# Nowmax is a class used to model values that have a current value "now" and 
# a maximum value "max", such as HP, NP, etc... 
# 0 is considered the minimum value for each of these
class Nowmax
  include Nowvalue

  attr_reader :max
  
  def initalize(n, m)
    super(n)
    @max = m
  end
  
  # Clamps value between 0 and max
  def clamp(value)
    return @max if value > @max
    return 0 if value < 0
    return value
  end
  
  # Makes a new Nowmax value with the given value and self.max as maximum.
  def make(value)
    return self.class.new(clamp(value), self.max)
  end

end




