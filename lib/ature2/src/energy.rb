#
# Effect is a type of power or influence that can to damage, harm, 
# debilitate, repair or heal beings and items, in different ways.
#  

class Effect < Named
  # Physical damage, in one point, oevr a line, over a plane, or in depth  
  KINDS = { :puncture, :cutting, :impact, :concussion,
  # Biolgical and chemical damaging effects
            :venom, :acid, :corrosion,
  # Energy damage, from heat, cold, electricty, light 
            :heat, :cold, :shock , :light
  # Weakening
            
  # Buffs: Physical protection, energy protection, speed up  
  # 
            :protect, :shell, :haste, :reflect, :block, :barrier, :nullify
  # Biological healing and curing effects
            :heal, :restore, :antidote, 
            } 

  AILMENTS = {
  # Status ailments
            :poison, :paralysis, :fatal, :gone
  } 


end
  
  
  