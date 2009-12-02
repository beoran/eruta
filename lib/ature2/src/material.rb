
# Any item is made out of at least one material
require 'named'

class Material < Named
  
  # Agregation states
  STATES = { :solid, :liquid, :gas, :plasma }
  
  # Current aggregation state
  attr_reader :state
  # Mass per volume
  attr_reader :density
  # Hardness determines piecing damage of weapons, and resistance to piercing
  attr_reader :hardness
  # Toughness determines resistance to damage
  attr_reader :toughness
  

  def initialize(name, details="")
    super(name, details)
  end
  
  
end
