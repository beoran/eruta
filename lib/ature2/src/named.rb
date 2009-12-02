

# Anything that has a name and a detailed description (details) is a Named
# It has also to_sym methods that will translate the name 
# to a symbol, replacing spaces by underscores
class Named
  attr_reader   :name
  attr_reader   :details
  
  
  def initialize(name, details="")
    @name     = name
    @detail   = details
  end
  
  def to_sym
    return @name.gsub(' ', '_').to_sym
  end


end



