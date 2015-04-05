

# Nowvalue is a mix in module module used to model values that have a current 
# value "now" 
# The class this is miked in must respond_to? :make(value) and :clamp(value).
module Nowvalue
  attr_reader :now
  
  def initalize(n)
    @now = n
  end
  
  def now=(value)
    @now = clamp(value)
  end
  
  def +(value)
    return make(@now + value)
  end
  
  def *(value)
    return make(@now * value)
  end
  
  def -(value)
    return make(@now - value)
  end

  def /(value)
    return make(@now / value)
  end
  
  def **(value)
    return make(@now ** value)
  end
  
  def <=>(value)
    return @now <=> value
  end

end

