

# Anything that has a name and a detailed description (details) is a Named
# It has also to_sym methods that will translate the name 
# to a symbol, replacing spaces by underscores
class Named
  attr_reader   :name
  attr_reader   :detail
  
  
  def initialize(name, detail="")
    @name     = name
    @detail   = detail
  end
  
  def to_sym
    return @name.gsub(' ', '_').downcase.to_sym
  end


end



