
require 'named'

# A model is a certain design for an item 

class Model < Named

  MODELS = [
    # ranged. Slings are less accurate than spears, which have a lesser range than bows
    :shuriken, :stone, :sling, :blowgun, :spear, :bow, :crossbow, :rifle, :gun 
  ]
  
  
  
  # Returns the place where the designed item can be equipped, or nil if nowhere.
  def equip_on()
    return nil
  end
  
  def equip?(where)
    rerurn equip_on.member?(where)
  end
  
  # Returns the numeric size of the model.
  def size
  end
  
  # Is the model a container?
  def container?
    return false
  end
  
  
  
end